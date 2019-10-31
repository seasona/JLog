#include "AsyncLogging.h"
#include <string.h>
#include <unistd.h>
#include <chrono>
#include <thread>
#include "Logging.h"

std::string name("JLog");
off_t size = 40 * 1000 * 1000;  // 40MB
const char* logline = "This is the asynclogging test\n";
int len = strlen(logline);
JLog::AsyncLogging logger(name, size);

void func(int howmany, int threads_count) {
    for (int i = 0; i < howmany / threads_count; i++) {
        logger.append(logline, len);
        // usleep(1);
    }
}


int main(int argc, char** argv) {
    if (argc < 3) {
        printf("usage: {progarm} <howmay> <threads_count>\n");
        exit(-1);
    }
    int howmany = atoi(argv[1]);
    int threads_count = atoi(argv[2]);

    std::vector<std::thread> threads;

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < threads_count; i++) {
        threads.emplace_back(std::thread(func, howmany, threads_count));
    }

    for (auto& x : threads) {
        x.join();
    }
    auto delta = std::chrono::high_resolution_clock::now() - start;
    auto delta_d =
        std::chrono::duration_cast<std::chrono::duration<double>>(delta)
            .count();

    // delta_d -= howmany / 1000000;

    printf("Benchmark: %lf s  %d iterations  %lf iter/s. \n", delta_d, howmany,
           static_cast<double>(howmany / delta_d));
    return 0;
}