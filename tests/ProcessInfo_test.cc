#include "ProcessInfo.h"
#include <iostream>

int main() {
    pid_t pid = ProcessInfo::pid();
    std::cout << "The pid is: " << pid << std::endl;
    std::string hostname = ProcessInfo::hostname();
    std::cout << hostname << std::endl;
    return 0;
}
