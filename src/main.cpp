#include<iostream>
#include"LogPrinter/core/LogPrinter.h"



int main()
{
    LogPrinter log(8888);
    log<<"草泥马\n";
    int x = 88;
    while(true)
    {
        log<<x<<" 这个是整型"<<123<<" 这个是浮点型"<<1.235<<" 这个是高精度浮点型"<<23.55555486888777<<"\n";
    }

    log.StopServer();
}