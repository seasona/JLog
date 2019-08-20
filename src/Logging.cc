#include "Logging.h"
#include <sys/time.h>
#include <time.h>
#include "AsyncLogging.h"

namespace JLog {

static pthread_once_t once_control = PTHREAD_ONCE_INIT;
static AsyncLogging* async_logger;
std::string Logger::log_filename_("JLog");
Logger::LogLevel Logger::level_(Logger::INFO);

const char* log_level_name[Logger::NUM_LOG_LEVELS] = {
    "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL",
};

void once_init() {
    async_logger = new AsyncLogging(Logger::getLogFileName(), roll_size);
}

// 异步写入数据，确保只有一个AsyncLogging单例类
void output(const char* msg, int len) {
    pthread_once(&once_control, once_init);
    async_logger->append(msg, len);
}

Logger::Impl::Impl(const char* filename, int line, LogLevel level,
                   const char* func)
    : line_(line), basename_(filename), level_(level), func_(func) {
    stream_ << "[" << log_level_name[level_] << "] ";
    formatTime();
}

void Logger::Impl::formatTime() {
    struct timeval tv;
    time_t time;
    char str_t[26] = {0};
    gettimeofday(&tv, NULL);
    time = tv.tv_sec;
    struct tm* p_time = localtime(&time);
    strftime(str_t, 26, "%Y-%m-%d %H:%M:%S", p_time);
    stream_ << str_t << "-( ";
}

Logger::Logger(const char* filename, int line)
    : impl_(filename, line, INFO, NULL) {}

Logger::Logger(const char* filename, int line, LogLevel level)
    : impl_(filename, line, level, NULL) {}

Logger::Logger(const char* filename, int line, LogLevel level, const char* func)
    : impl_(filename, line, level, func) {}

Logger::~Logger() {
    // 写入格式为：
    // [DEBUG] 2019-08-19 14:58:29-( 0 )-/home/jie/JLog/tests/Logging_test.cc:25:type_test()
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