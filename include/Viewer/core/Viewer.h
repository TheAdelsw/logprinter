#pragma once

//标准库
#include <cstring>
#include <cstdint>
#include <iostream>
#include <ctime>
#include <termios.h>

//套接字有关库
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>


//opencv库
#include <opencv2/opencv.hpp>

//多线程库
#include <mutex>
#include <thread>

/* 
协议
enum class LogType : uint8_t
{
    TEXT_UTF8 = 0x01,
    IMAGE_RAW = 0x02,
};
*/

//元消息类型 子消息类型暂定不使用继承
struct LogEntry
{
    uint8_t type;
    uint64_t timestamp;
    std::string category;
    std::string data;//文本或图片二进制数据 长度不定因此用string
    uint64_t index;
};

//文本消息
struct Entry_Text
{
  std::string text;  
};

//图片消息
struct Entry_Img
{
    cv::Mat img;
};




class Viewer
{
public:

    Viewer(const std::string ServerIP, const uint16_t ServerPort);
    ~Viewer();
    bool StopConnect();

    void StartThread();
    bool Hear();

    void ShowByCategory();
    void Show_A_Log();
    void IndexShift_L();
    void IndexShift_R();
    void SetFilter(const std::string& str);


    std::set<std::string> getset();

//utils 
private:
    ssize_t recv_all(int fd, void* buf, size_t n);//套接字 存储地址 字节数
    uint64_t ntohl_64(uint64_t val);
    std::string timestamp_decoder(uint64_t timestamp);
    cv::Mat imgdata_decoder(std::string imgdata);
    uint64_t TimeCounter_ms();
    void system_clear();

private:

    int server_fd;

    std::set<std::string> categories;//当前筛选分类 ""=全部
    std::vector<std::string> all_categories;
    uint16_t index_category = 0;

    std::vector<LogEntry> history;//所有元数据信息 扩容时旧迭代器失效
    size_t index_history = 0;
    
    std::vector<std::string> history_text;//所有文本数据
    std::vector<cv::Mat> history_img;//所有图片元数据



    std::string filter = "";     //当前筛选分类 ""=全部


    
    //监听消息的线程
    std::mutex recv_mtx;
    std::thread recv_thread;
    void ReceiveLoop();
    
    //监听键盘输入线程  //监听线程记录经过时间 并且调用播放日志函数 同时处理分类
    std::thread input_thread;
    bool input_running = true;
    bool show_pause = false;
    void InputLoop();
    
    //消息播放线程
    std::thread display_thread;
    bool display_running = true;
    double display_speed = 1.0;
    uint64_t display_waittime_cnt = 0;
    uint64_t display_interval = 200;//200ms
    void DisplayLoop();
    void Display_Reset();
    void IntervalUP();
    void IntervalDOWN();
    
    

};