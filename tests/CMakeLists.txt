add_executable(ProcessInfo_test ProcessInfo_test.cc)
#target_link_libraries(ProcessInfo_test JLog)
target_link_libraries(ProcessInfo_test JLog Ws2_32)

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