#include "../ProcessInfo.h"
#include <gtest/gtest.h>
#include <iostream>

TEST(ProcessInfoTest, pidTest) {
    pid_t pid = ProcessInfo::pid();
    std::cout << "The pid is: " << pid << std::endl;
}

TEST(ProcessInfoTest, hostnameTest) {
    std::string hostname = ProcessInfo::hostname();
    EXPECT_STREQ(hostname.c_str(), "JIE");
}
