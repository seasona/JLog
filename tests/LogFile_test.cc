#include "../LogFile.h"
#include <string.h>
#include <unistd.h>

JLog::LogFile lf("jj", 1000, 3, 1024, false);
const char* logline = "the is a error in line 1243\n";
int len=strlen(logline);

int main(){
    for(int i=0;i<30;i++){
        lf.append(logline,len);
    }
    // 测试时不能一次性写入，这样因为系统时间没有区别，
    // 所以会写入同一文件
    sleep(2);
    for(int i=0;i<30;i++){
        lf.append(logline,len);
    }
    return 0;
}