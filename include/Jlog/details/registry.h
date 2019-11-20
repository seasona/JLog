#pragma once

#include <chrono>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include "Jlog/common.h"
#include "Jlog/details/noncopyable.h"
#include "Jlog/logger.h"

namespace Jlog {

namespace details {

class Registry : public noncopyable {
public:
    // return raw ptr of default Logger
    Logger* getDefaultRaw();

    void setDefaultLogger(std::shared_ptr<Logger> new_default_logger);

    static Registry& instance();

private:
    Registry();
    ~Registry() = default;

    std::shared_ptr<Logger> default_logger_;
    std::unordered_map<std::string, std::shared_ptr<Logger>> loggers_;
    level::level_enum level_ = level::k_info;
    level::level_enum flush_level_ = level::k_off;
};

inline Logger* Registry::getDefaultRaw() { return default_logger_.get(); }

inline Registry& Registry::instance() {
    static Registry s_instance;
    return s_instance;
}

void Registry::setDefaultLogger(std::shared_ptr<Logger> new_default_logger) {
    if (default_logger_ != nullptr) {
        loggers_.erase(default_logger_->getName());
    }
    if (new_default_logger != nullptr) {
        loggers_[new_default_logger->getName()] = new_default_logger;
    }
    default_logger_ = std::move(new_default_logger);
}

}  // namespace details

}  // namespace Jlog
