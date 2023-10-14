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
        return;  // 如果日志级别低于INFO，不执行任何操作。
    }
    std::string msg;
    va_list ap;
    va_start(ap, fmt);
    char buf[1024];
    char timestamp[32];
    get_timestamp(timestamp); // 获取当前时间戳
    vsprintf(buf, fmt, ap); // 使用可变参数格式化日志消息。
    msg = std::string(timestamp) + " " + level_strings[LOG_INFO]; // 创建带时间戳和日志级别的日志消息头。
    for (const auto &item : this->stream) {
        if (item) {
            *item << msg << "：" << buf; // 将日志消息写入每个输出流。
        }
    }
    std::cout << level_colors[LOG_INFO] << msg << "\x1b[0m：" << buf; // 使用颜色打印日志消息到控制台。
    msg += "：";
    msg += buf;
    emit logInfo(QString::fromStdString(msg)); // 发出带有消息的logInfo信号。
}

void logger::error(const char *fmt, ...) {
    if (LOG_ERROR < this->level) {
        return; // 如果日志级别低于ERROR，不执行任何操作。
    }
    std::string msg;
    va_list ap;
    va_start(ap, fmt);
    char buf[1024];
    char timestamp[32];
    get_timestamp(timestamp); // 获取当前时间戳
    vsprintf(buf, fmt, ap); // 使用可变参数格式化日志消息。
    msg = std::string(timestamp) + " " + level_strings[LOG_ERROR]; // 创建带时间戳和日志级别的日志消息头。
    for (const auto &item : this->stream) {
        if (item) {
            *item << msg << "：" << buf; // 将日志消息写入每个输出流。
        }
    }
    std::cout << level_colors[LOG_ERROR] << msg << "\x1b[0m：" << buf; // 使用颜色打印日志消息到控制台。
    msg += "：";
    msg += buf;
    emit logError(QString::fromStdString(msg)); // 发出带有消息的logError信号。
}

void logger::warn(const char *fmt, ...) {
    if (LOG_WARN < this->level) {
        return; // 如果日志级别低于WARN，不执行任何操作。
    }
    std::string msg;
    va_list ap;
    va_start(ap, fmt);
    char buf[1024];
    char timestamp[32];
    get_timestamp(timestamp); // 获取当前时间戳
    vsprintf(buf, fmt, ap); // 使用可变参数格式化日志消息。
    msg = std::string(timestamp) + " " + level_strings[LOG_WARN]; // 创建带时间戳和日志级别的日志消息头。
    for (const auto &item : this->stream) {
        if (item) {
            *item << msg << "：" << buf; // 将日志消息写入每个输出流。
        }
    }
    std::cout << level_colors[LOG_WARN] << msg << "\x1b[0m：" << buf; // 使用颜色打印日志消息到控制台。
    msg += "：";
    msg += buf;
    emit logWarn(QString::fromStdString(msg)); // 发出带有消息的logWarn信号。
}
