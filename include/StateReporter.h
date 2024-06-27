#pragma once

#include <string>
#include <map>
#include <fstream>
#include <chrono>
#include <thread>
#include <vector>

#include "rapidjson/document.h"

class StateReporter {
public:

    static StateReporter& GetInstance() {
        static StateReporter _instance;
        return _instance;
    }

    void UpdateKey(std::string key, double val);
    void PushState();
    void EnableLogging();
    void EnableTelemetry();
    void Close();

private:

    struct TreeNode {
        std::string name;
        std::vector<TreeNode*> branchs;
        bool fruit;
        double value;
        rapidjson::Value* json;
        TreeNode() : fruit(false), json(nullptr) {}
        TreeNode(std::string _name) : name(_name), fruit(false), json(nullptr){}
        TreeNode(std::string _name, double _value) : name(_name), fruit(true), value(_value), json(nullptr) {}
    };

    StateReporter();

    void addKey(std::string key, double val);
    TreeNode* climbTree(TreeNode* current, std::string key);
    void deleteTree(TreeNode* node);
    void buildDoc(rapidjson::Value& doc, TreeNode* current, rapidjson::Document::AllocatorType& allocator);
    rapidjson::Value::MemberIterator climbDoc(rapidjson::Value& doc, std::string key);

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

};