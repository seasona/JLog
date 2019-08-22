#pragma once

#ifdef _MSC_VER
    #include <windows.h>
    #define SLEEP(x) delay(x)

    void delay(int ms){
        for(int i=0;i<50000;i++){

        }
    }
#else
    #include <unistd.h>
    #define SLEEP(x) usleep(x)
#endif