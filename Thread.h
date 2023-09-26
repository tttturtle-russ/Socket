//
// Created by russ on 23-9-21.
//

#ifndef SOCKET_THREAD_H
#define SOCKET_THREAD_H

#include <QThread>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include "log/log.h"


class ThreadWork : public QThread{
    Q_OBJECT

public:
    explicit ThreadWork(logger* l,QObject* parent = nullptr)
            : l(l),QThread(parent), progress(0)
    {
    }

    void setParams(const std::string & _filename, const std::string& _mode,const int& _method)
    {
        filename = _filename;
        mode = _mode;
        method = _method;
    }

    void setProgress(unsigned long long value)
    {
        progress = value;
    }

    [[nodiscard]] unsigned long long getProgress() const{
        return progress;
    }

    ssize_t sendRRQ(int sockfd, struct sockaddr_in server_ip, const std::string& _filename, const std::string& _mode);
    ssize_t sendDATA(int sockfd,struct sockaddr_in server_ip,const char* data,unsigned short n,int size);
    ssize_t sendACK(int sockfd,struct sockaddr_in server_ip,unsigned short n);
    ssize_t sendWRQ(int sockfd, struct sockaddr_in server_ip, const std::string& _filename, const std::string& _mode);
    ssize_t upload(const std::string& _filename, const std::string& _mode);
    ssize_t download(const std::string& _filename, const std::string& _mode);

protected:
    void run() override
    {
        ssize_t result;
        switch (method) {
            case 0:
                result = this->upload(filename,mode);
                if (result) {
                    l->error("上传文件发生错误，错误原因：%s",strerror(errno));
                    this->exit(result);
                }
                break;
            case 1:
                result = this->download(filename,mode);
                if (result) {
                    l->error("下载文件发生错误，错误原因：%s",strerror(errno));
                    this->exit(result);
                }
                break;
            default:
                break;
        }
    }

    signals:
        void progressChanged(unsigned long long progress);

private:
    unsigned long long progress;
    std::string filename;
    std::string mode;
    logger* l;
    int method{};
    char buffer[16];
    char packet[516];
};


#endif //SOCKET_THREAD_H
