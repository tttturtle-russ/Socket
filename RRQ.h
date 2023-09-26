//
// Created by russ on 23-9-13.
//

#ifndef SOCKET_RRQ_H
#define SOCKET_RRQ_H
#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

typedef struct RRQ {
    char filename[64];
    char mode[8];
} RRQ;

ssize_t sendRRQ(int sockfd,RRQ* rrq);

#endif //SOCKET_RRQ_H
