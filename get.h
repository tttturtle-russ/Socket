//
// Created by russ on 23-9-15.
//

#ifndef SOCKET_CORE_GET_H
#define SOCKET_CORE_GET_H
#include "RRQ.h"
#include "WRQ.h"
#include "ACK.h"
#include "log/log.h"

ssize_t get_file(const char *filename, const char *mode, unsigned long long int *progress, logger *l,void (*signal) (unsigned long long ));

#endif //SOCKET_CORE_GET_H
