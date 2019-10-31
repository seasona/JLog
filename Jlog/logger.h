#pragma once

#include <string>
#include "Jlog/common.h"
#include "Jlog/details/log_msg.h"

#define JLOG_LOGGER_TRY try
#define JLOG_LOGGER_CATCH()                           \
    catch (const std::exception &ex) {                \
        err_exception(ex.what());                     \
    }                                                 \
    catch (...) {                                     \
        err_exception("Unknown exception in Logger"); \
    }

namespace Jlog {

class Logger {
public:
    explicit Logger(std::string name) : name_(std::move(name)), sinks_() {}

    template <typename It>
    Logger(std::string name, It begin, It end)
        : name_(std::move(name)), sink_(begin, end) {}

    virtual ~Logger() = default;

    Logger(const Logger &other);
    Logger(const Logger &&other);
    Logger(Logger &&other) noexcept;
    Logger &operator=(Logger other) noexcept;

    void swap(Logger &other) noexcept;

    // format with args and save into LogMessage then write into log
    template <typename... Args>
    void log(SourceLoc loc, level::level_enum lvl, string_view_t fmt,
             const Args &... args) {
        if (!shouldLog(lvl)) {
            return;
        }
        JLOG_LOGGER_TRY {
            memory_buf_t buf;
            // use the format `fmt` to output args into buffer `buf`
            fmt::format_to(buf, fmt, args...);
            // create the LogMessage with the formated string buffer
            details::LogMessage log_msg(loc, name_, lvl,
                                        string_view_t(buf.data(), buf.size()));
            // write the LogMessage into log
            logIt(log_msg);
        }
        JLOG_LOGGER_CATCH()
    }

    template <typename... Args>
    void log(level::level_enum lvl, string_view_t fmt, const Args &... args) {
        log(SourceLoc{}, lvl, fmt, args...);
    }

    template <typename... Args>
    void trace(string_view_t fmt, const Args &... args) {
        log(level::k_trace, fmt, args...);
    }

    template <typename... Args>
    void debug(string_view_t fmt, const Args &... args) {
        log(level::k_debug, fmt, args...);
    }

    template <typename... Args>
    void info(string_view_t fmt, const Args &... args) {
        log(level::k_info, fmt, args...);
    }

    template <typename... Args>
    void warn(string_view_t fmt, const Args &... args) {
        log(level::k_warn, fmt, args...);
    }

    template <typename... Args>
    void error(string_view_t fmt, const Args &... args) {
        log(level::k_err, fmt, args...);
    }

    template <typename... Args>
    void critical(string_view_t fmt, const Args &... args) {
        log(level::k_critical, fmt, args...);
    }

    // SFINAE, argument T can static transform to string_view
    template <class T,
              typename std::enable_if<
                  std::is_convertible<const T &, Jlog::string_view_t>::value,
                  T>::type * = nullptr>
    void log(SourceLoc loc, level::level_enum lvl, const T &msg) {
        details::LogMessage log_msg(loc, name_, lvl, msg);
        log_it_(log_msg);
    }

    // SFINAE, argument T can't static transform to string_view and wstring_view
    template <
        class T,
        typename std::enable_if<
            !std::is_convertible<const T &, Jlog::string_view_t>::value &&
                !std::is_convertible<const T &, Jlog::wstring_view_t>::value,
            T>::type * = nullptr>
    void log(SourceLoc loc, level::level_enum lvl, const T &msg) {
        log(loc, lvl, "{}", msg);
    }

    template <typename T>
    void log(level::level_enum lvl, const T &msg) {
        log(SourceLoc{}, lvl, msg);
    }

    template <typename T>
    void trace(const T &msg) {
        log(level::k_trace, msg);
    }

    template <typename T>
    void debug(const T &msg) {
        log(level::k_debug, msg);
    }

    template <typename T>
    void info(const T &msg) {
        log(level::k_info, msg);
    }

    template <typename T>
    void warn(const T &msg) {
        log(level::k_warn, msg);
    }

    template <typename T>
    void error(const T &msg) {
        log(level::k_err, msg);
    }

    template <typename T>
    void critical(const T &msg) {
        log(level::k_critical, msg);
    }

    bool shouldLog(level::level_enum log_level);

    void setLevel(level::level_enum log_level);

    level::level_enum getLevel() const;

    const std::string &getName() const;

    // void setFormatter(std::unique_ptr<format>)

    // flush functions
    void flush();
    void flushOn(level::level_enum log_level);
    level::level_enum flushLevel() const;

    const std::vector<sink_ptr> &getSinks() const;

    void setErrorHandler(err_handler new_err);

    // virtual std::shared_ptr<Logger> clone(std::string logger_name);

protected:
    std::string name_;
    std::vector<sink_ptr> sinks_;

    level_t level_{level::k_info};
    level_t flush_level_{level::k_off};
    err_handler custom_err_handler_{nullptr};

    void logIt(const details::LogMessage &log_msg);
    virtual void sinkIt(const details::LogMessage &msg);
    virtual void flush();
    bool should_flush(const details::LogMessage *msg);

    // handle errors during logging
    void err_exception(const std::string &msg);
};

inline const std::string &Logger::getName() const { return name_; }

inline Logger::Logger(const Logger &other)
    : name_(other.name_),
      sinks_(other.sinks_),
      level_(other.level_.load(std::memory_order_relaxed)),
      flush_level_(other.flush_level_.load(std::memory_order_relaxed)) {}

inline Logger::Logger(const Logger &&other)
    : name_(std::move(other.name_)),
      sinks_(std::move(other.sinks_)),
      level_(std::move(other.level_.load(std::memory_order_relaxed))),
      flush_level_(
          std::move(other.flush_level_.load(std::memory_order_relaxed))) {}

// use swap to change value, you can use reference parameter here
inline void Logger::swap(Logger &other) noexcept {
    name_.swap(other.name_);
    sinks_.swap(other.sinks_);

    auto other_level = other.level_.load();
    auto my_level = level_.exchange(other_level);
    other.level_.store(my_level);

    other_level = other.flush_level_.load();
    my_level = flush_level_.exchange(other_level);
    other.level_.store(my_level);
}

// use copy-and-swap assignment, you don't want to change the value of parameter
// passed in, so the parameter should be a copy, not a reference. its benefit is
// you don't need to determinate if the parameter is itself, but copy-and-swap
// would always cause a reallocation so its performance maybe lower than the non
// copy-and-swap assignment
inline Logger &Logger::operator=(Logger other) noexcept { this->swap(other); }

}  // namespace Jlog