//
// Created by russ on 23-9-21.
//
#include "Thread.h"

ssize_t ThreadWork::upload(const std::string& _filename, const std::string& _mode){
    int sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd < 0){
        l->error("创建socket失败: %s\n",strerror(errno));
        exit(1);
    }
    l->info("成功创建Socket句柄: %d\n",sockfd);
    std::ifstream file(_filename, std::ios::in | std::ios::binary);
    if (!file.is_open() || !file.good()){
        l->error("打开文件失败: %s\n",strerror(errno));
        exit(1);
    }
    l->info("成功打开文件: %s\n",_filename.c_str());
    struct sockaddr_in server_ip{};
    server_ip.sin_port = htons(69);
    server_ip.sin_family = AF_INET;
    inet_pton(AF_INET,"localhost",&server_ip.sin_addr);
    if (sendWRQ(sockfd, server_ip, _filename, _mode) < 0){
        l->error("发送WRQ失败: %s\n",strerror(errno));
        return -1;
    }
    l->info("成功发送WRQ\n");
    unsigned short num = -1;
    std::streamsize size;
    unsigned short end = -1;
    bool flag = false;
    while (true) {
        char buf[512];
        char data[512];
        socklen_t len = sizeof(server_ip);
        ssize_t b = recvfrom(sockfd,buf,512,0,(struct sockaddr*)&server_ip,&len);
        if (!flag){
            l->info("与服务器建立连接，服务器端口号: %d\n",ntohs(server_ip.sin_port));
            flag = true;
        }
        if (b == -1){
            l->error("接收服务器数据发生错误: %s\n", strerror(errno));
            exit(1);
        }
        switch (buf[1]) {
            case 4:
            {
                unsigned short ack = ntohs(*(unsigned short*)(buf + 2));
                l->info("接受到ACK: %d\n",ack);
                if (end == ack)
                {
                    file.close();
                    close(sockfd);
                    l->info("关闭连接...\n");
                    l->info("成功上传文件: %s\n",_filename.c_str());
                    return 0;
                }
                if (ack != num){
                    file.read(data,512);
                    if (file.eof())
                        end = ack + 1;
                    size = file.gcount();
                    num = ack;
                    data[size] = '\0';
                    ssize_t result = sendDATA(sockfd,server_ip,data,ack + 1,(int)size);
                    if (result != size + 4){
                        l->error("发送DATA失败: %s\n",strerror(errno));
                        exit(1);
                    }
                    l->info("成功发送DATA,块编号: %d,长度: %d\n",ack + 1,size);
                    continue;
                }
                sendDATA(sockfd,server_ip,data,ack + 1,(int)size);
                l->info("成功发送DATA,块编号: %d,长度: %d\n",ack + 1,size);
                if (file.eof())
                    end = ack + 1;
                break;
            }
            case 5:
            {
                l->error("发生错误:%s\n",buf+4);
                return -1;
            }
            default:
                break;
        }
    }
}