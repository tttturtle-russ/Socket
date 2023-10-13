//
// Created by russ on 23-9-21.
//
#include "Thread.h"

// 上传文件到TFTP服务器
ssize_t ThreadWork::upload(const std::string& _filename, const std::string& _mode) {
    // 创建一个 UDP 套接字
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    // 检查套接字创建是否失败
    if (sockfd < 0) {
        l->error("创建socket失败: %s\n", strerror(errno));
        exit(1);
    }

    // 记录成功创建套接字
    l->info("成功创建Socket句柄: %d\n", sockfd);

    // 打开要上传的文件
    std::ifstream file(_filename, std::ios::in | std::ios::binary);

    // 检查文件是否成功打开
    if (!file.is_open() || !file.good()) {
        l->error("打开文件失败: %s\n", strerror(errno));
        exit(1);
    }

    // 记录成功打开的文件
    l->info("成功打开文件: %s\n", _filename.c_str());

    // 创建服务器 IP 地址结构
    struct sockaddr_in server_ip{};
    server_ip.sin_port = htons(69);  // TFTP 服务器端口
    server_ip.sin_family = AF_INET;
    inet_pton(AF_INET, "localhost", &server_ip.sin_addr);  // 设置服务器 IP 地址。

    // 向服务器发送 TFTP 写请求 (WRQ)
    if (sendWRQ(sockfd, server_ip, _filename, _mode) < 0) {
        l->error("发送WRQ失败: %s\n", strerror(errno));
        return -1;
    }

    // 记录成功发送 WRQ
    l->info("成功发送WRQ\n");

    unsigned short num = -1;
    std::streamsize size;
    unsigned short end = -1;
    bool flag = false;

    // 进入循环，准备接收来自服务器的ACK和发送数据
    while (true) {
        char buf[512];
        char data[512];
        socklen_t len = sizeof(server_ip);

        // 接收来自服务器的数据包
        ssize_t b = recvfrom(sockfd, buf, 512, 0, (struct sockaddr*)&server_ip, &len);

        // 如果之前未记录与服务器的连接，现在记录它
        if (!flag) {
            l->info("与服务器建立连接，服务器端口号: %d\n", ntohs(server_ip.sin_port));
            flag = true;
        }

        // 检查是否接收到数据包时发生错误
        if (b == -1) {
            l->error("接收服务器数据发生错误: %s\n", strerror(errno));
            exit(1);
        }

        // 根据数据包的操作码处理接收到的数据包
        switch (buf[1]) {
            case 4:  // ACK 数据包
            {
                unsigned short ack = ntohs(*(unsigned short*)(buf + 2));
                l->info("接受到ACK: %d\n", ack);

                // 如果已经接收到了所有数据块，关闭文件和连接
                if (end == ack) {
                    file.close();
                    close(sockfd);
                    l->info("关闭连接...\n");
                    l->info("成功上传文件: %s\n", _filename.c_str());
                    return 0;
                }

                // 如果ACK不等于上一个数据块的编号，读取下一个数据块并发送
                if (ack != num) {
                    file.read(data, 512);

                    // 如果已经读取到文件末尾，设置结束标志
                    if (file.eof())
                        end = ack + 1;

                    size = file.gcount();
                    num = ack;
                    data[size] = '\0';

                    // 发送数据块
                    ssize_t result = sendDATA(sockfd, server_ip, data, ack + 1, (int)size);

                    // 检查是否成功发送了数据块
                    if (result != size + 4) {
                        l->error("发送DATA失败: %s\n", strerror(errno));
                        exit(1);
                    }

                    // 记录成功发送的数据块信息
                    l->info("成功发送DATA,块编号: %d,长度: %d\n", ack + 1, size);
                    continue;
                }

                // 如果ACK等于上一个数据块的编号，重发上一个数据块
                sendDATA(sockfd, server_ip, data, ack + 1, (int)size);
                l->info("成功发送DATA,块编号: %d,长度: %d\n", ack + 1, size);

                // 如果已经读取到文件末尾，设置结束标志
                if (file.eof())
                    end = ack + 1;
                break;
            }
            case 5:  // 错误数据包
            {
                l->error("发生错误:%s\n", buf + 4);
                return -1;
            }
            default:
                break;
        }
    }
}
