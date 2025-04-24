#include "header.h"

int main() {
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
 
    char *port = "8000";
    char *ip = "0.0.0.0";
    struct sockaddr_in sockaddr;
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(atoi(port));
    sockaddr.sin_addr.s_addr = inet_addr(ip);
    int reuse = 1;
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    bind(socket_fd, (struct sockaddr *)&sockaddr, sizeof(sockaddr));
    listen(socket_fd, 10);
 
    while(1) {  // 外层循环，持续接受新的连接
        printf("等待新的客户端连接...\n");
        int net_fd = accept(socket_fd, NULL, NULL);
        if(net_fd < 0) {
            printf("接受连接失败\n");
            continue;
        }
        
        printf("客户端已连接\n");
        char buf[1024] = {0};
        // 接收客户端的初始请求
        recv(net_fd, buf, sizeof(buf), 0);
        
        // 检查是否是GET请求
        if (strstr(buf, "GET ") == NULL) {
            printf("未找到GET请求，关闭连接\n");
            close(net_fd);
            continue;
        }
        
        printf("收到GET请求：%s\n", buf);
        
        // 持续发送数据
        while(1) {
            char *data = "[\"test1\",\"test2\",\"test3\"]";
            int ret = send(net_fd, data, strlen(data), 0);
            
            if(ret <= 0) {
                printf("客户端已断开连接\n");
                break;
            }
            
            printf("发送数据: %s\n", data);
            sleep(1);  // Linux系统下使用sleep，参数单位为秒
        }
        
        close(net_fd);
    }
    
    close(socket_fd);
    return 0;
}