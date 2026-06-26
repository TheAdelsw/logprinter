#pragma once


#include <cstring>
#include <cstdint>
#include <iostream>

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
    IMAGE_JPEG = 0x02,
    IMAGE_PNG = 0x03,
    IMAGE_RAW = 0x04,
};
*/

struct LogEntry
{
    uint8_t type;
    std::string category;
    std::string data;//文本或图片二进制数据

};


class Viewer
{
public:

    Viewer(const std::string ServerIP, const uint16_t ServerPort);
    ~Viewer();

    bool Hear();
    void StarReceive();
    void ShowByCategory();
    void IndexShift_L();
    void IndexShift_R();
    void SetFilter(const std::string& str);


    int getsock();
    std::set<std::string> getset();

//utils 不使用成员变量
private:
    ssize_t recv_all(int fd, void* buf, size_t n);//套接字 存储地址 字节数


private:

    int server_fd;

    std::set<std::string> categories;
    std::vector<std::string> all_categories;
    uint16_t index_category = 0;

    std::vector<LogEntry> history;
    std::string filter = "";     //当前筛选分类 ""=全部

    std::mutex mtx;
    std::thread recv_thread;
    void ReceiveLoop();



};