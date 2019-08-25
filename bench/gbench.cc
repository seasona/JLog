#include <benchmark/benchmark.h>
#include <Logging.h>
#include <unistd.h>

static void BM_JLog(benchmark::State& state){
    for(auto _: state){
        LOG_INFO<<"This is a test";
        // 3208 ns per iteration
        // 311720 iteration per second
        // 32MB / 338737 iterations
        // 30MB/s
    }
}

BENCHMARK(BM_JLog);
BENCHMARK_MAIN();
