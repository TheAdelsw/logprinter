#pragma once

//服务端传输原始数据 客户端接收并且输出
//提供图片数据传输

//标准库
#include <iostream>
#include <vector>
#include <cstdint>
#include <chrono>

//套接字有关库
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <sstream>

//opencv库
#include <opencv2/opencv.hpp>


//发送协议 暂定
//[数据类型][时间戳][日志级别][分类长度][分类名][数据长度][原始数据]
//    1B      8B       1B        1B      ..      4B       ...
//服务端发送原始微秒级时间戳
//日志级别 1 2 3 : error(red) warning(yellow) info(white)

//客户端解算


//消息暂定两大类 文本消息全部使用std::string字符串 图片消息使用cv::Mat

enum class LogType : uint8_t
{
    TEXT_UTF8 = 0x01,
    IMAGE_RAW = 0x02,
};

enum class LogLevel : uint8_t
{
    ERROR = 0x01,
    WARNING = 0x02,
    INFO = 0x03,
};

struct LogStreamEntry
{
    std::vector<std::string> buf_text;
    std::vector<cv::Mat> buf_img;
};

class LogPrinter
{
public:

    LogPrinter(uint16_t port);
    ~LogPrinter();
    bool StopServer();
    //bool StartServer(uint16_t port);

    //文本消息
    LogPrinter& operator<<(const std::string& str);
    LogPrinter& operator<<(const int& val);
    LogPrinter& operator<<(const float& val);
    LogPrinter& operator<<(const double& val);
    
    //图片消息 发送原始图片数据
    LogPrinter& operator<<(const cv::Mat& mat);

    //建议只在单线程使用手动设置标签的方法 多线程会出错
    void SetMsgType(const std::string& str);

    
    
//utils
private:
    uint64_t TimeCounter();
    uint64_t htonl_64(uint64_t val);


private:

    int listen_fd;
    int client_fd;
    bool fd_released = false;
    bool send_running = true;//接收端结束不重复连接 避免中途阻塞
    //recv和send在异常时返回-1 若忽视异常继续使用 send会杀死程序


    std::string current_category = "Default";
    LogLevel current_level = LogLevel::INFO;


    bool send_Text(const LogLevel& level, const std::string& tag, const std::string& str);
    bool send_Img(const LogLevel& level, const std::string& tag, const cv::Mat& mat);
    

private:
    //解决多线程标签发送问题的结构体
    struct TaggedStream
    {
        LogPrinter& log;
        LogLevel level;
        std::string tag;
        LogStreamEntry msg;
        
        //析构函数临时对象(无名 由函数返回的值)在完整表达式结束后析构;
        ~TaggedStream();


        TaggedStream& operator<<(const std::string& str);
        TaggedStream& operator<<(const int& val);
        TaggedStream& operator<<(const float& val);
        TaggedStream& operator<<(const double& val);
        TaggedStream& operator<<(const cv::Mat& mat);

    };


public:
    //此operator是LogPrinter的
    TaggedStream operator()(const std::string& tag);
    TaggedStream operator()(const std::string& tag, const LogLevel& level);


};