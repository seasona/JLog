cmake_minimum_required(VERSION 3.0.0)
project(JLog VERSION 0.2.0)
set(CMAKE_CXX_STANDARD 11)

set(LIB_SRC
    AsyncLogging.cc
    FileUtil.cc
    LogFile.cc
    Logging.cc
    LogStream.cc
    ProcessInfo.cc
)

add_subdirectory(tests)

add_library(JLog ${LIB_SRC})

install(TARGETS JLog DESTINATION lib)
install(FILES Logging.h DESTINATION include)