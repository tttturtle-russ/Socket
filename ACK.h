//
// Created by russ on 23-9-14.
//

#ifndef SOCKET_CORE_ACK_H
#define SOCKET_CORE_ACK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
typedef struct ACK {
    char blockNum[2];
}ACK;

unsigned short getACKBlockNum(const char* buf,ACK* ack);
ssize_t sendACK(int sockfd,ACK* ack,struct sockaddr_in server_in);

#endif //SOCKET_CORE_ACK_H
