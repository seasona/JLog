AUX_SOURCE_DIRECTORY(. SRC_LIST)

add_library(JLog STATIC ${SRC_LIST})

install(TARGETS JLog DESTINATION lib)
install(FILES "${PROJECT_SOURCE_DIR}/include/Logging.h" DESTINATION include)