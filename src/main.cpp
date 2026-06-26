#include<iostream>
#include"LogPrinter/core/LogPrinter.h"



int main()
{
    LogPrinter log(8888);
    log<<"草泥马\n";
    int x = 88;
    /* while(true)
    {
        log<<x<<" 这个是整型"<<123<<" 这个是浮点型"<<1.235<<" 这个是高精度浮点型"<<23.55555486888777<<"\n";
    } */

    cv::Mat img = cv::imread("test/murasame.jpg");
    //log<<img;
    log.SetMsgType("哈哈哈");
    log<<"测试成功\n";
    log.SetMsgType("图类");
    log<<"图片发送完成\n";
    int i1 = 1;
    int i2 = 1;
    int i3 = 1;
    // while(1)
    // {
    //     //log<<"测试:"<<i<<"\n";
    //     log.SetMsgType("哈哈哈");
    //     log<<"此时为:"<<i1;
    //     log.SetMsgType("图类");
    //     log<<"此时为..."<<i2;
    //     log.SetMsgType("异类");
    //     log<<"Now:"<<i3<<'\n';
    //     i1+=1;
    //     i2+=2;
    //     i3+=3;
    // }
    
    log.StopServer();
}