#include "Logging.h"
#include <time.h>
#include <sstream>
#include "AsyncLogging.h"
#include "os.h"

namespace JLog {

std::string Logger::log_filename_("JLog");
Logger::LogLevel Logger::level_(Logger::INFO);

const char* log_level_name[Logger::NUM_LOG_LEVELS] = {
    "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL",
};

/*
// 使用异步写入数据，确保只有一个AsyncLogging单例类
void outputAsync(const char* msg, int len) {
    AsyncLogging::getInstance(Logger::getLogFileName(), roll_size)
        ->append(msg, len);
}
*/

void outputDefault(const char* msg, int len) { fwrite(msg, 1, len, stdout); }

Logger::OutputFunc Logger::output_func_(outputDefault);

Logger::Impl::Impl(const char* filename, int line, LogLevel level,
                   const char* func)
    : line_(line), basename_(filename), level_(level), func_(func) {
    stream_ << "[" << log_level_name[level_] << "] ";
    formatTime();
}

// TODO 主要耗时在这里，需要对时间和线程id分别进行缓存优化
void Logger::Impl::formatTime() {
    char str_t[26] = {0};
    std::tm p_time = OS::localTime(::time(NULL));
    strftime(str_t, 26, "%Y-%m-%d %H:%M:%S", &p_time);
    stream_ << static_cast<int>(OS::getThreadId()) << " ";
    stream_ << str_t << "-( ";
}

Logger::Logger(const char* filename, int line)
    : impl_(filename, line, INFO, NULL) {}

Logger::Logger(const char* filename, int line, LogLevel level)
    : impl_(filename, line, level, NULL) {}

Logger::Logger(const char* filename, int line, LogLevel level, const char* func)
    : impl_(filename, line, level, func) {}

Logger::~Logger() {
    impl_.stream_ << " )-" << impl_.basename_ << ":" << impl_.line_;
    if (impl_.func_) {
        impl_.stream_ << ":" << impl_.func_ << "()\n";
    } else {
        impl_.stream_ << "\n";
    }
    const LogStream::Buffer& buf(this->stream().buffer());
    output_func_(buf.data(), buf.length());
}

}  // namespace JLog