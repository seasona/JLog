#include "../LogStream.h"

int main() {
    LogStream LOG;
    void* p = &LOG;
    LOG << 0;
    LOG << 1234567890123;
    LOG << (double)1.0f;
    LOG << (float)3.1415926;
    LOG << (short)1;
    LOG << (int)1;
    LOG << (long long)1;
    LOG << (unsigned int)1;
    LOG << (unsigned long)1;
    LOG << (long double)1.6555556;
    LOG << (unsigned long long)1;
    LOG << 'c';
    LOG << "abcdefg";
    LOG << std::string("This is a string");
    LOG << p;

    printf("%s\n", LOG.buffer().data());
    return 0;
}
