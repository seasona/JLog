#include "bq_AsyncLogging.h"
#include <string.h>

namespace JLog {

void BqAsyncLogging::threadFunc() {
    LogFile output(basename_, roll_size_);
    std::string logline;
    while (bq_.popFor(logline, std::chrono::milliseconds(3))) {
        output.append(logline.c_str(), logline.size());
    }
}

BqAsyncLogging::BqAsyncLogging(const std::string& basename, off_t roll_size,
                               size_t bq_size, int flush_interval)
    : basename_(basename),
      roll_size_(roll_size),
      bq_(bq_size),
      flush_interval_(flush_interval),
      thread_(std::bind(&BqAsyncLogging::threadFunc, this)) {}

BqAsyncLogging::~BqAsyncLogging() {
    thread_.join();
}

void BqAsyncLogging::append(const char* logline, int len) {
    bq_.push(std::string(logline));
    //bq_.pushNoWait(std::string(logline));
}

}  // namespace JLog