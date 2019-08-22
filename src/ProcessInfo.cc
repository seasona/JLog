#include "ProcessInfo.h"

#ifdef _MSC_VER
    long ProcessInfo::pid(){return GetCurrentProcessId();}
#else
    pid_t ProcessInfo::pid() { return getpid(); }
#endif

std::string ProcessInfo::hostname() {
    char buf[256];
    /*if (gethostname(buf, sizeof(buf)) == 0) {
        buf[sizeof(buf) - 1] = '\0';
        return std::string(buf);
    } else {
        return std::string("unknowhost");
    }*/

    return std::string("unknowhost");

}
