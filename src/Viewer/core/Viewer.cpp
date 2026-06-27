#include "Viewer/core/Viewer.h"
// 辅助函数：循环读取直到读满 n 字节

Viewer::Viewer(const std::string ServerIP, const uint16_t ServerPort)
{
    this->server_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, ServerIP.c_str(), &addr.sin_addr);
    addr.sin_port = htons(ServerPort);

    if(connect(this->server_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
    {
        printf("连接失败,请检查IP和端口\n");
        return;
    }
    printf("连接成功\n");


    //set容器初始化
    this->categories.insert("");

}



Viewer::~Viewer()
{
    //主函数结束前阻塞
    this->recv_thread.join();
    this->input_thread.join();
    this->display_thread.join();
    // this->display_running = false;
    // this->display_thread.join();
    close(this->server_fd);
}


bool Viewer::StopConnect()
{
    close(this->server_fd);
    return true;
}



bool Viewer::Hear()
{
    //协议:[数据类型][时间戳][分类长度][分类名][数据长度][原始数据]
    //按元消息格式解算
    uint8_t type;
    uint64_t timestamp;
    uint8_t category_len;
    uint32_t data_len;

    if (recv_all(this->server_fd, &type, 1) <= 0) return false;

    if (recv_all(this->server_fd, &timestamp, 8) <= 0) return false;
    timestamp = ntohl_64(timestamp);

    if (recv_all(this->server_fd, &category_len, 1) <= 0) return false;
    std::string category(category_len, '\0');
    if (recv_all(this->server_fd, &category[0], category_len) <= 0) return false;

    if (recv_all(this->server_fd, &data_len, 4) <= 0) return false;
    data_len = ntohl(data_len);
    std::string data(data_len, '\0');
    if (recv_all(this->server_fd, &data[0], data_len) <= 0) return false;

    uint64_t index;
    switch(type)
    {
        case 0x01:
            index = this->history_text.size();
            this->history_text.push_back({data});
            break;
        case 0x02:
            index = this->history_img.size();
            this->history_img.push_back(imgdata_decoder(data));
            break;
    }

    LogEntry msg = 
    {
        type,
        timestamp,
        category,
        data,
        index,
    };
    this->history.push_back(msg);

    //set集合更新 vector拷贝分类名
    if(this->categories.find(category) == this->categories.end())
    {
        this->categories.insert(category);
        this->all_categories.assign(this->categories.begin(),this->categories.end());
    }
    

    // if(type == 0x01)
    // {    
    //     if(msg.category == this->all_categories[this->index_category] || this->all_categories[this->index_category] == "")
    //     {
    //         printf("[%s][%s]%s\n",timestamp_decoder(msg.timestamp).c_str(),msg.category.c_str(),msg.data.c_str());
    //     }
    // }
    // if(type == 0x02)
    // {
    //     if(msg.category == this->all_categories[this->index_category] || this->all_categories[this->index_category] == "")
    //     {

    //         cv::imshow("LogPrinter", this->history_img[msg.index]);
    //         cv::waitKey(10);

    //     }
    // }



    return true;
}


void Viewer::StartThread()
{
    //关于多线程 std::thread(&有参普通函数,参数)或std::thread(&无参普通函数)
    //对于普通函数 会隐式转化为函数指针 故而可以省略&s
    //如果是类中的方法 一定要&同时传入这个对象 如 类A x 则 std::thread(&A::函数,&x)
    //如果std::thread(&A::函数,x)则会拷贝一份x传入
    this->recv_thread = std::thread(&Viewer::ReceiveLoop, this);
    this->input_thread = std::thread(&Viewer::InputLoop, this);
    this->display_thread = std::thread(&Viewer::DisplayLoop, this);
}

//线程函数-----
void Viewer::ReceiveLoop()
{
    while(true)
    {
        if (Hear() == false) break;
    }
}

void Viewer::InputLoop()
{
    struct termios old_tio;
    tcgetattr(STDIN_FILENO, &old_tio);
    struct termios new_tio = old_tio;
    new_tio.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);

    char cmd;
    while(this->input_running && read(STDIN_FILENO, &cmd, 1))
    {

        

        if(cmd == 'q')this->input_running = false;
        if(cmd == 'r')this->Display_Reset();
        if(cmd == 'a')this->IndexShift_L();
        if(cmd == 'd')this->IndexShift_R();
        if(cmd == '-')this->IntervalDOWN();
        if(cmd == '=')this->IntervalUP();
        // system("clear");
        // this->ShowByCategory();


    }
    tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);

}

void Viewer::DisplayLoop()
{
    system_clear();
    while(this->display_running)
    {
        static uint64_t first_time = this->TimeCounter_ms();

        uint64_t last_time = this->TimeCounter_ms();
        this->display_waittime_cnt += last_time - first_time;
        //printf("此时cnt %lu\n",this->display_waittime_cnt);
        first_time = last_time;

        if(this->display_waittime_cnt >= this->display_interval)
        {
            this->display_waittime_cnt = 0;

            std::string select = this->all_categories[this->index_category];
            size_t total = this->history.size();

            while(select != ""&& this->index_history < total && this->history[this->index_history].category != select)
            {
                this->index_history++;
            }

            //printf("触发输出\n");
            this->Show_A_Log();
            this->index_history++;

            while(select != ""&& this->index_history < total && this->history[this->index_history].category != select)
            {
                this->index_history++;
            }
        }

    }



}


//-------




void Viewer::ShowByCategory()
{
    std::string select = this->all_categories[this->index_category]; 
    for(auto msg : this->history)
    {
        if( msg.type == 0x01)
        {
            if(select == "" || msg.category == select)
            {
                printf("[%s][%s]%s\n",timestamp_decoder(msg.timestamp).c_str(),msg.category.c_str(),this->history_text[msg.index].c_str());
            }
        }
        if( msg.type == 0x02)
        {
            if(select == "" || msg.category == select)
            {

                cv::imshow("LogPrinter", this->history_img[msg.index]);
                cv::waitKey(10);

            }
        }

    }

}


void Viewer::Show_A_Log()
{
    if(this->index_history >= this->history.size())return;
    auto now_msg = this->history[this->index_history];
    switch(now_msg.type)
    {
        case 0x01:
        printf("[%s][%s]%s\n",timestamp_decoder(now_msg.timestamp).c_str(),now_msg.category.c_str(),this->history_text[now_msg.index].c_str());
        break;

        case 0x02:
        cv::imshow("LogPrinter", this->history_img[now_msg.index]);
        cv::waitKey(1);
        break;
    }
    
}




void Viewer::IndexShift_L()
{
    int v = this->all_categories.size();
    this->index_category = (this->index_category + v -1) % v;
    this->filter = this->all_categories[this->index_category];
    system_clear();
    this->index_history = 0;
}

void Viewer::IndexShift_R()
{
    int v = this->all_categories.size();
    this->index_category = (this->index_category + v +1) % v;
    system_clear();
    this->index_history = 0;
}

void Viewer::Display_Reset()
{
    system_clear();
    this->index_history = 0;
}

void Viewer::IntervalDOWN()
{
    if(this->display_interval <= 100)return;
    this->display_interval -= 100;
}
void Viewer::IntervalUP()
{
    this->display_interval += 100;
}

void Viewer::SetFilter(const std::string& str)
{
    this->filter = str;
}


//utils


ssize_t Viewer::recv_all(int fd, void* buf, size_t n)
{
    size_t total = 0;
    while (total < n)
    {
        ssize_t r = recv(fd, (char*)buf + total, n - total, 0);
        if(r <= 0) return r;
        total += r;
    }
    return total;
}

uint64_t Viewer::ntohl_64(uint64_t val)
{
    static const uint32_t test = 1;
    if (*reinterpret_cast<const uint8_t*>(&test) == 1)
    {
        return __builtin_bswap64(val);
    }
    return val;
}

std::string Viewer::timestamp_decoder(uint64_t timestamp)
{
    time_t sec = timestamp / 1'000'000;
    int usec = timestamp % 1'000'000;
    struct tm* t = localtime(&sec);
    char timebuf[32];
    strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", t);
    char result[64];
    //snprintf(目标缓冲区 缓冲区大小 格式串 值)
    //适用于混合格式拼接
    snprintf(result, sizeof(result), "%s.%06d", timebuf, usec);

    return std::string(result);

}

cv::Mat Viewer::imgdata_decoder(std::string imgdata)
{
    uint32_t rows;
    uint32_t cols;
    uint32_t cvtype;

    memcpy(&rows, &imgdata[0], 4); rows = ntohl(rows);
    memcpy(&cols, &imgdata[4], 4); cols = ntohl(cols);
    memcpy(&cvtype, &imgdata[8], 4); cvtype = ntohl(cvtype);

    //不拷贝数据 只存储了其指针 因此return需要返回一个clone深拷贝后的数据
    cv::Mat img(rows, cols, cvtype, (void*)&imgdata[12]);

    //printf("img: %dx%d type=%d empty=%d\n", img.rows, img.cols, img.type(), img.empty());

    return img.clone();
}

uint64_t Viewer::TimeCounter_ms()
{
    auto now = std::chrono::system_clock::now();
    uint64_t ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    return ms;
}

void Viewer::system_clear()
{
    system("clear");
}