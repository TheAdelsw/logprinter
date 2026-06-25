#pragma once

//服务端传输原始数据 客户端接收并且输出
//提供图片数据传输

#include <iostream>
#include <cstdint>

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
//[数据类型][数据长度][原始数据]



enum class LogType : uint8_t
{
    TEXT_UTF8 = 0x01,
    IMAGE_JPEG = 0x02,
    IMAGE_PNG = 0x03,
    IMAGE_RAW = 0x04,
};


class LogPrinter
{
public:

    LogPrinter(uint16_t port);
    
    //bool StartServer(uint16_t port);
    bool StopServer();

    //文本消息
    LogPrinter& operator<<(const std::string& str);
    LogPrinter& operator<<(const int& val);
    LogPrinter& operator<<(const float& val);
    LogPrinter& operator<<(const double& val);
    
    //图片消息
    LogPrinter& operator<<(const cv::Mat& mat);


private:

    int listen_fd;
    int client_fd;
    uint16_t port;


};