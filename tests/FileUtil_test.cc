#include "../FileUtil.h"
#include <iostream>
#include <string.h>

const char* filename = "./test.log";
JLog::AppendFile f(filename);
const char* logline1 = "Test1 2019/2/4 0:32:22 test() 1129\n";
int len = strlen(logline1);

int main(){
    f.append(logline1,len);
    f.append(logline1,len);
    f.append(logline1,len);
    // 3*35=105
    std::cout<<f.writtenBytes()<<std::endl;
    return 0;
}
