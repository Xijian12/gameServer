# Game Server Learning Project

这是一个从 0 开始实现的 C++ TCP 服务器项目。

## 学习目标
- 掌握 Linux 下 C++ 网络编程
- 理解 socket / epoll / 多线程模型
- 构建可维护的服务器工程

## 学习方式
- 每个问题都会记录在 docs/
- 每个问题都能追溯到具体 commit
- 不记录“抄到的知识”，只记录“踩过的坑”

## 已解决问题
- [x] 为什么 bind 需要 htons
- [x] accept 为什么用 sockaddr*
- [x] 栈变量传给线程导致 UB
- [ ] epoll 与多线程的取舍
