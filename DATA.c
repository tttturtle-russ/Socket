//
// Created by russ on 23-9-13.
//

#include "DATA.h"

/*
 * Description:发送一个DATA到服务器
 * @param sockfd: socket文件描述符，在unix是一个int,在windowns环境下是一个Socket
 * @param d: 要发送的DATA结构体
 * @param dest: 目标地址
 */
ssize_t sendDATA(int sockfd,DATA* d,struct sockaddr_in dest){
    // 一个DATA有两个字节的操作码，两个字节的块编号，数据，一个0字节
    int len = strlen(d->data) + 2 + 2;
    // 为发送的数据分配内存
    char buf[len];
    // 将数据写入内存
    sprintf(buf,"%c%c%c%c%s",0,3,d->num[0],d->num[1],d->data);
    // 调用sendto发送数据
    return sendto(sockfd,buf,len,0,(struct sockaddr*)&dest,sizeof(dest));
}

unsigned short getDATABlockNum(const char* buf,DATA* d){
    // 先转换成unsigned short
    unsigned short num = buf[2] << 8 | buf[3];
    num += 1;
    // 再转换成char
    d->num[0] = num >> 8;
    d->num[1] = num & 0xff;
    return num;
}