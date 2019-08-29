add_executable(gbench gbench.cc)
target_link_libraries(gbench JLog benchmark pthread)
