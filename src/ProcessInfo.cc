#include "ProcessInfo.h"
#include <unistd.h>

pid_t ProcessInfo::pid() { return getpid(); }

std::string ProcessInfo::hostname() {
    char buf[256];
    if (gethostname(buf, sizeof(buf)) == 0) {
        buf[sizeof(buf) - 1] = '\0';
        return std::string(buf);
    } else {
        return std::string("unknowhost");
    }
}
