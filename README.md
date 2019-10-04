# JLog

一个日志库，实现了以下的基本功能：

- 兼容win和linux两个平台
- 支持对于设定时间和文件大小的日志滚动
- 输出信息详尽，包括日志级别、线程id、日期、时间、内容、文件、行号
- 分级别输出
- 支持同步和异步输出，异步输出分为双缓冲区多线程异步以及阻塞队列异步两种方式
- 写入速度达到100w/s

## 程序思路

参照了muduo的日志库和spdlog，日志库大抵分为以下部分:

- LogStream: 日志输出的格式处理，重载了<<方便输出
- FileUtil: 最底层的文件类，封装了Log文件的打开、写入并在类析构的时候关闭文件，底层使用了标准IO
- LogFile: 对FileUtil类进行了封装，可以完成对日志文件名的自动生成，以及对于日志写入数据大小以及每24小时自动更换日志文件
- AsyncLogging: 异步日志类，分为前后端，前端写入缓冲区，缓冲区满了之后写入后端，后端是一个线程，将缓冲区写入文件。使用双缓冲区提高写入速度
- bq_AsyncLogging: 使用了阻塞队列，写入速度比双缓冲区低一些，所以作为备用选项
- Logging: JLog的API，可以设立日志文件的滚动大小、日志文件的基本名称、日志级别的输出、选择同步还是异步输出、输出的日志文件等

## 使用

```cpp
JLog::AsyncLogging* g_asyncLog = NULL;
JLog::LogFile* g_logFile = NULL;

// 选择异步输出，以及输出的日志文件
void asyncOutput(const char* msg, int len) { g_asyncLog->append(msg, len); }
// 选择同步输出，以及输出的日志文件
void dummyOutput(const char* msg, int len) { g_logFile->append(msg, len); }

void benchAsync(bool longLog) {
    JLog::Logger::setOutput(asyncOutput);
    // do something...
}

int main(int argc, char* argv[]) {
    char name[256] = {0};
    strncpy(name, argv[0], sizeof name - 1);

    // 默认标准输出
    LOG_INFO << "this is the default output";

    JLog::LogFile log(::basename(name), kRollSize);
    g_logFile = &log;
    benchDummy(longLog);

    JLog::AsyncLogging alog(::basename(name), kRollSize);
    g_asyncLog = &alog;
    benchAsync(longLog);

    return 0;
}
```

