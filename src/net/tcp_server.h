#pragma once
#include <string>

class TcpServer
{
private:
    int m_listen_fd;
    int m_port;
    int m_epfd;

public:
    TcpServer(int port);
    ~TcpServer();

    void start(); // 服务器启动入口
    static void set_nonblocking(int fd);                      // 设置 socket 为非阻塞
    void handle_client(int client_fd, const std::string &ip); // 多线程控制客户端
};