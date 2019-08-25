add_executable(gbench gbench.cc)
target_link_libraries(gbench JLog benchmark pthread)

add_executable(mybench mybench.cc)
target_link_libraries(mybench JLog pthread)