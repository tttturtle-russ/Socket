//
// Created by russ on 23-9-21.
//

#include "Thread.h"


ssize_t ThreadWork::sendRRQ(int sockfd, struct sockaddr_in server_ip, const std::string &_filename,
                            const std::string &_mode) {
    std::string newName = _filename.substr(_filename.find_last_of('/') + 1);
    int len = 2 + newName.length() + 1 + _mode.length() + 1;
    sprintf(buffer, "%c%c%s%c%s%c", 0, 1, newName.c_str(), 0, _mode.c_str(), 0);
    return sendto(sockfd, buffer,len, 0, (struct sockaddr*)&server_ip, sizeof(server_ip));
}

ssize_t ThreadWork::sendACK(int sockfd,struct sockaddr_in server_ip,unsigned short n){
    sprintf(buffer,"%c%c%c%c",0,4,n >> 8,n & 0xff);
    return sendto(sockfd,buffer,4,0,(struct sockaddr*)&server_ip,sizeof(server_ip));
}

ssize_t ThreadWork::sendWRQ(int sockfd, struct sockaddr_in server_ip, const std::string& _filename, const std::string& _mode){
    std::string newName = _filename.substr(_filename.find_last_of('/') + 1);
    int len = 2 + newName.length() + 1 + _mode.length() + 1;
    sprintf(packet, "%c%c%s%c%s%c", 0, 2, newName.c_str(), 0, _mode.c_str(), 0);
    return sendto(sockfd,packet,len,0,(struct sockaddr*)&server_ip,sizeof(server_ip));
}

ssize_t ThreadWork::sendDATA(int sockfd,struct sockaddr_in server_ip,const char* data,unsigned short n,int size){
    sprintf(packet,"%c%c%c%c%s",0,3,n >> 8,n & 0xff,data);
    return sendto(sockfd,packet,size + 4,0,(struct sockaddr*)&server_ip,sizeof(server_ip));
}