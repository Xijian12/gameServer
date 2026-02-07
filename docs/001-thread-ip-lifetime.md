# 问题编号：001
## 问题标题
为什么把栈上的 char ip[] 传给 detach 线程会导致悬空指针？

---

## 回答
因为 char ip[] 是一个栈帧变量，每次循环就会失效，而传给线程后，线程内部仍然在使用这个野指针
不要把“所有权不明确的裸指针”传给线程

---

## 出现问题的代码版本
- Commit: `41f1bac`
- 文件: `tcp_server.cpp`
- 函数: `TcpServer::start`

---

## 当时的代码（简化）
```cpp
char *ip;
std::thread t(&TcpServer::handle_client, this, client_fd, ip);
t.detach();
```

## 修改后
```cpp
std::string ip_str = ip;
void TcpServer::handle_client(int client_fd, const std::string &ip)
```
