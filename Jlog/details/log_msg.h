#pragma once

#include <string>
#include "Jlog/common.h"
#include "Jlog/details/os.h"

namespace Jlog {

namespace details {

struct LogMessage {
    LogMessage() = default;
    // use std::string_view as argument, it is faster than std::string and can
    // both accept the std::string and const char * as interface
    LogMessage(SourceLoc loc, string_view_t logger_name_in, level::level_enum lvl,
               string_view_t msg);
    LogMessage(string_view_t logger_name, level::level_enum lvl,
               string_view_t msg);
    LogMessage(const LogMessage &other) = default;

    string_view_t logger_name;
    level::level_enum level = level::k_off;
    log_clock::time_point time; 
    size_t thread_id = 0;
    SourceLoc source;
    string_view_t payload;
};

inline LogMessage::LogMessage(Jlog::SourceLoc loc, string_view_t logger_name_in,
                              Jlog::level::level_enum lvl,
                              Jlog::string_view_t msg)
    : logger_name(logger_name_in),
      level(lvl),
      time(os::now()),
      thread_id(getThreadId()),
      source(loc),
      payload(msg) {}

inline LogMessage::LogMessage(string_view_t logger_name_in,
                              Jlog::level::level_enum lvl,
                              Jlog::string_view_t msg)
    : LogMessage(SourceLoc{}, logger_name_in, lvl, msg) {}

}  // namespace details

}  // namespace Jlog
