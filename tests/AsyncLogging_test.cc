#include "../AsyncLogging.h"
#include <string.h>
#include <unistd.h>

std::string name("jlog");
off_t size = 400 * 1000;  // 40KB
const char* logline = "this is the asynclogging test\n";
int n = strlen(logline);

int main() {
    AsyncLogging LOG(name, size);

    // 缓冲区很大，所以其实都是写入了缓冲区
    // 3s测试--OK，32KB
    for (int i = 0; i < 10000; i++) {
        LOG.append(logline, n);
        usleep(1000);
    }
    // 普通测试--OK
    for (int i = 0; i < 10000; i++) {
        LOG.append(logline, n);
        usleep(1000);
    }
    //! 滚动测试--ERROR
    for (int i = 0; i < 10000; i++) {
        LOG.append(logline, n);
        // 睡1ms
        usleep(1000);
    }
    return 0;
}