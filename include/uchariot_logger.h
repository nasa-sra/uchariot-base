#pragma once

#include "spdlog/spdlog.h"
#include "utils.h"
#include "NetworkManager.h"
#include <memory>
#include "uchariot_logger.h"

class uchariot_logger {
    public:
        static void init() {
            GetLogger()->set_level(spdlog::level::info);
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

        // This one also prints the time at the beginning b/c its a log
        template <typename... A>
        void LogFmt(const std::string& fmt, A&&... args) {
            std::cout << _strfmt("[" + CurrentDateTimeStr() + "] " + fmt + "\n",
                                _convert(std::forward<A>(args))...);
        }

        // Handle errors.
        template <typename... A>
        void ErrFmt(const std::string& fmt, A&&... args) {
            std::cerr << _strfmt("[" + CurrentDateTimeStr() + "] " + fmt + "\n",
                                _convert(std::forward<A>(args))...);
            std::exit(1);
        }

        // TODO: create a new file for each run and adds a date and timestamp to the filename
        // TODO: have the logger write to the currently running file in the logs directory
        // TODO: have the logs sent off to 
}