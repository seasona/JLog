#include "Logging.h"
#include <sys/time.h>
#include <time.h>
#include "AsyncLogging.h"

static pthread_once_t once_control = PTHREAD_ONCE_INIT;
static AsyncLogging* async_logger;

std::string Logger::log_filename_ = "/JLog.log";

void once_init() {
    async_logger = new AsyncLogging(Logger::getLogFileName(), roll_size);
}

void output(const char* msg, int len) {
    pthread_once(&once_control, once_init);
    async_logger->append(msg, len);
}

Logger::Impl::Impl(const char* filename, int line)
    : line_(line), basename_(filename) {
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
    stream_ << str_t;
}

Logger::Logger(const char* filename, int line) : impl_(filename, line) {}

Logger::~Logger() {
    // 写入格式为时间--源文件:行号\n正文
    // 写入源文件：行号
    impl_.stream_ << "--" << impl_.basename_ << ":" << impl_.line_ << "\n";
    const LogStream::Buffer& buf(this->stream().buffer());
    output(buf.data(), buf.length());
}