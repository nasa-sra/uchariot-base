
#include <sys/stat.h>

#include <cmath>

#define RAPIDJSON_HAS_STDSTRING 1
#include "NetworkManager.h"
#include "StateReporter.h"
#include "Utils.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

StateReporter::StateReporter() {
    _startTime = std::chrono::steady_clock::now();  // Sets start time
    _treeRoot = new TreeNode("robot");              // Creates root node
}

void StateReporter::UpdateKey(std::string key, bool val) {
    genericUpdateKey(key, ValueEntry(val));
}

void StateReporter::UpdateKey(std::string key, int val) {
    genericUpdateKey(key, ValueEntry(val));
}

void StateReporter::UpdateKey(std::string key, double val) {
    if (std::isfinite(val)) {
        val = (int(val * 1000 + 0.5)) / 1000.0;
        genericUpdateKey(key, ValueEntry(val));
    }
}

void StateReporter::UpdateKey(std::string key, std::string val) {
    genericUpdateKey(key, ValueEntry(val));
}

void StateReporter::PushState() {
    _stateRefreshed = true;
    if (_logging) {
        if (_logFile.is_open()) {
            logState();
        } else {
            if (initLogFile()) logState();
        }
    }
}

void StateReporter::EnableLogging() { _logging = true; }

void StateReporter::EnableTelemetry() {
    _telemetry = true;
    _telemetryThread = std::thread(&StateReporter::sendState, this);
}

void StateReporter::Close() {
    _telemetry = false;
    _telemetryThread.join();
    deleteTree(_treeRoot);
    _logging = false;
    if (_logFile.is_open()) _logFile.close();
}

/**
 * @brief Sets a rapidjson::Value to the value stored
 *
 * @param json The rapidjson::Value to be set.
 * @param allocator The allocator to be used for creating rapidjson strings
 *
 */
void StateReporter::ValueEntry::setJsonValue(
    rapidjson::Value* json, rapidjson::Document::AllocatorType& allocator) {
    switch (valueType) {
        case BOOL:
            json->SetBool(boolValue);
            break;
        case INT:
            json->SetInt(intValue);
            break;
        case DOUBLE:
            json->SetDouble(doubleValue);
            break;
        case STRING:
            json->SetString(stringValue.c_str(), stringValue.length(),
                            allocator);
            break;
        default:
            break;
    }
}

/**
 * @brief Retrieves a rapidjson::Value object representing the stored value.
 *
 * This function returns a rapidjson::Value object based on the type of the
 * stored value. It uses the provided allocator to create the rapidjson::Value
 * object.
 *
 * @param allocator The allocator to be used for creating the rapidjson::Value
 * object.
 *
 * @return A rapidjson::Value object representing the stored value.
 *
 * @note The returned rapidjson::Value object will be of type
 * rapidjson::kTrueType, rapidjson::kFalseType, rapidjson::kNumberType, or
 * rapidjson::kStringType, depending on the type of the stored value.
 */
rapidjson::Value StateReporter::ValueEntry::getGenericValue(
    rapidjson::Document::AllocatorType& allocator) {
    switch (valueType) {
        case BOOL:
            return rapidjson::Value(boolValue);
            break;
        case INT:
            return rapidjson::Value(intValue);
            break;
        case DOUBLE:
            return rapidjson::Value(doubleValue);
            break;
        case STRING:
            return rapidjson::Value(stringValue, allocator);
            break;
        default:
            break;
    }
    return rapidjson::Value(0);
}

std::string StateReporter::ValueEntry::toString() {
    switch (valueType) {
        case BOOL:
            return boolValue ? "true" : "false";
        case INT:
            return std::to_string(intValue);
        case DOUBLE:
            return std::to_string(doubleValue);
        case STRING:
            return stringValue;
        default:
            break;
    }
    return "";
}

void StateReporter::genericUpdateKey(std::string key,
                                     StateReporter::ValueEntry val) {
    auto it = _state.find(key);
    if (it == _state.end()) {
        addKey(key, val);
    } else {
        it->second->value = val;
        if (it->second->json != nullptr)
            val.setJsonValue(it->second->json, _doc.GetAllocator());
    }
}

/**
 * @brief Adds a new key-value pair to the state tree and associates it with a
 * TreeNode.
 *
 * This function takes a key and a ValueEntry as input, climbs the state tree to
 * find the appropriate branch, creates a new TreeNode with the value name
 * extracted from the key, and adds it to the branch. The key-value pair is also
 * stored in the _state map for quick access.
 *
 * @param key The key to be added to the state tree.
 * @param val The ValueEntry to be associated with the key.
 *
 * @return void
 */
void StateReporter::addKey(std::string key, ValueEntry val) {
    TreeNode* branch = climbTree(_treeRoot, key.substr(1));
    std::string valueName;
    for (int i = key.size() - 1; i >= 0; i--) {
        if (key[i] == '/') {
            valueName = key.substr(i + 1);
            break;
        }
    }
    TreeNode* node = new TreeNode(valueName, val);
    branch->branches.push_back(node);
    _state[key] = node;
}

/**
 * @brief Climbs the state tree to find the appropriate branch based on the
 * given key.
 *
 * This function traverses the state tree starting from the given current node,
 * using the key to determine the next branch to visit. If the key contains
 * multiple branches, it will recursively call itself to climb deeper into the
 * tree. If the key does not exist in the tree, a new branch will be created and
 * added to the current node.
 *
 * @param current The current node in the state tree to start climbing from.
 * @param key The key representing the path to the desired branch in the state
 * tree.
 *
 * @return A pointer to the final branch node in the state tree, based on the
 * given key.
 */
StateReporter::TreeNode* StateReporter::climbTree(TreeNode* current,
                                                  std::string key) {
    int delim = key.find("/");
    if (delim == key.npos) {
        return current;
    }
    std::string nextBranch = key.substr(0, delim);
    auto it = std::find_if(
        current->branches.begin(), current->branches.end(),
        [&nextBranch](TreeNode* n) { return n->name == nextBranch; });
    if (it == current->branches.end()) {
        current->branches.push_back(new TreeNode(nextBranch));
        it = current->branches.end() - 1;
    }
    return climbTree(*it, key.substr(delim + 1));
}

/**
 * @brief Recursively deletes a tree of TreeNode objects.
 *
 * This function deletes a tree of TreeNode objects starting from the given
 * node. If the node is a fruit node (i.e., it has no branches), it is simply
 * deleted. If the node is a branch node, the function recursively calls itself
 * to delete all its child branches before deleting the node itself.
 *
 * @param node The root node of the tree to be deleted.
 *
 * @return void
 */
void StateReporter::deleteTree(TreeNode* node) {
    if (node->fruit) {
        delete node;
        return;
    }
    for (TreeNode* branch : node->branches) {
        deleteTree(branch);
    }
    delete node;
}

/**
 * @brief Recursively builds a JSON document from a tree of TreeNode objects.
 *
 * This function traverses a tree of TreeNode objects starting from the given
 * current node, and constructs a JSON document using the rapidjson library. If
 * the current node is a fruit node (i.e., it has no branches), it adds a member
 * to the JSON document with the node's name and value. If the current node is a
 * branch node, it creates a new JSON object as a member of the JSON document,
 * and recursively calls itself to build the JSON document for each child
 * branch.
 *
 * @param doc The rapidjson::Value object representing the JSON document to be
 * built.
 * @param current The TreeNode object representing the current node in the tree.
 * @param allocator The rapidjson::Document::AllocatorType object used to
 * allocate memory for the JSON document.
 *
 * @return void
 */
void StateReporter::buildDoc(rapidjson::Value& doc, TreeNode* current,
                             rapidjson::Document::AllocatorType& allocator) {
    rapidjson::Value name(current->name.c_str(), current->name.size());
    if (current->fruit) {
        doc.AddMember(name, current->value.getGenericValue(allocator),
                      allocator);
        return;
    }
    rapidjson::Value docBranch(rapidjson::kObjectType);
    for (TreeNode* treeBranch : current->branches) {
        buildDoc(docBranch, treeBranch, allocator);
    }
    doc.AddMember(name, docBranch, allocator);
}

/**
 * @brief Recursively climbs a JSON document to find a specific member.
 *
 * This function traverses a JSON document represented by a rapidjson::Value
 * object, starting from the given current node. It uses the provided key to
 * determine the next member to visit. If the key contains multiple members, it
 * will recursively call itself to climb deeper into the JSON document. If the
 * key does not exist in the JSON document, it logs an error message using the
 * Utils::LogFmt function.
 *
 * @param doc The rapidjson::Value object representing the JSON document to be
 * climbed.
 * @param key The key representing the path to the desired member in the JSON
 * document.
 *
 * @return A rapidjson::Value::MemberIterator pointing to the desired member in
 * the JSON document. If the member is not found, it returns doc.MemberEnd().
 */
rapidjson::Value::MemberIterator StateReporter::climbDoc(rapidjson::Value& doc,
                                                         std::string key) {
    int delim = key.find("/");
    if (delim == key.npos) {
        return doc.FindMember(key.c_str());
    }
    std::string nextBranch = key.substr(0, delim);
    auto member = doc.FindMember(nextBranch.c_str());
    if (member == doc.MemberEnd()) {
        Utils::LogFmt("StateReporter::climbDoc - Failed to find branch %s",
                      nextBranch.c_str());
    }
    return climbDoc(member->value, key.substr(delim + 1));
}

/**
 * @brief Sends the current state of the robot to a network destination.
 *
 * This function continuously sends the current state of the robot to a network
 * destination at a specified rate. It builds a JSON document from the state
 * tree, sets the JSON document value pointers in the state tree, and then sends
 * the JSON document over the network.
 *
 * @return void
 *
 * @note This function runs in an infinite loop until the telemetry is disabled.
 * It checks the _stateRefreshed flag to determine whether the state has been
 * updated since the last send. If the state has not been updated, it sleeps for
 * 1 millisecond before checking again. It also uses a rate limiting mechanism
 * to ensure that the sending rate does not exceed the specified rate. If the
 * sending rate is exceeded, it logs an error message.
 */
void StateReporter::sendState() {
    while (!_stateRefreshed && _telemetry) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    _doc.SetObject();
    rapidjson::Document::AllocatorType& allocator = _doc.GetAllocator();
    buildDoc(_doc, _treeRoot, allocator);  // Build JSON document from data tree

    // Set JSON document value pointers in data tree.
    for (auto i = _state.begin(); i != _state.end(); i++) {
        i->second->json =
            &climbDoc(_doc.FindMember("robot")->value, i->first.substr(1))
                 ->value;
    }

    rapidjson::StringBuffer strbuf;

    auto start_time = std::chrono::high_resolution_clock::now();
    while (_telemetry) {
        start_time = std::chrono::high_resolution_clock::now();

        while (!_stateRefreshed && _telemetry) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        strbuf.Clear();
        // rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(strbuf);
        rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
        _doc.Accept(writer);

        NetworkManager::GetInstance().SendAll(strbuf.GetString(),
                                              strbuf.GetSize());

        int rate = 10;  // Hz
        double dt = Utils::ScheduleRate(rate, start_time);
        if (dt > 1.0 / rate) {
            Utils::LogFmt("StateReporter sendState overran by %f s", dt);
        }
        _stateRefreshed = false;
    }
}

bool StateReporter::initLogFile() {
    struct stat sb;
    if (stat("./logs", &sb) != 0) {
        if (mkdir("./logs/", 0777) != 0) {
            Utils::LogFmt("StateReporter - Failed to create logs directory");
            return false;
        }
    }

    std::string logPath =
        "./logs/" + Utils::CurrentDateTimeStr("%Y-%m-%d_%H%M%S") + ".csv";
    _logFile.open(logPath);
    if (!_logFile.is_open()) {
        Utils::LogFmt("StateReporter - Failed to open log file at %s",
                      logPath.c_str());
        _logging = false;
        return false;
    }

    std::string header = "time, ";
    for (auto i = _state.begin(); i != _state.end(); i++) {
        header += i->first + ", ";
    }
    header = header.substr(0, header.size() - 2);
    _logFile << header << '\n';
    _logVarCount = _state.size();

    return true;
}

void StateReporter::logState() {
    if (_logVarCount != _state.size()) {
        Utils::LogFmt(
            "StateReporter - logState:variables have been added to the state "
            "after logging init");
        return;
    }

    std::string row = "";
    double time = std::chrono::duration_cast<std::chrono::milliseconds>(
                      std::chrono::steady_clock::now() - _startTime)
                      .count() /
                  1000.0;
    row += std::to_string(time) + ", ";

    for (auto i = _state.begin(); i != _state.end(); i++) {
        row += i->second->value.toString() + ", ";
    }
    row = row.substr(0, row.size() - 2);
    _logFile << row << '\n';
}
