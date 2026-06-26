#include"LogPrinter/core/LogPrinter.h"



LogPrinter::LogPrinter(uint16_t port)
{
    //启动服务端需要专用套接字
    this->listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    //套接字地址
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;//IPV4
    addr.sin_addr.s_addr = INADDR_ANY;//监听所有网卡
    addr.sin_port = htons(port);//大小端转换 防止字节序问题
    //绑定 启动服务端

    //暂未设置TCP端口复用
    if(bind(this->listen_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
    {
        printf("服务端启动失败");
        return;
    }
    
    printf("服务端启动成功\n");
    printf("监听中...\n");
    listen(this->listen_fd, 5);//队列长度为5

    //接收连接
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    this->client_fd = accept(this->listen_fd, (struct sockaddr*)&client_addr, &client_len);
    if (this->client_fd == -1)
    {
        printf("接受连接失败\n");
        return;
    }    

    printf("连接成功\n");

}
bool LogPrinter::StopServer()
{
    close(this->listen_fd);
    close(this->client_fd);
    return true;
}



LogPrinter& LogPrinter::operator<<(const std::string& str)
{
    //标记
    uint8_t type = static_cast<uint8_t>(LogType::TEXT_UTF8);
    //分类长度
    uint8_t category_len = this->current_category.size();
    //数据长度
    uint32_t data_len = htonl(str.size());//htonl将字节序转为网络字节序 对于不同架构的统一
    //字节序只针对多字节的数字 而非字节序列 
    //多字节数字 字节连续  字节序列 字节独立紧凑

    send(this->client_fd, &type, 1, 0);
    send(this->client_fd, &category_len, 1, 0);
    send(this->client_fd, this->current_category.c_str(), category_len, 0);
    send(this->client_fd, &data_len, 4, 0);
    //坑之一 str.size()不可替换为data_len data_len此时是htonl修饰过的
    //而send只需在本地端告知整数长度即可
    send(this->client_fd, str.c_str(), str.size(), 0);

    
    return *this;//链式调用 返回自身实例
}

LogPrinter& LogPrinter::operator<<(const int& val)
{
    std::ostringstream oss;
    oss << val;
    *this << oss.str();
    return *this;
}

LogPrinter& LogPrinter::operator<<(const float& val)
{
    std::ostringstream oss;
    oss << val;
    *this << oss.str();
    return *this;
}
LogPrinter& LogPrinter::operator<<(const double& val)
{
    std::ostringstream oss;
    oss << val;
    *this << oss.str();
    return *this;
}


LogPrinter& LogPrinter::operator<<(const cv::Mat& mat)
{
    std::vector<uchar> buf;
    cv::imencode(".jpg", mat, buf);

    uint8_t type = static_cast<uint8_t>(LogType::IMAGE_JPEG);
    uint32_t len = htonl(buf.size());

    send(this->client_fd, &type, 1, 0);
    send(this->client_fd, &len, 4, 0);
    send(this->client_fd, buf.data(), buf.size(), 0);

    return *this;
}


void LogPrinter::SetMsgType(const std::string& str)
{
    this->current_category = str;
}