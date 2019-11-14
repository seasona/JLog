#pragma once

#include "Jlog/details/log_msg.h"

namespace Jlog {

namespace sinks {

class Sink {
public:
    virtual ~Sink() = default;
    virtual void log(const details::LogMessage &msg) = 0;
    virtual void flush() = 0;
    virtual void setPattern(const std::string &pattern) = 0;
    // virtual void setFormatter() = 0;

    void setLevel(level::level_enum log_level);
    level::level_enum getLevel() const;
    bool shouldLog(level::level_enum msg_level) const;

protected:
    level_t level_{level::k_trace};
};

/// inline must be placed before definition, useless put before declaration
inline bool Sink::shouldLog(Jlog::level::level_enum msg_level) const {
    /// load and store can get and set the value of std::atomic
    /// std::memory_order_relaxed means the operation is atomic but without
    /// sequence, usually use in condtion which only care about data result
    return msg_level >= level_.load(std::memory_order_relaxed);
}

inline void Sink::setLevel(level::level_enum log_level) {
    level_.store(log_level, std::memory_order_relaxed);
}

inline level::level_enum Sink::getLevel() const {
    return static_cast<Jlog::level::level_enum>(
        level_.load(std::memory_order_relaxed));
}

}  // namespace sinks

}  // namespace Jlog
