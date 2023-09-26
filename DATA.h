//
// Created by russ on 23-9-13.
//

#ifndef SOCKET_CORE_DATA_H
#define SOCKET_CORE_DATA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
typedef struct DATA{
    char num[2];
    char data[512];
}DATA;

ssize_t sendDATA(int sockfd,DATA* d,struct sockaddr_in server_in);
unsigned short getDATABlockNum(const char* buf,DATA *d);
#endif //SOCKET_CORE_DATA_H
