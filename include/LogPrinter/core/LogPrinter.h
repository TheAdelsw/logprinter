#pragma once

//服务端传输原始数据 客户端接收并且输出
//提供图片数据传输


#include<iostream>
#include<cstdint>




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

    LogPrinter& operator<<(const std::string& str);
    //LogPrinter& operator<<(const );



private:

    uint16_t port;


};