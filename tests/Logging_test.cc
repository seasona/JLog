/**
 * @brief 一次性写入没办法进行文件滚动，实际上也不会有一次性写入大数量日志的情况,
 * 每次写入日志都会有一些间隔，所以使用usleep模拟
 * 
 */

#include "../Logging.h"
#include <unistd.h>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

void threadFunc() {
    for (int i = 0; i < 100000; ++i) {
        LOG_INFO << i;
        usleep(100);
    }
}

void type_test() {
    // 13 lines
    std::cout << "----------type test-----------" << std::endl;
    LOG_DEBUG << 0;
    LOG_TRACE << 123456789;
    LOG_INFO << 1.0f;
    LOG_INFO << 3.1415926;
    LOG_WARN << (short)1;
    LOG_WARN << (long long)1;
    LOG_ERROR << (unsigned int)1;
    LOG_FATAL << (unsigned long)1;
    LOG_INFO << (long double)1.6555556;
    LOG_INFO << (unsigned long long)1;
    LOG_ERROR << 'c';
    LOG_INFO << "abcdefg";
    LOG_INFO << std::string("This is a string");
}

void stressing_single_thread() {
    // 100000 lines
    std::cout << "----------stressing test single thread-----------"
              << std::endl;
    for (int i = 0; i < 100000; ++i) {
        LOG_INFO << i;
        usleep(100);
    }
}

void stressing_multi_threads(int threadNum = 4) {
    // threadNum * 100000 lines
    std::cout << "----------stressing test multi thread-----------"
              << std::endl;
    std::vector<std::shared_ptr<std::thread>> vsp;
    for (int i = 0; i < threadNum; ++i) {
        std::shared_ptr<std::thread> tmp(new std::thread(threadFunc));
        vsp.push_back(tmp);
    }
    for (int i = 0; i < threadNum; ++i) {
        vsp[i]->join();
    }
    sleep(3);
}

void other() {
    // 1 line
    std::cout << "----------other test-----------" << std::endl;
    LOG_INFO << "fddsa" << 'c' << 0 << 3.666 << std::string("This is a string");
}

int main() {
    JLog::Logger::setLogFileName("test");
    JLog::Logger::setLogLevel(JLog::Logger::TRACE);
    // 共500014行
    type_test();
    sleep(3);

    stressing_single_thread();
    sleep(3);

    other();
    sleep(3);

    stressing_multi_threads();
    sleep(3);
    return 0;
}