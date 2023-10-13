//
// Created by russ on 23-9-21.
//

#include "Thread.h"


// 发送TFTP读请求 (RRQ) 数据包
ssize_t ThreadWork::sendRRQ(int sockfd, struct sockaddr_in server_ip, const std::string &_filename,
                            const std::string &_mode) {
    // 从完整路径中提取文件名
    std::string newName = _filename.substr(_filename.find_last_of('/') + 1);

    // 计算数据包的长度，包括操作码和文件名、传输模式以及各种分隔符的长度
    int len = 2 + newName.length() + 1 + _mode.length() + 1;

    // 格式化数据包，将操作码、文件名、传输模式以及分隔符组合到一个字符串中
    sprintf(buffer, "%c%c%s%c%s%c", 0, 1, newName.c_str(), 0, _mode.c_str(), 0);

    // 使用UDP套接字将数据包发送到服务器
    return sendto(sockfd, buffer, len, 0, (struct sockaddr*)&server_ip, sizeof(server_ip));
}

// 发送TFTP ACK 数据包
ssize_t ThreadWork::sendACK(int sockfd, struct sockaddr_in server_ip, unsigned short n) {
    // 格式化ACK数据包，包括操作码和数据块编号
    sprintf(buffer, "%c%c%c%c", 0, 4, n >> 8, n & 0xff);

    // 使用UDP套接字将ACK数据包发送到服务器
    return sendto(sockfd, buffer, 4, 0, (struct sockaddr*)&server_ip, sizeof(server_ip));
}

// 发送TFTP写请求 (WRQ) 数据包
ssize_t ThreadWork::sendWRQ(int sockfd, struct sockaddr_in server_ip, const std::string& _filename, const std::string& _mode) {
    // 从完整路径中提取文件名
    std::string newName = _filename.substr(_filename.find_last_of('/') + 1);

    // 计算数据包的长度，包括操作码和文件名、传输模式以及各种分隔符的长度
    int len = 2 + newName.length() + 1 + _mode.length() + 1;

    // 格式化数据包，将操作码、文件名、传输模式以及分隔符组合到一个字符串中
    sprintf(packet, "%c%c%s%c%s%c", 0, 2, newName.c_str(), 0, _mode.c_str(), 0);

    // 使用UDP套接字将数据包发送到服务器
    return sendto(sockfd, packet, len, 0, (struct sockaddr*)&server_ip, sizeof(server_ip));
}

// 发送TFTP数据 (DATA) 数据包
ssize_t ThreadWork::sendDATA(int sockfd, struct sockaddr_in server_ip, const char* data, unsigned short n, int size) {
    // 格式化数据包，包括操作码、数据块编号以及数据内容
    sprintf(packet, "%c%c%c%c%s", 0, 3, n >> 8, n & 0xff, data);

    // 使用UDP套接字将数据包发送到服务器
    return sendto(sockfd, packet, size + 4, 0, (struct sockaddr*)&server_ip, sizeof(server_ip));
}
