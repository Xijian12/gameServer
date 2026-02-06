#include "tcp_server.h"
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <thread>
#include <iostream>

TcpServer::TcpServer(int port) : m_listen_fd(-1),
                                 m_port(port)
{
    // 1 创建socket
    m_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_listen_fd < 0)
    {
        perror("create");
        exit(1);
    }

    // 2 绑定地址
    sockaddr_in addr = {};
    addr.sin_family = AF_INET;         // 指定协议为 ipv4
    addr.sin_addr.s_addr = INADDR_ANY; // 绑定所有绑定本机所有网卡
    addr.sin_port = htons(m_port);     // 主机字节序 转成 网络字节序(统一转成网络通信用的大端字节序)

    // 将sockaddr_in类型的向上转化为sockaddr类
    if (bind(m_listen_fd, (sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        exit(1);
    }

    // 3 启动监听
    if (listen(m_listen_fd, 5) < 0)
    {
        perror("listen");
        exit(1);
    }

    std::cout << "Start Listen on port :" << m_port << std::endl;
}
TcpServer::~TcpServer()
{
    if (m_listen_fd >= 0)
    {
        close(m_listen_fd);
    }
}

void TcpServer::start()
{
    while (true)
    {
        sockaddr_in client_addr = {};
        socklen_t client_len = sizeof(client_addr);

        // 4 等待客户端连接
        int client_fd = accept(m_listen_fd, (sockaddr *)&client_addr, &client_len);
        if (client_fd < 0)
        {
            perror("accept");
            exit(1);
        }

        char ip[INET_ADDRSTRLEN]; // 把二进制ip转为网络地址模式
        inet_ntop(AF_INET, &client_addr.sin_addr, ip, sizeof(ip));
        std::cout << "Client connected: " << ip << std::endl;

        // 5 为每一个客户端创建线程
        std::thread t(&TcpServer::handle_client, this, client_fd, ip);

        // 6 分离线程
        t.detach();
    }
}

void TcpServer::handle_client(int client_fd, char *ip)
{
    // 1 接收客户端信息并回显
    char buffer[1024];
    while (true)
    {
        ssize_t n = recv(client_fd, buffer, sizeof(buffer), 0);
        if (n <= 0)
        {
            std::cout << "Client disconnected: " << ip << std::endl;
            break;
        }
        send(client_fd, buffer, n, 0);
        std::cout << "Send to:" << client_fd << " Content: " << buffer << std::endl;
    }
    close(client_fd);
}
