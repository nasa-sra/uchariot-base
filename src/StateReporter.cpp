
#include <sys/stat.h>

#include "Utils.h"
#include "StateReporter.h"

StateReporter::StateReporter() {
    _startTime = std::chrono::steady_clock::now();
}

void StateReporter::UpdateKey(std::string key, double val) {
    _state[key] = val;
}

void StateReporter::PushState() {
    if (_logging) {
        if (_logFile.is_open()) {
            logState();
        } else {
            if (initLogFile())
                logState();
        }
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
        row += std::to_string(i->second) + ", ";
    }
    row = row.substr(0, row.size() - 2);
    _logFile << row << '\n';

}

void StateReporter::Close() {
    if (_logFile.is_open())
        _logFile.close();
}