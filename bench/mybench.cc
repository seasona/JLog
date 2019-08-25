#include <Logging.h>
#include <chrono>

//Benchmark: 313872 iterations per second.
// 95B * 313872 = 28.43MiB/s
int main(){
    int howmany=1000000;
    auto start = std::chrono::high_resolution_clock::now();
    for(int i=0;i<howmany;i++){
        LOG_INFO<<"JLog benchmark";
    }
    auto delta = std::chrono::high_resolution_clock::now() - start;
    auto delta_d = std::chrono::duration_cast<std::chrono::duration<double>>(delta).count();
    printf("Benchmark: %d iterations per second.\n", static_cast<int>(howmany/delta_d));

    return 0;
}
