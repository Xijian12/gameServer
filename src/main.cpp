#include "net/tcp_server.h"

int main()
{
    TcpServer server(7777);
    server.start();
    return 0;
}
