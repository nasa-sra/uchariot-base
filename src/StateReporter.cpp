
#include <sys/stat.h>

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"

#include "Utils.h"
#include "NetworkManager.h"
#include "StateReporter.h"

StateReporter::StateReporter() {
    _startTime = std::chrono::steady_clock::now();
    _treeRoot = new TreeNode("robot");
}

void StateReporter::UpdateKey(std::string key, double val) {
    auto it = _state.find(key);
    if (it == _state.end()) {
        addKey(key, val);
    } else {
        it->second->value = val;
        if (it->second->json != nullptr)
            it->second->json->SetDouble(val);
    }
}

void StateReporter::PushState() {
    _stateRefreshed = true;
    if (_logging) {
        if (_logFile.is_open()) {
            logState();
        } else {
            if (initLogFile())
                logState();
        }
    }
}

void StateReporter::EnableLogging() {
    _logging = true;
}

void StateReporter::EnableTelemetry() {
    _telemetry = true;
    _telemetryThread = std::thread(&StateReporter::sendState, this);
}

void StateReporter::Close() {
    _telemetry = false;
    _telemetryThread.join();
    deleteTree(_treeRoot);
    _logging = false;
    if (_logFile.is_open())
        _logFile.close();
}

void StateReporter::addKey(std::string key, double val) {
    TreeNode* branch = climbTree(_treeRoot, key.substr(1));
    std::string valueName;
    for (int i = key.size() - 1; i >= 0; i--) {
        if (key[i] == '/') {
            valueName = key.substr(i + 1);
            break;
        }
    }
    TreeNode* node = new TreeNode(valueName, val);
    branch->branchs.push_back(node);
    _state[key] = node;
}

StateReporter::TreeNode* StateReporter::climbTree(TreeNode* current, std::string key) {
    int delim = key.find("/");
    if (delim == key.npos) {
        return current;
    }
    std::string nextBranch = key.substr(0, delim);
    auto it = std::find_if(current->branchs.begin(), current->branchs.end(), [&nextBranch](TreeNode* n){return n->name == nextBranch;});
    if (it == current->branchs.end()) {
        current->branchs.push_back(new TreeNode(nextBranch));
        it = current->branchs.end() - 1;
    }
    return climbTree(*it, key.substr(delim + 1));
}

void StateReporter::deleteTree(TreeNode* node) {
    if (node->fruit) {
        delete node;
        return;
    }
    for (TreeNode* branch : node->branchs) {
        deleteTree(branch);
    }
    delete node;
}

void StateReporter::buildDoc(rapidjson::Value& doc, TreeNode* current, rapidjson::Document::AllocatorType& allocator) {
    rapidjson::Value name(current->name.c_str(), current->name.size());
    if (current->fruit) {
        doc.AddMember(name, current->value, allocator);
        return;
    }
    rapidjson::Value docBranch(rapidjson::kObjectType);
    for (TreeNode* treeBranch : current->branchs) {
        buildDoc(docBranch, treeBranch, allocator);
    }
    doc.AddMember(name, docBranch, allocator);
}

rapidjson::Value::MemberIterator StateReporter::climbDoc(rapidjson::Value& doc, std::string key) {
    int delim = key.find("/");
    if (delim == key.npos) {
        return doc.FindMember(key.c_str());
    }
    std::string nextBranch = key.substr(0, delim);
    auto member = doc.FindMember(nextBranch.c_str());
    if (member == doc.MemberEnd()) {
        Utils::LogFmt("StateReporter::climbDoc - Failed to find branch %s", nextBranch.c_str());
    }
    return climbDoc(member->value, key.substr(delim + 1));
}

void StateReporter::sendState() {

    while(!_stateRefreshed) { std::this_thread::sleep_for(std::chrono::milliseconds(1)); }

    _doc.SetObject();
    rapidjson::Document::AllocatorType& allocator = _doc.GetAllocator();
    buildDoc(_doc, _treeRoot, allocator); // Build JSON document from data tree

    // Set JSON document value pointers in data tree
    for (auto i = _state.begin(); i != _state.end(); i++) {
        i->second->json = &climbDoc(_doc.FindMember("robot")->value, i->first.substr(1))->value;
    }

    rapidjson::StringBuffer strbuf;

    auto start_time = std::chrono::high_resolution_clock::now();
    while(_telemetry) {
        start_time = std::chrono::high_resolution_clock::now();

        while(!_stateRefreshed && _telemetry) { std::this_thread::sleep_for(std::chrono::milliseconds(1)); }

        strbuf.Clear();
        // rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(strbuf);
        rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
        _doc.Accept(writer);

        NetworkManager::GetInstance().SendAll(strbuf.GetString(), strbuf.GetSize());

        int overrun = Utils::ScheduleRate(10, start_time);
        if (overrun > 0) {
            Utils::LogFmt("StateReporter sendState overran by %i ms", overrun);
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

    std::string logPath = "./logs/" + Utils::CurrentDateTimeStr("%Y-%m-%d_%H%M%S") + ".csv";
    _logFile.open(logPath);
    if (!_logFile.is_open()) {
        Utils::LogFmt("StateReporter - Failed to open log file at %s", logPath.c_str());
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
        Utils::LogFmt("StateReporter - logState:variables have been added to the state after logging init");
        return;
    }

    std::string row = "";
    double time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - _startTime).count() / 1000.0;
    row += std::to_string(time) + ", ";

    for (auto i = _state.begin(); i != _state.end(); i++) {
        row += std::to_string(i->second->value) + ", ";
    }
    row = row.substr(0, row.size() - 2);
    _logFile << row << '\n';

}