#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cstdint>
#include <iostream>

// 辅助函数：循环读取直到读满 n 字节
ssize_t recv_all(int fd, void* buf, size_t n)
{
    size_t total = 0;
    while (total < n) {
        ssize_t r = recv(fd, (char*)buf + total, n - total, 0);
        if (r <= 0) return r;  // 出错或对端关闭
        total += r;
    }
    return total;
}

int main()
{
    // 1. 创建套接字
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    // 2. 连接服务端
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
    addr.sin_port = htons(8888);  // 和服务端端口一致

    connect(sock, (struct sockaddr*)&addr, sizeof(addr));

    // 3. 循环接收并解析
    while (true) {
        uint8_t type;
        uint32_t len;

        if (recv_all(sock, &type, 1) <= 0) break;          // 读类型
        if (recv_all(sock, &len, 4) <= 0) break;           // 读长度
        len = ntohl(len);                                   // 转回主机字节序

        std::string data(len, '\0');
        if (recv_all(sock, &data[0], len) <= 0) break;     // 读数据

        // 4. 按类型处理
        if (type == 0x01) {  // TEXT_UTF8
            std::cout << data ;
        }
        // TODO: 处理图片类型（保存为文件或显示）
    }

    close(sock);
    return 0;
}
