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

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include "LogStream.h"

const int roll_size = 40000; ///< 设定日志文件的滚动大小

class Logger {
public:
    Logger(const char* fileName, int line);
    ~Logger();
    LogStream& stream() { return impl_.stream_; }

    static void setLogFileName(std::string filename) {
        log_filename_ = filename;
    }

    static std::string getLogFileName() { return log_filename_; }

private:
    //? 这也不是pimpl，不知道专门搞个类是想干嘛
    class Impl {
    public:
        Impl(const char* filename, int line);
        ~Impl() = default;

        void formatTime();

        LogStream stream_;
        int line_;
        std::string basename_;
    };
    Impl impl_;
    off_t roll_size_;
    static std::string log_filename_;
};

#define LOG Logger(__FILE__, __LINE__).stream();