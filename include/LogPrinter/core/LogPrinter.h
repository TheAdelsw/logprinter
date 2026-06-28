#pragma once

//服务端传输原始数据 客户端接收并且输出
//提供图片数据传输

//标准库
#include <iostream>
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
//[数据类型][时间戳][分类长度][分类名][数据长度][原始数据]
//    1B      8B       1B      ..      4B       ...
//服务端发送原始微秒级时间戳
//客户端解算


//消息暂定两大类 文本消息全部使用std::string字符串 图片消息使用cv::Mat

enum class LogType : uint8_t
{
    TEXT_UTF8 = 0x01,
    IMAGE_RAW = 0x02,
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


};