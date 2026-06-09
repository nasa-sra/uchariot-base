#pragma once

#include "spdlog/spdlog.h"
#include <memory>
#include "uchariot_logger.h"

class uchariot_logger {
    //TODO: make logger available for different levels of logging and available for both cpp and cmake files
    public:
        static void init() {
            GetLogger()->set_level(spdlog::level::info);
        }
        //TODO add more logging functions for different levels of logging (debug, error, etc.)
        template<typename... Args>
        static void LogFmt(const char* fmt, const Args&... args) {
            GetLogger()->info(fmt, args...);
        } 
        static void info(const std::string& msg) {
            GetLogger()->info(msg);
        }
        static void error(const std::string& errmsg) {
            GetLogger()->error(errmsg);
        }

        static std::shared_ptr<spdlog::logger> GetLogger() {
            static std::shared_ptr<spdlog::logger> logger = spdlog::basic_logger_mt("uchariot_logger", "uchariot.log");
            return logger;
        }

        //TODO: add function for periodic logging
        




}