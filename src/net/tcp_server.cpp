#include "tcp_server.h"
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <thread>
#include <sys/epoll.h>
#include <fcntl.h>
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
    if (listen(m_listen_fd, 1024) < 0)
    {
        perror("listen");
        exit(1);
    }

    // 将监听socket加入epoll
    m_epfd = epoll_create1(EPOLL_CLOEXEC);
    epoll_event ev{};
    ev.events = EPOLLIN;
    ev.data.fd = m_listen_fd;

    set_nonblocking(m_listen_fd);
    epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_listen_fd, &ev);

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
    epoll_event events[1024];
    while (true)
    {
        int n = epoll_wait(m_epfd, events, 1024, -1);

        for (int i = 0; i < n; i++)
        {
            int fd = events[i].data.fd;

            // 1 如果是监听请求的话，说明有新客户端尝试连接
            // 有新客户端尝试连接时，操作系统会让m_listen_fd变为“可读”状态，也就是m_listen_fd出现在epoll队列中，
            if (fd == m_listen_fd)
            {
                int client_fd = accept(m_listen_fd, nullptr, nullptr);
                if (client_fd < 0)
                {
                    if (errno == EAGAIN || errno == EWOULDBLOCK)
                    {
                        // 没有更多连接了（正常情况）
                        continue;
                    }
                    else
                    {
                        perror("accept");
                        continue;
                    }
                }
                set_nonblocking(client_fd);

                epoll_event ev{};
                ev.events = EPOLLIN;
                ev.data.fd = client_fd;
                epoll_ctl(m_epfd, EPOLL_CTL_ADD, client_fd, &ev);
            }
            else
            {
                // 2 接收客户端数据
                char buf[1024];

                ssize_t len = recv(fd, buf, sizeof(buf), 0);
                if (len < 0)
                {
                    if (len == EAGAIN || len == EWOULDBLOCK)
                    {
                        // 没有更多连接了（正常情况）
                        continue;
                    }
                    else
                    {
                        perror("receive");
                        continue;
                    }
                }
                else if (len == 0)
                {
                    std::cout << "Client closed connection " << std::endl;
                    close(fd);
                    epoll_ctl(m_epfd, EPOLL_CTL_DEL, fd, nullptr);
                }
                else
                {
                    send(fd, buf, sizeof(buf), 0);
                    std::cout << "Send to:" << fd << " Content: " << buf << std::endl;
                }
            }
        }
    }
}

void TcpServer::set_nonblocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}