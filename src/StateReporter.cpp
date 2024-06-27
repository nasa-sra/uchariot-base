
#include <sys/stat.h>

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"

#include "Utils.h"
#include "NetworkManager.h"
#include "StateReporter.h"

StateReporter::StateReporter() {
    _startTime = std::chrono::steady_clock::now();
}

void StateReporter::UpdateKey(std::string key, double val) {
    _state[key] = val;
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
    _logging = false;
    if (_logFile.is_open())
        _logFile.close();
}

void StateReporter::sendState() {

    while(!_stateRefreshed) { std::this_thread::sleep_for(std::chrono::milliseconds(1)); }

    // TODO: Actually format JSON doc
    rapidjson::Document doc;
    doc.SetObject();
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
    for (auto i = _state.begin(); i != _state.end(); i++) {
        rapidjson::Value name(i->first.c_str(), i->first.size());
        doc.AddMember(name, i->second, allocator);
    }

    auto start_time = std::chrono::high_resolution_clock::now();
    while(_telemetry) {
        start_time = std::chrono::high_resolution_clock::now();

        while(!_stateRefreshed) { std::this_thread::sleep_for(std::chrono::milliseconds(1)); }

        for (auto i = _state.begin(); i != _state.end(); i++) {
            auto member = doc.FindMember(i->first.c_str());
            // std::cout << i->first << " Json: " << member->value.GetDouble() << " Actual: " << i->second << "\n";
            member->value.SetDouble(i->second);
        }

        rapidjson::StringBuffer strbuf;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(strbuf);
        doc.Accept(writer);

        NetworkManager::GetInstance().SendAll(strbuf.GetString(), strbuf.GetSize());

        int overrun = Utils::ScheduleRate(1, start_time);
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
        row += std::to_string(i->second) + ", ";
    }
    row = row.substr(0, row.size() - 2);
    _logFile << row << '\n';

}