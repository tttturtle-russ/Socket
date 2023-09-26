//
// Created by russ on 23-9-21.
//
#include "Thread.h"

ssize_t ThreadWork::download(const std::string& _filename, const std::string& _mode){
    int sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd < 0){
        l->error("创建socket失败: %s\n",strerror(errno));
        exit(1);
    }
    l->info("成功创建Socket句柄: %d\n",sockfd);
    auto name = _filename.substr(_filename.find_last_of('/') + 1);
    std::ofstream file(name, std::ios::out | std::ios::binary);
    if (!file.is_open() || !file.good()){
        l->error("打开文件失败: %s\n",strerror(errno));
        exit(1);
    }
    struct sockaddr_in server_ip{};
    server_ip.sin_port = htons(69);
    server_ip.sin_family = AF_INET;
    socklen_t len = sizeof(server_ip);
    inet_pton(AF_INET,"localhost",&server_ip.sin_addr);
    if (sendRRQ(sockfd, server_ip, _filename, _mode)<0){
        l->error("发送RRQ失败: %s\n",strerror(errno));
        return -1;
    }
    l->info("成功发送RRQ\n");
    unsigned short num = 0;
    std::streamsize size;
    unsigned short end = -1;
    bool flag = false;
    while (true) {
        char buf[512 + 4];
        ssize_t b = recvfrom(sockfd,buf,516,0,(struct sockaddr*)&server_ip,&len);
        if (b == -1){
            l->error("接收服务器数据发生错误: %s\n", strerror(errno));
            exit(1);
        }
        if (!flag) {
            flag = true;
            l->info("与服务器建立连接，服务器端口号: %d\n", ntohs(server_ip.sin_port));
        }
        switch (buf[1]) {
            case 3:
            {
                unsigned short n = ntohs(*(unsigned short*)(buf + 2));
                l->info("接收到DATA: %d\n",n);
                if (n == num + 1)
                {
                    num = n;
                    size = b - 4;
                    if (size < 512) {
                        end = n;
                    }
                    file.write(buf+4,size);
                    sendACK(sockfd,server_ip,n);
                    l->info("发送ACK: %d\n",n);
                }
                else
                {
                    sendACK(sockfd,server_ip,num);
                    l->info("发送ACK: %d\n",n);
                }
                if (end == n)
                {
                    l->info("关闭连接...\n");
                    l->info("成功下载文件: %s\n",_filename.c_str());
                    file.close();
                    close(sockfd);
                    return 0;
                }
                break;
            }
            case 5:
            {
                l->error("接收到ERROR: %s\n",buf+4);
                file.close();
                close(sockfd);
                return -1;
            }
            default:
                break;
        }

    }
}