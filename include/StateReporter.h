#pragma once

#include <string>
#include <map>
#include <fstream>
#include <chrono>
#include <thread>

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

    StateReporter();

    bool initLogFile();
    void logState();
    void sendState();

    std::map<std::string, double> _state;

    bool _logging{false};
    int _logVarCount{0};
    std::ofstream _logFile;
    std::chrono::time_point<std::chrono::steady_clock> _startTime;

    bool _stateRefreshed{false};
    std::thread _telemetryThread;

};