//
// Created by russ on 23-9-21.
//
#include "Thread.h"

ssize_t ThreadWork::download(const std::string& _filename, const std::string& _mode) {
    // 创建一个 UDP 套接字。
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    // 检查套接字创建是否失败。
    if (sockfd < 0) {
        l->error("创建socket失败: %s\n", strerror(errno));
        exit(1);
    }

    // 记录成功创建套接字。
    l->info("成功创建Socket句柄: %d\n", sockfd);

    // 从完整路径中提取文件名。
    auto name = _filename.substr(_filename.find_last_of('/') + 1);

    // 创建一个用于写入二进制数据的输出文件流。
    std::ofstream file(name, std::ios::out | std::ios::binary);

    // 检查文件流是否已打开且处于良好状态。
    if (!file.is_open() || !file.good()) {
        l->error("打开文件失败: %s\n", strerror(errno));
        exit(1);
    }

    // 创建一个 sockaddr_in 结构，用于存储服务器信息。
    struct sockaddr_in server_ip{};
    server_ip.sin_port = htons(69);  // TFTP 服务器端口
    server_ip.sin_family = AF_INET;
    socklen_t len = sizeof(server_ip);
    inet_pton(AF_INET, "localhost", &server_ip.sin_addr);  // 设置服务器 IP 地址。

    // 向服务器发送 TFTP 读请求 (RRQ)。
    if (sendRRQ(sockfd, server_ip, _filename, _mode) < 0) {
        l->error("发送RRQ失败: %s\n", strerror(errno));
        return -1;
    }

    // 记录成功发送 RRQ。
    l->info("成功发送RRQ\n");

    unsigned short num = 0;
    std::streamsize size;
    unsigned short end = -1;
    bool flag = false;

    // 进入循环，接收和处理来自服务器的数据包。
    while (true) {
        char buf[512 + 4];
        ssize_t b = recvfrom(sockfd, buf, 516, 0, (struct sockaddr*) &server_ip, &len);

        // 检查接收数据时是否发生错误。
        if (b == -1) {
            l->error("接收服务器数据发生错误: %s\n", strerror(errno));
            exit(1);
        }

        // 如果之前未记录与服务器的连接，现在记录它。
        if (!flag) {
            flag = true;
            l->info("与服务器建立连接，服务器端口号: %d\n", ntohs(server_ip.sin_port));
        }

        // 根据数据包的操作码处理接收到的数据包。
        switch (buf[1]) {
            case 3:  // 数据包
            {
                unsigned short n = ntohs(*(unsigned short*)(buf + 2));
                l->info("接收到DATA: %d\n", n);

                // 检查接收到的数据块号是否符合预期。
                if (n == num + 1) {
                    num = n;
                    size = b - 4;

                    // 检查是否是最后一个数据包。
                    if (size < 512) {
                        end = n;
                    }

                    // 将数据写入输出文件并发送 ACK 到服务器。
                    file.write(buf + 4, size);
                    sendACK(sockfd, server_ip, n);
                    l->info("发送ACK: %d\n", n);
                } else {
                    // 如果接收到的数据块号与预期不符，重新发送上一个接收到的数据块的 ACK。
                    sendACK(sockfd, server_ip, num);
                    l->info("发送ACK: %d\n", n);
                }

                // 如果接收到最后一个数据包，关闭连接和文件。
                if (end == n) {
                    l->info("关闭连接...\n");
                    l->info("成功下载文件: %s\n", _filename.c_str());
                    file.close();
                    close(sockfd);
                    return 0;
                }
                break;
            }
            case 5:  // 错误包
            {
                l->error("接收到ERROR: %s\n", buf + 4);

                // 关闭文件和连接。
                file.close();
                close(sockfd);
                return -1;
            }
            default:
                break;
        }
    }
}
