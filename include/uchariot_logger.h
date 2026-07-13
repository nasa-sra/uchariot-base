#pragma once

#include <filesystem>
#include <iostream>
#include <memory>
#include <string>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "Utils.h"

class uchariot_logger {
   private:
    inline static std::string _current_file;
    inline static std::shared_ptr<spdlog::sinks::basic_file_sink_mt> _file_sink;

    template <typename T>
    static auto _convert(T&& t) {
        if constexpr (std::is_same<std::remove_cv_t<std::remove_reference_t<T>>,
                                   std::string>::value)
            return std::forward<T>(t).c_str();
        else
            return std::forward<T>(t);
    }

    template <typename... A>
    static std::string _strfmt(const std::string& fmt, A&&... args) {
        const auto size = snprintf(nullptr, 0, fmt.c_str(), std::forward<A>(args)...) + 1;
        if (size <= 0) return "<StrFmt error>";
        std::unique_ptr<char[]> buf(new char[size]);
        snprintf(buf.get(), size, fmt.c_str(), args...);
        return std::string(buf.get(), buf.get() + size - 1);
    }

   public:
    static void init() {
        std::filesystem::create_directories("logs");
        _current_file = "logs/" + Utils::CurrentDateTimeStr("%Y-%m-%d_%H-%M-%S") + ".log";
        _file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(_current_file, true);
        auto logger = std::make_shared<spdlog::logger>("uchariot", _file_sink);
        spdlog::set_default_logger(logger);
        spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] %v");
    }

    template <typename... A>
    static std::string StrFmt(const std::string& fmt, A&&... args) {
        return _strfmt(fmt, _convert(std::forward<A>(args))...);
    }

    template <typename... A>
    static void PrintFmt(const std::string& fmt, A&&... args) {
        std::cout << _strfmt(fmt, _convert(std::forward<A>(args))...);
    }

    template <typename... A>
    static void PrintLnFmt(const std::string& fmt, A&&... args) {
        std::cout << _strfmt(fmt + "\n", _convert(std::forward<A>(args))...);
    }

    template <typename... A>
    static void LogFmt(const std::string& fmt, A&&... args) {
        std::string msg = _strfmt(fmt, _convert(std::forward<A>(args))...);
        spdlog::info("{}", msg);
        std::cout << "[" << Utils::CurrentDateTimeStr() << "] " << msg << "\n";
    }

    template <typename... A>
    static void ErrFmt(const std::string& fmt, A&&... args) {
        std::string msg = _strfmt(fmt, _convert(std::forward<A>(args))...);
        spdlog::critical("{}", msg);
        spdlog::shutdown();
        std::cerr << "[" << Utils::CurrentDateTimeStr() << "] " << msg << "\n";
        std::exit(1);
    }
};
