/**
 * @file Logging.h
 * @author 季杰 (396438446@qq.com)
 * @brief JLog的接口
 * @version 0.1
 * @date 2019-08-16
 *
 * @copyright Copyright (c) 2019
 *
 */

#pragma once

#include <stdio.h>
#include <string.h>
#include <string>
#include "LogStream.h"

namespace JLog {

const int roll_size = 4000000;  ///< 设定日志文件的滚动大小（默认4MB）

/**
 * @brief JLog的API，可以设立日志文件的滚动大小、日志文件的基本名称、
 * 日志级别的输出
 *
 */
class Logger {
public:
    enum LogLevel {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERR,
        FATAL,
        NUM_LOG_LEVELS,
    };

    Logger(const char* filename, int line);
    Logger(const char* filename, int line, LogLevel level);
    Logger(const char* filename, int line, LogLevel level, const char* func);
    ~Logger();

    LogStream& stream() { return impl_.stream_; }

    /**
     * @brief 设立日志文件的基本名
     *
     * @param filename 日志文件基本名
     */
    static void setLogFileName(std::string filename) {
        log_filename_ = filename;
    }

    static std::string getLogFileName() { return log_filename_; }

    /**
     * @brief 设立日志的输出级别
     *
     * @param level 输出级别，高于该级别的能够被输出
     */
    static void setLogLevel(LogLevel level) { level_ = level; }

    inline static LogLevel getLogLevel() { return level_; }

private:
    //? 这也不是pimpl，不知道专门搞个类是想干嘛
    class Impl {
    public:
        Impl(const char* filename, int line, LogLevel level, const char* func_);
        ~Impl() = default;

        void formatTime();

        LogLevel level_;
        LogStream stream_;
        int line_;
        std::string basename_;
        const char* func_;
    };
    Impl impl_;
    static std::string log_filename_;  ///< 日志文件基本名
    static LogLevel level_;            ///< 日志输出级别
};

}  // namespace JLog


// 超过设立的日志级别的日志可以输出，比如设立日志级别为INFO，
// 则INFO、 WARN、ERROR、FATAL可以输出

#define LOG_TRACE                                           \
    if (JLog::Logger::getLogLevel() <= JLog::Logger::TRACE) \
    JLog::Logger(__FILE__, __LINE__, JLog::Logger::TRACE, __func__).stream()

#define LOG_DEBUG                                           \
    if (JLog::Logger::getLogLevel() <= JLog::Logger::DEBUG) \
    JLog::Logger(__FILE__, __LINE__, JLog::Logger::DEBUG, __func__).stream()

#define LOG_INFO                                           \
    if (JLog::Logger::getLogLevel() <= JLog::Logger::INFO) \
    JLog::Logger(__FILE__, __LINE__).stream()

#define LOG_WARN                                           \
    if (JLog::Logger::getLogLevel() <= JLog::Logger::WARN) \
    JLog::Logger(__FILE__, __LINE__, JLog::Logger::WARN).stream()

// JLog::Logger::ERROR会与MSVC中的全局宏ERROR冲突，所以替换为ERR
#define LOG_ERROR                                           \
    if (JLog::Logger::getLogLevel() <= (JLog::Logger::ERR)) \
    JLog::Logger(__FILE__, __LINE__, (JLog::Logger::ERR)).stream()

#define LOG_FATAL                                           \
    if (JLog::Logger::getLogLevel() <= JLog::Logger::FATAL) \
    JLog::Logger(__FILE__, __LINE__, JLog::Logger::FATAL).stream()
