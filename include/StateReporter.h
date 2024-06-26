#pragma once

#include <string>
#include <map>
#include <fstream>
#include <chrono>

class StateReporter {
public:

    static StateReporter& GetInstance() {
        static StateReporter _instance;
        return _instance;
    }

    void UpdateKey(std::string key, double val);
    void PushState();

    void SetLogging(bool logging) {_logging = logging;}
    void Close();

private:

    StateReporter();

    bool initLogFile();
    void logState();

    std::map<std::string, double> _state;

    bool _logging{false};
    int _logVarCount{0};
    std::ofstream _logFile;

    std::chrono::time_point<std::chrono::steady_clock> _startTime;

};