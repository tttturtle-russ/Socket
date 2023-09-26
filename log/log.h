//
// Created by russ on 23-9-17.
//

#ifndef SOCKET_CORE_LOG_H
#define SOCKET_CORE_LOG_H

#include <iostream>
#include <cstdarg>
#include <ctime>
#include <sys/time.h>
#include <vector>
#include <fstream>
#include <QObject>

enum { LOG_INFO,LOG_WARN, LOG_ERROR };


class logger:public QObject{

    Q_OBJECT
public:
    logger(int level,const std::vector<std::string> &streams,QObject* parent = nullptr): QObject(parent),level(level){
        for (const auto &item: streams)
        {
            auto *fs = new std::ofstream (item,std::ios::out|std::ios::app);
            if (!fs->is_open()){
                std::cerr << "open file " << item << " failed" << std::endl;
                exit(1);
            }
            stream.push_back(fs);
        }


    };

    ~logger(){
        for (auto & i : stream){
            i->close();
            delete i;
            i = nullptr;
        }
    }

    signals:
        void logInfo(const QString& msg);
        void logWarn(const QString& msg);
        void logError(const QString& msg);

public:
    void info(const char* fmt, ...);
    void warn(const char* fmt,...);
    void error(const char* fmt, ...);


private:
    static const std::string level_colors[];
    static const std::string level_strings[];
    std::vector<std::ofstream*> stream{};
    int level{};
};


void get_timestamp(char* buf);
#endif //SOCKET_CORE_LOG_H
