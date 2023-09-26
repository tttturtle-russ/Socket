//
// Created by russ on 23-9-21.
//

#include "Thread.h"

//ssize_t ThreadWork::put_file() {
//    int sockfd;
//    unsigned short block_num = 0,end_num = 0;
//    struct sockaddr_in server_in{};
//    struct sockaddr_in client_addr{};
//    WRQ wrq ;
//    socklen_t server_len = sizeof(server_in);
//    ssize_t len;
//    char buf[MAX_BUFFER_SIZE + 4];
//    DATA data;
//    FILE * fp = fopen(filename.c_str(),"r");
//    if (fp == nullptr){
//        perror("open file");
//        return 1;
//    }
//    l->info("成功打开文件: %s\n",filename.c_str());
//    // 为wrq结构体赋值
//    // 参数filename是绝对路径，需要去掉路径
//    const char *p = strrchr(filename.c_str(),'/');
//    if (p == nullptr){
//        strcpy(wrq.filename,filename.c_str());
//    } else{
//        strcpy(wrq.filename,p+1);
//    }
//    strcpy(wrq.mode, mode.c_str());
//    // 创建socket
//    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
//        l->error("创建socket失败: %s\n",strerror(errno));
//        exit(1);
//    }
//    l->info("成功创建socket,句柄%d\n",sockfd);
//    // 构造服务器地址
//    server_in.sin_family = AF_INET;
//    server_in.sin_port = htons(69);
//    inet_pton(AF_INET, "localhost", &server_in.sin_addr);
//    memset(&client_addr, 0, sizeof(client_addr));
//    // 调用封装好的发送wrq函数
//    if (sendWRQ(sockfd,&wrq) < 0) {
//        l->error("发送WRQ失败: %s\n",strerror(errno));
//        return 1;
//    }
//    l->info("成功发送WRQ\n");
//    while (1) {
//        // 调用recvfrom函数接收服务器发回的数据并保存服务器信息到server_in结构体中
//        len = recvfrom(sockfd, buf, MAX_BUFFER_SIZE + 4, 0, (struct sockaddr *)&server_in, &server_len);
//        if (len == -1){
//            perror("recvfrom");
//            return 1;
//        }
//        switch (buf[1]) {
//            // 4表示接收到ACK,发送DATA回去
//            case 4:
//            {
//                // 获取ACK中的块编号
//                unsigned short num = getDATABlockNum(buf,&data);
//                if (block_num != num)
//                {
//                    size_t n = fread(data.data,1,MAX_BUFFER_SIZE,fp);
//                    data.data[n] = '\0';
//                    progress += n;
//                    block_num = num;
//                }
//                if (sendDATA(sockfd,&data,server_in) < 0) {
//                    l->error("发送DATA失败: %s\n",strerror(errno));
//                    return 1;
//                }
//                l->info("成功发送DATA,块编号: %d\n",block_num);
//                emit progressChanged(progress);
//                if (end_num == block_num) {
//                    close(sockfd);
//                    fclose(fp);
//                    l->info("成功发送文件: %s\n",filename.c_str());
////                    return block_num;
//                    return 0;
//                }
//                if (strlen(data.data) < 512) {
//                    end_num = block_num + 1;
//                }
//                break;
//            }
//                // 5表示接收到ERROR
//            case 5:
//            {
//                l->error("接收到ERROR: %s\n",buf+4);
//                return 1;
//            }
//            default:
//                break;
//        }
//    }
//}
//
//ssize_t ThreadWork::get_file() {
//    int sockfd;
//    unsigned short block_num = 0;
//    struct sockaddr_in server_in{};
//    struct sockaddr_in client_addr{};
//    RRQ rrq;
//    socklen_t server_len = sizeof(server_in);
//    ssize_t len;
//    char buf[MAX_BUFFER_SIZE + 4];
//    ACK ack;
//    FILE * fp = fopen(filename.c_str(),"w");
//    // 为rrq结构体赋值
//    // 参数filename是绝对路径，需要去掉路径
//    const char *p = strrchr(filename.c_str(), '/');
//    if (p == nullptr){
//        strcpy(rrq.filename,filename.c_str());
//    } else{
//        strcpy(rrq.filename,p+1);
//    }
//    strcpy(rrq.mode, mode.c_str());
//    // 创建socket
//    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
//        l->error("创建socket失败: %s\n",strerror(errno));
//        exit(1);
//    }
//    l->info("成功创建socket,句柄%d\n",sockfd);
//    // 构造服务器地址
//    server_in.sin_family = AF_INET;
//    server_in.sin_port = htons(69);
//    inet_pton(AF_INET, "localhost", &server_in.sin_addr);
//    // 调用封装好的发送rrq函数
//    if (sendRRQ(sockfd,&rrq) < 0) {
//        perror("sendRRQ");
//        return 1;
//    }
//    while (1) {
//        // 调用recvfrom函数接收服务器发回的数据并保存服务器信息到server_in结构体中
//        len = recvfrom(sockfd, buf, MAX_BUFFER_SIZE + 4, 0, (struct sockaddr *)&server_in, &server_len);
//        if (len == -1){
//            perror("recvfrom");
//            return 1;
//        }
//        switch (buf[1]) {
//            // 3表示接收到DATA,发送ACK回去
//            case 3:
//            {
//                // 从DATA中获取块号
//                unsigned short num = getACKBlockNum(buf,&ack);
//                if (block_num != num)
//                {
//                    progress += fwrite(buf+4,1,len - 4,fp);
//                    block_num = num;
//                }
//                if (sendACK(sockfd,&ack,server_in) < 0) {
//                    perror("sendACK");
//                    return 1;
//                }
//                emit progressChanged(progress);
//                break;
//            }
//                // 5表示接收到ERROR
//            case 5:
//            {
//                printf("ERROR: %s\n",buf+4);
//                return 1;
//            }
//            default:
//                break;
//        }
//        if (len < MAX_BUFFER_SIZE + 4) {
//            printf("get file: %s successfully\n",filename.c_str());
//            close(sockfd);
//            fclose(fp);
//            break;
//        }
//    }
//    return 0;
//}
