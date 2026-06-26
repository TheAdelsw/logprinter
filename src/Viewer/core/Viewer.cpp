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
    close(this->server_fd);
}


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

bool Viewer::Hear()
{
    uint8_t type;
    uint8_t category_len;
    uint32_t data_len;

    if (recv_all(this->server_fd, &type, 1) <= 0 ) return false;

    if (recv_all(this->server_fd, &category_len, 1) <= 0) return false;
    std::string category(category_len, '\0');
    if (recv_all(this->server_fd, &category[0], category_len) <= 0) return false;

    if (recv_all(this->server_fd, &data_len, 4) <= 0) return false;
    data_len = ntohl(data_len);
    std::string data(data_len, '\0');
    if (recv_all(this->server_fd, &data[0], data_len) <= 0) return false;

    LogEntry msg = 
    {
        type,
        category,
        data
    };
    this->history.push_back(msg);

    if(this->categories.find(category) == this->categories.end())
    {
        this->categories.insert(category);
        this->all_categories.assign(this->categories.begin(),this->categories.end());
    }
    



    // if (type == 0x01 ) 
    // {  // TEXT_UTF8
    //     std::cout <<"类型["<<category<<"]:"<< data ;
    // }

    /* if (type == 0x02)//IMAGE_JPEG
        {
            cv::Mat raw(1, len, CV_8UC1, (void*)data.data());
            cv::Mat img = cv::imdecode(raw, cv::IMREAD_COLOR);

            if (!img.empty()) {
                cv::imshow("LogPrinter Viewer", img);
                //while(1)
                cv::waitKey(1000);   // 必须调用，否则窗口不会刷新
                
            }

        } */


    return true;
}


void Viewer::StarReceive()
{
    this->recv_thread = std::thread(&Viewer::ReceiveLoop, this);
    //关于多线程 std::thread(&有参普通函数,参数)或std::thread(&无参普通函数)
    //对于普通函数 会隐式转化为函数指针 故而可以省略&s
    //如果是类中的方法 一定要&同时传入这个对象 如 类A x 则 std::thread(&A::函数,&x)
    //如果std::thread(&A::函数,x)则会拷贝一份x传入
    
}

void Viewer::ReceiveLoop()
{
    while(true)
    {
        if (Hear() == false) break;
    }
}


//----测试函数待删
int Viewer::getsock()
{
    return this->server_fd;
}
std::set<std::string> Viewer::getset()
{
    return this->categories;
}
//---


void Viewer::ShowByCategory()
{
    std::string select = this->all_categories[this->index_category]; 
    for(auto msg : this->history)
    {
        if( msg.type == 0x01)
        {
            if(select == "" || msg.category == select)
            {
                printf("[%s]%s\n",msg.category.c_str(),msg.data.c_str());
            }
        }
    }
}

void Viewer::IndexShift_L()
{
    int v = this->all_categories.size();
    this->index_category = (this->index_category + v -1) % v;
}

void Viewer::IndexShift_R()
{
    int v = this->all_categories.size();
    this->index_category = (this->index_category + v +1) % v;
}

void Viewer::SetFilter(const std::string& str)
{
    this->filter = str;
}