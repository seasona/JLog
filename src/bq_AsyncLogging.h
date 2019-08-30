#include <atomic>
#include <mutex>
#include <thread>
#include "LogFile.h"
#include "blocking_queue.h"

namespace JLog {

class BqAsyncLogging : public noncopyable {
public:
    BqAsyncLogging(const std::string& basename, off_t roll_size,
                   size_t bq_size = 8192, int flush_interval = 3);
    ~BqAsyncLogging();

    void append(const char* logline, int len);

private:
    BlockingQueue<std::string> bq_;
    const int flush_interval_;
    const off_t roll_size_;
    const std::string basename_;

    void threadFunc();

    std::thread thread_;
    std::mutex mutex_;
};

}  // namespace JLog