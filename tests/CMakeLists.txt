add_executable(FileUtil_test FileUtil_test.cc)
target_link_libraries(FileUtil_test JLog)

add_executable(LogFile_test LogFile_test.cc)
target_link_libraries(LogFile_test JLog)

add_executable(LogStream_test LogStream_test.cc)
target_link_libraries(LogStream_test JLog)

add_executable(AsyncLogging_test AsyncLogging_test.cc)
target_link_libraries(AsyncLogging_test JLog)

add_executable(Logging_test Logging_test.cc)
target_link_libraries(Logging_test JLog)

if(WIN32)
    target_link_libraries(ProcessInfo_test Ws2_32)
else(UNIX)
    target_link_libraries(AsyncLogging_test pthread)
    target_link_libraries(Logging_test pthread)
endif()
    