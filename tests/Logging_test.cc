#include "Logging.h"
#include "AsyncLogging.h"
#include "LogFile.h"

#include <stdio.h>
#include <sys/resource.h>
#include <unistd.h>
#include <chrono>
#include <string>
#include <thread>

off_t kRollSize = 100 * 1000 * 1000;  // 100MB

JLog::AsyncLogging* g_asyncLog = NULL;
JLog::LogFile* g_logFile = NULL;

void asyncOutput(const char* msg, int len) { g_asyncLog->append(msg, len); }

void dummyOutput(const char* msg, int len) { g_logFile->append(msg, len); }

void func(int kBatch) {
    for (int i = 0; i < kBatch; ++i) {
        LOG_INFO << "Hello 0123456789"
                 << " abcdefghijklmnopqrstuvwxyz ";
    }
}

void benchAsync(bool longLog) {
    JLog::Logger::setOutput(asyncOutput);

    int howmany = 1000000;
    int threads_count = 10;

    std::vector<std::thread> threads;

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < threads_count; i++) {
        threads.emplace_back(std::thread(func, howmany / threads_count));
    }

    for (auto& x : threads) {
        x.join();
    }
    auto delta = std::chrono::high_resolution_clock::now() - start;
    auto delta_d =
        std::chrono::duration_cast<std::chrono::duration<double>>(delta)
            .count();

    printf("Benchmark: %lf s  %d iterations  %lf iter/s. \n", delta_d, howmany,
           static_cast<double>(howmany / delta_d));
}

void benchDummy(bool longLog) {
    JLog::Logger::setOutput(dummyOutput);

    int cnt = 0;
    const int kBatch = 1000000;
    std::string empty = " ";
    std::string longStr(3000, 'X');
    longStr += " ";

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < kBatch; ++i) {
        LOG_INFO << "Hello 0123456789"
                 << " abcdefghijklmnopqrstuvwxyz "
                 << (longLog ? longStr : empty) << cnt;
        ++cnt;
    }
    auto delta = std::chrono::high_resolution_clock::now() - start;
    auto delta_d =
        std::chrono::duration_cast<std::chrono::duration<double>>(delta)
            .count();
    printf("Benchmark: %lf s  %d iterations  %lf iter/s. \n", delta_d, kBatch,
           static_cast<double>(kBatch / delta_d));
}

int main(int argc, char* argv[]) {
    printf("pid = %d\n", getpid());

    char name[256] = {0};
    strncpy(name, argv[0], sizeof name - 1);

    printf("=====================normal=====================\n");
    JLog::LogFile log(::basename(name), kRollSize);
    g_logFile = &log;
    bool longLog = argc > 1;
    benchDummy(longLog);

    printf("===============async (10 threads)================\n");
    JLog::AsyncLogging alog(::basename(name), kRollSize);
    g_asyncLog = &alog;
    benchAsync(longLog);

    return 0;
}
