//
// Created by russ on 23-9-14.
//

#include "ACK.h"

/*
 * Description:发送一个ACK到服务器
 * @param sockfd: socket文件描述符，在unix是一个int,在windowns环境下是一个Socket
 * @param ack: 要发送的ACK结构体
 * @param dest: 目标地址
 */
ssize_t sendACK(int sockfd,ACK* ack,struct sockaddr_in server_in){
    char buf[4];
    sprintf(buf,"%c%c%c%c",0,4,ack->blockNum[0],ack->blockNum[1]);
    return sendto(sockfd,buf,4,0,(struct sockaddr*)&server_in, sizeof(server_in));
}

unsigned short getACKBlockNum(const char* buf,ACK* ack){
    // 将结果重新存储回buf中
    ack->blockNum[0] = buf[2]; // 高字节
    ack->blockNum[1] = buf[3];        // 低字节
    // 将buf中的数据转换为无符号短整型
    return (unsigned short)buf[2] << 8 | (unsigned short)buf[3];
}
