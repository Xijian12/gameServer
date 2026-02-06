#pragma once

class TcpServer
{
private:
    int m_listen_fd;
    int m_port;

public:
    TcpServer(int port);
    ~TcpServer();

    void start(); // 服务器启动入口
};