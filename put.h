//
// Created by russ on 23-9-15.
//

#ifndef SOCKET_CORE_PUT_H
#define SOCKET_CORE_PUT_H
#include <iostream>
#include "RRQ.h"
#include "WRQ.h"
#include "ACK.h"
#include "DATA.h"
#include "log/log.h"


ssize_t put_file(const char* filename,const char* mode,unsigned long long *progress,logger* l,void (*signal) (unsigned long long ));

#endif //SOCKET_CORE_PUT_H
