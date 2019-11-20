#pragma once
#include "Jlog/details/log_msg.h"
#include "Jlog/fmt/fmt.h"

namespace Jlog {

class Formatter {
public:
    Formatter() = default;
    ~Formatter() = default;
    virtual void format(const details::LogMessage &msg, memory_buf_t &dest) = 0;
};

}  // namespace Jlog