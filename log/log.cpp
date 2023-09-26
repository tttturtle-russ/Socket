//
// Created by russ on 23-9-17.
//

#include "log.h"
const std::string logger::level_colors[] = {
        "\x1b[32m", // Green for info
        "\x1b[33m", // Yellow for warning
        "\x1b[31m"  // Red for error
};

const std::string logger::level_strings[] = {
        "INFO", "WARN","ERROR"
};


// 生成如下时间格式:
// [%04d-%02d-%02d %02d:%02d:%02d:%03d]
void get_timestamp(char* buf){
    struct timeval tv{};
    struct tm* tm;
    gettimeofday(&tv, nullptr);
    tm = localtime(&tv.tv_sec);
    sprintf(buf,"[%04d-%02d-%02d %02d:%02d:%02d:%03d]",
            tm->tm_year+1900,tm->tm_mon+1,tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_sec,(int)tv.tv_usec/1000);
}


void logger::info(const char *fmt, ...) {
    if (LOG_INFO < this->level) {
        return;
    }
    std::string msg;
    va_list ap;
    va_start(ap, fmt);
    char buf[1024];
    char timestamp[32];
    get_timestamp(timestamp);
    vsprintf(buf,fmt,ap);
    msg = std::string(timestamp) + " "+level_strings[LOG_INFO];
    for (const auto &item: this->stream){
        if (item) {
            *item << msg << "：" <<  buf ;
        }
    }
    std::cout << level_colors[LOG_INFO] << msg << "\x1b[0m：" << buf;
    msg += "：";
    msg += buf;
    emit logInfo(QString::fromStdString(msg));
}

void logger::error(const char *fmt, ...) {
    if (LOG_ERROR < this->level) {
        return ;
    }
    std::string msg;
    va_list ap;
    va_start(ap, fmt);
    char buf[1024];
    char timestamp[32];
    get_timestamp(timestamp);
    vsprintf(buf,fmt,ap);
    msg = std::string(timestamp) + " "+level_strings[LOG_ERROR];
    for (const auto &item: this->stream){
        if (item) {
            *item << msg << "：" <<  buf ;
        }
    }
    std::cout << level_colors[LOG_ERROR] << msg << "\x1b[0m：" << buf ;
    msg += "：";
    msg += buf;
    emit logError(QString::fromStdString(msg));
}

void logger::warn(const char *fmt, ...) {
    if (LOG_WARN < this->level) {
        return ;
    }
    std::string msg;
    va_list ap;
    va_start(ap, fmt);
    char buf[1024];
    char timestamp[32];
    get_timestamp(timestamp);
    vsprintf(buf,fmt,ap);
    msg = std::string(timestamp) + " "+level_strings[LOG_WARN];
    for (const auto &item: this->stream){
        if (item) {
            *item << msg << "：" <<  buf ;
        }
    }
    std::cout << level_colors[LOG_WARN] << msg << "\x1b[0m：" << buf ;
    msg += "：";
    msg += buf;
    emit logWarn(QString::fromStdString(msg));
}
