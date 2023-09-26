//
// Created by russ on 23-9-13.
//

#ifndef SOCKET_CORE_WRQ_H
#define SOCKET_CORE_WRQ_H

#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
typedef struct WRQ{
    char filename[64];
    char mode[8];
}WRQ;

ssize_t sendWRQ(int sockfd,WRQ* wrq);

#endif //SOCKET_CORE_WRQ_H
