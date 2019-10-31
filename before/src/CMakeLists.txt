set(src
    AsyncLogging.cc 
    FileUtil.cc
    LogFile.cc
    Logging.cc
    LogStream.cc
)

add_library(JLog STATIC ${src})

#AUX_SOURCE_DIRECTORY(. SRC_LIST)
#add_library(JLog STATIC ${SRC_LIST})
