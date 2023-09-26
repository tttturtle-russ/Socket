//
// Created by russ on 23-9-13.
//

#include "RRQ.h"

/*
 * Description:发送一个RRQ请求到服务器
 * @param sockfd: socket文件描述符，在unix是一个int,在windowns环境下是一个Socket
 * @param rrq: 要发送的RRQ结构体
 */
ssize_t sendRRQ(int sockfd,RRQ* rrq) {
    // 一个RRQ有两个字节的操作码，一个文件名，一个0字节，一个模式，一个0字节
    int len = 2 + strlen(rrq->filename) + 1 + strlen(rrq->mode) + 1;
    // 为发送的数据分配内存
    char buffer[len];
    // 将数据写入内存
    sprintf(buffer,"%c%c%s%c%s%c",0,1,rrq->filename,0,rrq->mode,0);
    // 构造目标地址
    struct sockaddr_in dest;
    memset(&dest,0,sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(69);
    inet_pton(AF_INET,"127.0.0.1",&dest.sin_addr);
    // 调用sendto发送数据
    return sendto(sockfd,buffer,len,0,(struct sockaddr*)&dest,sizeof(dest));
}
