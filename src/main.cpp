#include"LogPrinter/core/LogPrinter.h"



int main()
{
    LogPrinter log(8888);
    //log<<"草泥马";
    int x = 88;
    /* while(true)
    {
        log<<x<<" 这个是整型"<<123<<" 这个是浮点型"<<1.235<<" 这个是高精度浮点型"<<23.55555486888777<<"\n";
    } */

    cv::Mat img = cv::imread("test/murasame.jpg");
    cv::Mat img1 = cv::imread("test/000001.jpg");
    cv::Mat img2 = cv::imread("test/000002.jpg");
    cv::Mat img3 = cv::imread("test/000003.jpg");
    cv::Mat img4 = cv::imread("test/000004.jpg");
    cv::Mat img5 = cv::imread("test/000005.jpg");
    cv::Mat img6 = cv::imread("test/000006.jpg");
    cv::Mat img7 = cv::imread("test/000007.jpg");
    cv::Mat img8 = cv::imread("test/000008.jpg");
    cv::Mat img9 = cv::imread("test/000009.jpg");
    
    // log.SetMsgType("哈哈哈");
    // log<<"测试成功";
    // log.SetMsgType("图类");
    // log<<"图片发送完成";
    log("哈哈哈", LogLevel::ERROR)<<"测试成功"<<x;

    // log("图类")<<"图片发送完成";
    log("default", LogLevel::WARNING)<<"all";
    
    // log<<img1;
    // log<<img2;

    log("图类",LogLevel::ERROR)<<img1;
    log("图类",LogLevel::ERROR)<<img2;
    log("图类",LogLevel::ERROR)<<img3;
    log("图类",LogLevel::ERROR)<<img4;
    log("图类",LogLevel::ERROR)<<img5;
    log("图类",LogLevel::ERROR)<<img6;
    log("图类",LogLevel::ERROR)<<img7;
    log("图类",LogLevel::ERROR)<<img8;
    log("图类",LogLevel::ERROR)<<img9;
    
    int i1 = 1;
    int i2 = 1;
    int i3 = 1;
    printf("运行到这里了\n");
    while(1)
    {
        //log<<"测试:"<<i<<"\n";
        //log.SetMsgType("哈哈哈");
        log("哈哈哈",LogLevel::INFO)<<"此时为:"<<i1;
        //log.SetMsgType("图类");
        log("图类",LogLevel::ERROR)<<"此时为..."<<i2;
        //log.SetMsgType("异类");
        log("异类",LogLevel::WARNING)<<"Now:"<<i3;
        //log<<img;
        i1+=1;
        i2+=2;
        i3+=3;
        sleep(1);
     }
    printf("主函数结束\n");
    log.StopServer();
}