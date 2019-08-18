VPATH=../
#vpath %.cc ../
CXXFLAGS= -std=c++11

TARGET=ProcessInfo_test.out FileUtil_test.out LogFile_test.out LogStream_test.out AsyncLogging_test.out Logging_test.out

all: ${TARGET}

# -lgtest -lgtest_main -pthread

FileUtil_test.out: FileUtil_test.cc FileUtil.cc
	g++ -std=c++11 $^ -o $@ -g 
ProcessInfo_test.out: ProcessInfo_test.cc ProcessInfo.cc
	g++ -std=c++11 $^ -o $@ -g 
LogFile_test.out: LogFile_test.cc LogFile.cc FileUtil.cc ProcessInfo.cc
	g++ -std=c++11 $^ -o $@ -g
LogStream_test.out: LogStream_test.cc LogStream.cc
	g++ -std=c++11 $^ -o $@ -g
AsyncLogging_test.out: AsyncLogging_test.cc LogFile.cc FileUtil.cc ProcessInfo.cc AsyncLogging.cc
	g++ -std=c++11 $^ -o $@ -g -pthread
Logging_test.out:Logging_test.cc Logging.cc LogFile.cc FileUtil.cc ProcessInfo.cc AsyncLogging.cc LogStream.cc
	g++ -std=c++11 $^ -o $@ -g -pthread

.PHONY: clean test

clean:
	rm -f *.out
	rm -f *.log

test:
	
