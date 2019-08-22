#include "AsyncLogging.h"
#include <string.h>
#include "sleep.h"

std::string name("jlog");
off_t size = 400 * 1000;  // 40KB
const char* logline = "this is the asynclogging test\n";
int n = strlen(logline);

#define LOG JLog::AsyncLogging::getInstance(name,size)

int main() {
    // 缓冲区很大，所以其实都是写入了缓冲区
    // 3s测试--OK，32KB
    for (int i = 0; i < 10000; i++) {
        LOG->append(logline, n);
        SLEEP(100);
    }
    // 普通测试--OK
    for (int i = 0; i < 10000; i++) {
        LOG->append(logline, n);
        SLEEP(100);
    }
    //! 滚动测试--ERROR
    for (int i = 0; i < 10000; i++) {
        LOG->append(logline, n);
        // 睡0.1ms
        SLEEP(100);
    }
    return 0;
}