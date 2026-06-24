#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "Utils.h"
#include "NetworkManager.h"
#include <memory>

class uchariot_logger {
    private: 
        std::string _current_file;
        std::shared_ptr<spdlog::sinks::basic_file_sink_mt> _file_sink;

    public:
        void uchariot_logger::init() {
            std::filesystem::create_directories("logs");

            _current_file = Utils::CurrentDateTimeStr("%Y-%m-%d_%H-%M-%S") + ".log";

            _file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(_current_file, true);
            
            auto logger =
                std::make_shared<spdlog::logger>(
                    "uchariot",
                    _file_sink);

            
            
            spdlog::set_default_logger(logger);
        }

        void file_events(const std::string& fmt, ...) {
            va_list args;
            va_start(args, fmt);
            _file_sink->log(spdlog::source_loc{}, spdlog::level::info, fmt.c_str(), args);
            va_end(args);
        }

        // ------------------------------------------------------
        // Private uchariot_logger Functions 
        // ------------------------------------------------------

        // A constant expression function that converts std::string
        // types to char* C strings in variatic argument lists. The
        // function works by evaulating if a given type is of the
        // std::string type. If so, it returns the result of .c_str(),
        // otherwise, it just returns the param. This is designed to
        // be used in variatic argument comprehention.
        template <typename T>
        auto _convert(T&& t) {
            if constexpr (std::is_same<std::remove_cv_t<std::remove_reference_t<T>>,
                                    std::string>::value)
                return std::forward<T>(t).c_str();
            else
                return std::forward<T>(t);
}
        // Internal function to perform a C snprintf style format
        // string. The problem with this function, the reason it's
        // the internal function, is that it cannot take std::string
        // types as arguments, only char* C strings. Wrapping this
        // function solves this problem. 
        template <typename... A>
        std::string _strfmt(const std::string& fmt, A&&... args) {
            const auto size =
                _snprintf(nullptr, 0, fmt.c_str(), std::forward<A>(args)...) + 1;
            if (size <= 0) return "<StrFmt error>";
            std::unique_ptr<char[]> buf(new char[size]);
            _snprintf(buf.get(), size, fmt.c_str(), args...);
            return std::string(buf.get(), buf.get() + size - 1);
        }

        // Gets the time in ms
        // msec_t time_ms(void)
        // {
        //     struct timeval tv;
        //     gettimeofday(&tv, NULL);
        //     return (msec_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;
        // }

        // ------------------------------------------------------
        // Logging functions to uchariot-console
        // ------------------------------------------------------

        // Formats a string the same way C snprintf does it,
        // returning the formatted string. This method can take
        // std::string objects as arguments (for the %s format).
        template <typename... A>
        std::string StrFmt(const std::string& fmt, A&&... args) {
            return _strfmt(fmt, _convert(std::forward<A>(args))...);
        }

        // Prints a string the same way C printf does it, however
        // this method is modernized for C++, and can use std::string
        // objects as arguments (for the %s format).
        template <typename... A>
        void PrintFmt(const std::string& fmt, A&&... args) {
            std::cout << _strfmt(fmt, _convert(std::forward<A>(args))...);
        }

        // The same as PrintFmt but has a newline appended to the end.
        template <typename... A>
        void PrintLnFmt(const std::string& fmt, A&&... args) {
            std::cout << _strfmt(fmt + "\n", _convert(std::forward<A>(args))...);
        }

        // This one also prints the time at the beginning b/c its a log. 
        template <typename... A>
        void LogFmt(const std::string& fmt, A&&... args) {
            file_events("[" + Utils::CurrentDateTimeStr() + "] " + fmt, Utils::_convert(std::forward<A>(args))...);
            std::cout << _strfmt("[" + Utils::CurrentDateTimeStr() + "] " + fmt + "\n",
                                _convert(std::forward<A>(args))...);
        }

        // Handle errors. 
        template <typename... A>
        void ErrFmt(const std::string& fmt, A&&... args) {
            std::cerr << _strfmt("[" + Utils::CurrentDateTimeStr() + "] " + fmt + "\n",
                                _convert(std::forward<A>(args))...);
            std::exit(1);
        }


}