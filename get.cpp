//
// Created by russ on 23-9-15.
//


#include "get.h"

/*
 * Description:发送一个RRQ请求到服务器
 * @param sockfd: socket文件描述符，在unix是一个int,在windowns环境下是一个Socket
 * @param rrq: 要发送的RRQ结构体
 */
ssize_t get_file(const char *filename, const char *mode, unsigned long long int *progress, logger *l,void (*signal) (unsigned long long )) {
    int sockfd;
    unsigned short block_num = 0;
    struct sockaddr_in server_in{};
    struct sockaddr_in client_addr{};
    RRQ rrq;
    socklen_t server_len = sizeof(server_in);
    ssize_t len;
    char buf[MAX_BUFFER_SIZE + 4];
    ACK ack;
    FILE * fp = fopen(filename,"w");
    // 为rrq结构体赋值
    // 参数filename是绝对路径，需要去掉路径
    const char *p = strrchr(filename, '/');
    if (p == nullptr){
        strcpy(rrq.filename,filename);
    } else{
        strcpy(rrq.filename,p+1);
    }
    strcpy(rrq.mode, mode);
    // 创建socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        l->error("创建socket失败: %s\n",strerror(errno));
        exit(1);
    }
    l->info("成功创建socket,句柄%d\n",sockfd);
    // 构造服务器地址
    server_in.sin_family = AF_INET;
    server_in.sin_port = htons(69);
    inet_pton(AF_INET, "localhost", &server_in.sin_addr);
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(0);
    // 绑定客户端地址,主要是端口，更具体的说是C-TID
    if (bind(sockfd,(struct sockaddr*)&client_addr,sizeof(client_addr)) < 0) {
        perror("bind");
        return 1;
    }
    // 调用封装好的发送rrq函数
    if (sendRRQ(sockfd,&rrq) < 0) {
        perror("sendRRQ");
        return 1;
    }
    while (1) {
        // 调用recvfrom函数接收服务器发回的数据并保存服务器信息到server_in结构体中
        len = recvfrom(sockfd, buf, MAX_BUFFER_SIZE + 4, 0, (struct sockaddr *)&server_in, &server_len);
        if (len == -1){
            perror("recvfrom");
            return 1;
        }
        switch (buf[1]) {
            // 3表示接收到DATA,发送ACK回去
            case 3:
            {
                // 从DATA中获取块号
                unsigned short num = getACKBlockNum(buf,&ack);
                if (block_num != num)
                {
                    *progress += fwrite(buf+4,1,len - 4,fp);
                    block_num = num;
                }
                if (sendACK(sockfd,&ack,server_in) < 0) {
                    perror("sendACK");
                    return 1;
                }
                emit signal(*progress);
                break;
            }
                // 5表示接收到ERROR
            case 5:
            {
                printf("ERROR: %s\n",buf+4);
                return 1;
            }
            default:
                break;
        }
        if (len < MAX_BUFFER_SIZE + 4) {
            printf("get file: %s successfully\n",filename);
            close(sockfd);
            fclose(fp);
            break;
        }
    }
    return 0;
}
