#pragma once

#include <chrono>
#include <fstream>
#include <map>
#include <string>
#include <thread>
#include <vector>

#include "rapidjson/document.h"

#include "NetworkManager.h"

class StateReporter {
   public:
    static StateReporter& GetInstance() {
        static StateReporter _instance;
        return _instance;
    }

    void UpdateKey(std::string key, bool val);
    void UpdateKey(std::string key, int val);
    void UpdateKey(std::string key, double val);
    void UpdateKey(std::string key, std::string val);

    void PushState();
    void EnableLogging();
    void EnableTelemetry(NetworkManager* network);
    void Close();

   private:
    struct ValueEntry {
        enum ValueType { NULLVALUE, BOOL, INT, DOUBLE, STRING };

        ValueType valueType;
        bool boolValue;
        int intValue;
        double doubleValue;
        std::string stringValue;

        ValueEntry() : valueType(NULLVALUE) {}
        ValueEntry(bool _value) : boolValue(_value), valueType(BOOL) {}
        ValueEntry(int _value) : intValue(_value), valueType(INT) {}
        ValueEntry(double _value) : doubleValue(_value), valueType(DOUBLE) {}
        ValueEntry(std::string _value)
            : stringValue(_value), valueType(STRING) {}

        void setJsonValue(rapidjson::Value* json,
                          rapidjson::Document::AllocatorType& allocator);
        rapidjson::Value getGenericValue(
            rapidjson::Document::AllocatorType& allocator);
        std::string toString();
    };

    struct TreeNode {
        std::string name;
        std::vector<TreeNode*> branches;
        bool fruit = true;
        rapidjson::Value* json = nullptr;
        ValueEntry value;

        TreeNode() : fruit(false) {}
        TreeNode(std::string _name) : name(_name), fruit(false) {}
        TreeNode(std::string _name, ValueEntry _value)
            : name(_name), value(_value) {}
    };

    StateReporter();

    void genericUpdateKey(std::string key, ValueEntry val);
    void addKey(std::string key, ValueEntry val);
    TreeNode* climbTree(TreeNode* current, std::string key);
    void deleteTree(TreeNode* node);
    void buildDoc(rapidjson::Value& doc, TreeNode* current,
                  rapidjson::Document::AllocatorType& allocator);
    rapidjson::Value::MemberIterator climbDoc(rapidjson::Value& doc,
                                              std::string key);

    bool initLogFile();
    void logState();
    void sendState();

    std::map<std::string, TreeNode*> _state;
    TreeNode* _treeRoot;
    rapidjson::Document _doc;

    bool _logging{false};
    int _logVarCount{0};
    std::ofstream _logFile;
    std::chrono::time_point<std::chrono::steady_clock> _startTime;

    bool _stateRefreshed{false};
    bool _telemetry{false};
    std::thread _telemetryThread;
    NetworkManager* _network{nullptr};
};