#include "header.h"
#include <errno.h>
// 聊天室服务端
typedef struct conn_s{
    int netfd;
    int isalive;
    int isClient1;  // 标记是否为客户端1
} conn_t;

// 全局缓冲区，用于存储JSON数据
char json_buffer[4096] = {0};
int buffer_size = 0;
int buffer_ready = 0;

// 日志文件指针
FILE *log_file = NULL;

// 写入日志的函数
void write_log(const char *format, ...) {
    if (log_file == NULL) {
        return;
    }
    
    // 获取当前时间
    time_t now = time(NULL);
    struct tm *tm_now = localtime(&now);
    char time_str[64];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_now);
    
    // 写入时间戳
    fprintf(log_file, "[%s] ", time_str);
    
    // 写入日志内容
    va_list args;
    va_start(args, format);
    vfprintf(log_file, format, args);
    va_end(args);
    
    // 添加换行符并刷新缓冲区
    fprintf(log_file, "\n");
    fflush(log_file);
}

int main(int argc,char*argv[]){
    // 打开日志文件
    log_file = fopen("log.txt", "a+");
    if (log_file == NULL) {
        perror("无法打开日志文件");
        return 1;
    }
    
    write_log("服务器启动");
    
    // 启动socket函数, 获得对应的socket_fd描述符
    int sofd = socket(AF_INET, SOCK_STREAM, 0);
    if (sofd == -1) {
        write_log("创建socket失败: %s", strerror(errno));
        fclose(log_file);
        return 1;
    }
    write_log("创建socket成功，描述符: %d", sofd);

    // 创建服务端的地址和端口
    struct sockaddr_in sockaddr;
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = inet_addr("0.0.0.0");
    sockaddr.sin_port = htons(atoi("8000"));

    // 绑定端口ip
    if (bind(sofd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) == -1) {
        write_log("绑定端口失败: %s", strerror(errno));
        close(sofd);
        fclose(log_file);
        return 1;
    }
    write_log("绑定端口成功: 0.0.0.0:8000");
    
    // 监听端口
    if (listen(sofd, 10) == -1) {
        write_log("监听端口失败: %s", strerror(errno));
        close(sofd);
        fclose(log_file);
        return 1;
    }
    write_log("监听端口成功，等待连接...");

    // 构建select监听的集合: fd_set类型
    fd_set set;
    // 清空set: 初始化set
    FD_ZERO(&set);
    FD_SET(sofd, &set);

    // 客户端的存在个数
    int index = 0;  // 初始化index为0
    // 客户端数组: 用于保存所有链接服务器的客户端
    conn_t list[1024];
    memset(list, 0, sizeof(list));

    printf("服务器已启动，等待客户端连接...\n");

    while(1){
        // 构建此次的监听集合
        fd_set temp_set;
        // 把set记录复制到本次监听集合
        memcpy(&temp_set, &set, sizeof(set));

        // select开始监听有没有就绪事件
        select(1024, &temp_set, NULL, NULL, NULL);

        // 判断是否有socket数据达到:即socket就绪
        if(FD_ISSET(sofd, &temp_set)){
            // 获得一个新的客户端链接
            int netfd = accept(sofd, NULL, NULL);
            if (netfd == -1) {
                write_log("接受连接失败: %s", strerror(errno));
                continue;
            }
            
            // 保存到""客户端""链接数组中
            list[index].isalive = 1;
            list[index].netfd = netfd;
            
            // 第一个连接的客户端标记为客户端1
            if(index == 0){
                list[index].isClient1 = 1;
                printf("客户端1已连接，将接收JSON数据\n");
                write_log("客户端1已连接，将接收JSON数据，描述符: %d", netfd);
            } else {
                list[index].isClient1 = 0;
                printf("客户端%d已连接\n", index + 1);
                write_log("客户端%d已连接，描述符: %d", index + 1, netfd);
            }
            
            // 下一次增加监听这个客户端
            FD_SET(netfd, &set);

            index++;
        }
        
        // 遍历客户端列表 
        for(int i=0; i<index; i++){
            // 判断这个客户端是否存活, 以及是否就绪: 即是否有消息到来
            if(list[i].isalive == 1 && FD_ISSET(list[i].netfd, &temp_set)){
                // 该客户端存活, 且处于就绪状态
                // 读取客户端的到达信息
                char buf[4096] = {0};
                int res_recv = recv(list[i].netfd, buf, sizeof(buf) - 1, 0);
                
                // 如果返回值为0, 说明这个客户端已经断开
                if(res_recv == 0){
                    printf("客户端 %d 断开连接\n", i + 1);
                    write_log("客户端 %d 断开连接", i + 1);
                    // 把客户端数组中该标记置为0: 即非存活状态
                    list[i].isalive = 0;
                    // 取消后续对次客户端的select监控
                    FD_CLR(list[i].netfd, &set);
                    // 关闭这个客户端
                    close(list[i].netfd);
                    continue;
                }
                
                buf[res_recv] = '\0';  // 确保字符串结束
                
                printf("收到客户端 %d 消息(%d字节):\n", i + 1, res_recv);
                printf("------------------------\n");
                printf("%s\n", buf);
                printf("------------------------\n");
                write_log("收到客户端 %d 消息(%d字节)", i + 1, res_recv);
                
                // 获取HTTP方法和路径
                char method[10] = {0};
                char path[256] = {0};
                sscanf(buf, "%9s %255s", method, path);
                write_log("解析HTTP请求: 方法=%s, 路径=%s", method, path);
                
                // 客户端1直接写入文件，不判断POST
                if(list[i].isClient1 == 1){
                    write_log("客户端1发送数据，准备写入文件");
                    
                    // 检查是否是JSON数组格式
                    char *json_start = strchr(buf, '[');
                    if(json_start){
                        // 找到JSON数组的开始位置
                        char *json_end = strchr(json_start, ']');
                        if(json_end){
                            // 找到JSON数组的结束位置
                            *(json_end + 1) = '\0'; // 在']'后截断字符串
                            
                            // 将JSON数据存入全局缓冲区
                            int json_len = strlen(json_start);
                            if(json_len < sizeof(json_buffer)){
                                memset(json_buffer, 0, sizeof(json_buffer));
                                strcpy(json_buffer, json_start);
                                buffer_size = json_len;
                                buffer_ready = 1; // 标记缓冲区有有效数据
                                write_log("JSON数据已存入缓冲区，长度: %d字节", json_len);
                            }
                            
                            // 打开文件准备写入
                            int fp = open("file.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
                            if(fp != -1){
                                // 直接写入整个JSON数组
                                int bytes_written = write(fp, json_start, strlen(json_start));
                                write(fp, "\r\n", 2);  // 添加\r\n换行符
                                
                                close(fp);
                                write_log("JSON数组已写入file.txt文件，共写入 %d 字节", bytes_written + 2);
                                
                                // 发送HTTP响应
                                char response[] = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 2\r\n\r\nOK";
                                send(list[i].netfd, response, strlen(response), 0);
                                write_log("向客户端1发送成功响应");
                            } else {
                                write_log("打开file.txt文件失败: %s", strerror(errno));
                                // 发送错误响应
                                char response[] = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/plain\r\nContent-Length: 5\r\n\r\nERROR";
                                send(list[i].netfd, response, strlen(response), 0);
                                write_log("向客户端1发送错误响应: 500 Internal Server Error");
                            }
                        } else {
                            write_log("警告：JSON格式不正确，未找到结束符']'");
                            // 发送错误响应
                            char response[] = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\nContent-Length: 15\r\n\r\nInvalid JSON data";
                            send(list[i].netfd, response, strlen(response), 0);
                            write_log("向客户端1发送错误响应: 400 Bad Request (Invalid JSON data)");
                        }
                    } else {
                        write_log("警告：未找到JSON数组格式");
                        // 发送错误响应
                        char response[] = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\nContent-Length: 15\r\n\r\nInvalid JSON data";
                        send(list[i].netfd, response, strlen(response), 0);
                        write_log("向客户端1发送错误响应: 400 Bad Request (Invalid JSON data)");
                    }
                } 
                // 客户端2请求数据
                else if(strcmp(method, "GET") == 0 && strcmp(path, "/1") == 0){
                    printf("收到客户端 %d 的GET /1请求，准备发送文件数据\n", i + 1);
                    write_log("收到客户端 %d 的GET /1请求，准备发送文件数据", i + 1);
                    
                    // 读取文件内容
                    char file_content[4096] = {0};
                    int fp = open("file.txt", O_RDONLY);
                    
                    if(fp != -1){
                        // 获取文件大小
                        off_t file_size = lseek(fp, 0, SEEK_END);
                        
                        if(file_size > 0){
                            // 从文件开头读取所有内容
                            lseek(fp, 0, SEEK_SET);
                            int bytes_read = read(fp, file_content, sizeof(file_content) - 1);
                            
                            if(bytes_read > 0){
                                file_content[bytes_read] = '\0';
                                
                                // 构建HTTP响应
                                char response_header[512] = {0};
                                sprintf(response_header, 
                                    "HTTP/1.1 200 OK\r\n"
                                    "Content-Type: application/json\r\n"
                                    "Content-Length: %d\r\n"
                                    "\r\n", bytes_read);
                                
                                // 发送HTTP响应头
                                send(list[i].netfd, response_header, strlen(response_header), 0);
                                // 发送文件内容
                                send(list[i].netfd, file_content, bytes_read, 0);
                                
                                printf("已向客户端 %d 发送文件内容，长度: %d 字节\n", i + 1, bytes_read);
                                write_log("已向客户端 %d 发送文件内容，长度: %d 字节", i + 1, bytes_read);
                            } else {
                                // 读取失败
                                write_log("读取file.txt文件失败: %s", strerror(errno));
                                char response[] = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/plain\r\nContent-Length: 17\r\n\r\nFailed to read file";
                                send(list[i].netfd, response, strlen(response), 0);
                                write_log("向客户端 %d 发送错误响应: 500 Internal Server Error", i + 1);
                            }
                        } else {
                            // 文件为空
                            write_log("file.txt文件为空");
                            char response[] = "HTTP/1.1 204 No Content\r\nContent-Type: text/plain\r\nContent-Length: 10\r\n\r\nEmpty file";
                            send(list[i].netfd, response, strlen(response), 0);
                            write_log("向客户端 %d 发送响应: 204 No Content", i + 1);
                        }
                        close(fp);
                    } else {
                        // 文件不存在
                        write_log("file.txt文件不存在");
                        char response[] = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\nContent-Length: 9\r\n\r\nNOT FOUND";
                        send(list[i].netfd, response, strlen(response), 0);
                        write_log("向客户端 %d 发送响应: 404 Not Found", i + 1);
                    }
                } else {
                    // 其他请求
                    printf("收到未知请求，发送帮助信息\n");
                    write_log("收到未知请求，方法=%s，路径=%s", method, path);
                    char help_msg[] = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\nContent-Length: 77\r\n\r\n"
                                     "Usage:\n- Client 1: Send JSON data\n- Client 2: Send 'GET /1' to retrieve data";
                    send(list[i].netfd, help_msg, strlen(help_msg), 0);
                    write_log("向客户端 %d 发送帮助信息", i + 1);
                }
            }
        }
    }
    
    write_log("服务器关闭");
    close(sofd);
    fclose(log_file);
    return 0;
}