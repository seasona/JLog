#include "Logging.h"
#include <time.h>
#include <sstream>
#include "AsyncLogging.h"

namespace JLog {

AsyncLogging* async_logger;
std::string Logger::log_filename_("JLog");
Logger::LogLevel Logger::level_(Logger::INFO);

const char* log_level_name[Logger::NUM_LOG_LEVELS] = {
    "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL",
};

// 异步写入数据，确保只有一个AsyncLogging单例类
void output(const char* msg, int len) {
    AsyncLogging::getInstance(Logger::getLogFileName(), roll_size)
        ->append(msg, len);
}

Logger::Impl::Impl(const char* filename, int line, LogLevel level,
                   const char* func)
    : line_(line), basename_(filename), level_(level), func_(func) {
    stream_ << "[" << log_level_name[level_] << "] ";
    formatTime();
}

void Logger::Impl::formatTime() {
    time_t t = time(NULL);
    char str_t[26] = {0};
    struct tm* p_time = localtime(&t);
    strftime(str_t, 26, "%Y-%m-%d %H:%M:%S", p_time);
    std::stringstream sin;
    sin << std::this_thread::get_id();
    stream_ << sin.str()<<" ";
    stream_ << str_t << "-( ";
}

Logger::Logger(const char* filename, int line)
    : impl_(filename, line, INFO, NULL) {}

Logger::Logger(const char* filename, int line, LogLevel level)
    : impl_(filename, line, level, NULL) {}

Logger::Logger(const char* filename, int line, LogLevel level, const char* func)
    : impl_(filename, line, level, func) {}

// 写入格式为：
// [DEBUG] 2019-08-19 14:58:29-( 0
// )-/home/jie/JLog/tests/Logging_test.cc:25:type_test()
Logger::~Logger() {
    impl_.stream_ << " )-" << impl_.basename_ << ":" << impl_.line_;
    if (impl_.func_) {
        impl_.stream_ << ":" << impl_.func_ << "()\n";
    } else {
        impl_.stream_ << "\n";
    }
    const LogStream::Buffer& buf(this->stream().buffer());
    output(buf.data(), buf.length());
}

}  // namespace JLog