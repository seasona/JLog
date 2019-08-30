#include "bq_AsyncLogging.h"
#include "Logging.h"

#include <stdio.h>
#include <chrono>
#include <string>
#include <thread>

off_t kRollSize = 100 * 1000 * 1000;  // 100MB

JLog::BqAsyncLogging *g_bqLog = nullptr;

void bqOutput(const char* msg, int len) { g_bqLog->append(msg, len); }

void func(int kBatch) {
    for (int i = 0; i < kBatch; ++i) {
        LOG_INFO << "Hello 0123456789"
                 << " abcdefghijklmnopqrstuvwxyz ";
    }
}

void benchAsync(bool longLog) {
    JLog::Logger::setOutput(bqOutput);

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

int main(int argc, char* argv[]) {
    char name[256] = {0};
    strncpy(name, argv[0], sizeof name - 1);

    printf("===============bq_async (10 threads)================\n");
    JLog::BqAsyncLogging bqlog(::basename(name), kRollSize);
    g_bqLog = &bqlog;
    benchAsync(false);

    return 0;
}