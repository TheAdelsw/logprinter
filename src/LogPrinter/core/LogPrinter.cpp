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

    //设置TCP端口复用
    int opt = 1;
    setsockopt(this->listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    //绑定
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

LogPrinter::~LogPrinter()
{
    close(this->listen_fd);
    close(this->client_fd);
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
    //时间戳
    uint64_t timestamp = LogPrinter::htonl_64(LogPrinter::TimeCounter());
    //转换为1字节读入buf
    uint8_t* p_timestamp = reinterpret_cast<uint8_t*>(&timestamp);
    //分类长度
    uint8_t category_len = this->current_category.size();
    //数据长度
    uint32_t data_len = htonl(str.size());//htonl将字节序转为网络字节序 对于不同架构的统一
    //转换读入给buf
    uint8_t* p_data_len = reinterpret_cast<uint8_t*>(&data_len);

    //字节序只针对多字节的数字 而非字节序列 
    //多字节数字 字节连续  字节序列 字节独立紧凑

    //坑之一 str.size()不可替换为data_len data_len此时是htonl修饰过的
    //而send只需在本地端告知整数长度即可
    // send(this->client_fd, str.c_str(), str.size(), 0);


    //防止多线程资源竟态问题 所有数据拼接成一个buffer
    std::vector<uint8_t> buf;
    buf.push_back(type);
    buf.insert(buf.end(), p_timestamp, p_timestamp + 8);
    buf.push_back(category_len);
    buf.insert(buf.end(), this->current_category.begin(), this->current_category.end());
    buf.insert(buf.end(), p_data_len, p_data_len + 4);//左闭右开
    buf.insert(buf.end(), str.begin(), str.end());
    
    send(this->client_fd, buf.data(), buf.size(), 0);

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

//图片发送的重载
LogPrinter& LogPrinter::operator<<(const cv::Mat& mat)
{
    //图片消息仍然按照元数据协议
    //其中data数据 按照[rows:4B][cols:4B][cvType:4B][raw pixelx:N]格式发送
    
    uint8_t type = static_cast<uint8_t>(LogType::IMAGE_RAW);

    uint64_t timestamp = LogPrinter::htonl_64(LogPrinter::TimeCounter());
    uint8_t* p_timestamp = reinterpret_cast<uint8_t*>(&timestamp);

    uint8_t category_len = this->current_category.size();

    //data内容
    uint32_t rows = htonl(mat.rows);
    uint32_t cols = htonl(mat.cols);
    uint32_t cvtype = htonl(mat.type());
    //total返回像素数 elemSize返回每个像素占用的字节数
    size_t pixel_bytes = mat.total() * mat.elemSize();
    uint32_t data_len = htonl(12 + pixel_bytes);
    uint8_t* p_data_len = reinterpret_cast<uint8_t*>(&data_len);
    uint8_t* p_rows = reinterpret_cast<uint8_t*>(&rows);
    uint8_t* p_cols = reinterpret_cast<uint8_t*>(&cols);
    uint8_t* p_cvtype = reinterpret_cast<uint8_t*>(&cvtype);

    std::vector<uint8_t> buf;
    buf.push_back(type);
    buf.insert(buf.end(), p_timestamp, p_timestamp + 8);
    buf.push_back(category_len);
    buf.insert(buf.end(), this->current_category.begin(), this->current_category.end());
    buf.insert(buf.end(), p_data_len, p_data_len + 4);
    buf.insert(buf.end(), p_rows, p_rows + 4);
    buf.insert(buf.end(), p_cols, p_cols + 4);
    buf.insert(buf.end(), p_cvtype, p_cvtype + 4);
    buf.insert(buf.end(), mat.data, mat.data + pixel_bytes);
    
    send(this->client_fd, buf.data(), buf.size(), 0);

    return *this;
}


void LogPrinter::SetMsgType(const std::string& str)
{
    this->current_category = str;
}


//utils
uint64_t LogPrinter::TimeCounter()
{
    auto now = std::chrono::system_clock::now();
    uint64_t ns = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
    return ns;
}
uint64_t LogPrinter::htonl_64(uint64_t val)
{
    //网络传输是大端 不同架构机器可能是大端或者小端
    //符合人类读写习惯的为大端 例如值为1的四字节数字 
    //大端为00 00 00 01
    //小端为01 00 00 00

    static const uint32_t test = 1;
    if (*reinterpret_cast<const uint8_t*>(&test) == 1)
    {
        return __builtin_bswap64(val);
    }
    return val;
}