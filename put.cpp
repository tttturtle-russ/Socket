//
// Created by russ on 23-9-15.
//

#include "put.h"
#include "log/log.h"

/*
 * Description:发送一个文件到服务器
 * @param filename: 要发送的文件名
 * @param mode: 传输模式
 */
ssize_t put_file(const char* filename,const char* mode,unsigned long long *progress,logger* l,void (*signal) (unsigned long long )){
    int sockfd;
    unsigned short block_num = 0,end_num = 0;
    struct sockaddr_in server_in{};
    struct sockaddr_in client_addr{};
    WRQ wrq ;
    socklen_t server_len = sizeof(server_in);
    ssize_t len;
    char buf[MAX_BUFFER_SIZE + 4];
    DATA data;
    FILE * fp = fopen(filename,"r");
    if (fp == nullptr){
        perror("open file");
        return 1;
    }
    l->info("成功打开文件: %s\n",filename);
    // 为wrq结构体赋值
    // 参数filename是绝对路径，需要去掉路径
    const char *p = strrchr(filename,'/');
    if (p == nullptr){
        strcpy(wrq.filename,filename);
    } else{
        strcpy(wrq.filename,p+1);
    }
    strcpy(wrq.mode, mode);
    // 创建socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        l->error("创建socket失败: %s\n",strerror(errno));
        exit(1);
    }
    l->info("成功创建socket,句柄%d\n",sockfd);
    // 构造服务器地址
    server_in.sin_family = AF_INET;
    server_in.sin_port = htons(69);
    inet_pton(AF_INET, "localhost", &server_in.sin_addr);
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(0);
    // 绑定客户端地址,主要是端口，更具体的说是C-TID
    if (bind(sockfd,(struct sockaddr*)&client_addr,sizeof(client_addr)) < 0) {
        l->error("绑定端口失败: %s\n",strerror(errno));
        return 1;
    }
    // 调用封装好的发送wrq函数
    if (sendWRQ(sockfd,&wrq) < 0) {
        l->error("发送WRQ失败: %s\n",strerror(errno));
        return 1;
    }
    l->info("成功发送WRQ\n");
    while (1) {
        // 调用recvfrom函数接收服务器发回的数据并保存服务器信息到server_in结构体中
        len = recvfrom(sockfd, buf, MAX_BUFFER_SIZE + 4, 0, (struct sockaddr *)&server_in, &server_len);
        if (len == -1){
            perror("recvfrom");
            return 1;
        }
        switch (buf[1]) {
            // 4表示接收到ACK,发送DATA回去
            case 4:
            {
                // 获取ACK中的块编号
                unsigned short num = getDATABlockNum(buf,&data);
                if (block_num != num)
                {
                    size_t n = fread(data.data,1,MAX_BUFFER_SIZE,fp);
                    data.data[n] = '\0';
                    *progress += n;
                    block_num = num;
                }
                if (sendDATA(sockfd,&data,server_in) < 0) {
                    l->error("发送DATA失败: %s\n",strerror(errno));
                    return 1;
                }
                emit signal(*progress);
                if (end_num == block_num) {
                    close(sockfd);
                    fclose(fp);
                    l->info("成功发送文件: %s\n",filename);
//                    return block_num;
                    return 0;
                }
                if (strlen(data.data) < 512) {
                    end_num = block_num + 1;
                }
                break;
            }
            // 5表示接收到ERROR
            case 5:
            {
                l->error("接收到ERROR: %s\n",buf+4);
                return 1;
            }
            default:
                break;
        }
    }
}