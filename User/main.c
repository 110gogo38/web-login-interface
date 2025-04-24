#include "stm32f10x.h"
#include "UART1.h"
#include "Delay.h"
#include "OLED.h"

char buf[MAX_BUF_SIZE] = {0};
char buf2[MAX_BUF_SIZE] = {0};
char sensorData[100];
char *tokenArray[MAX_TOKENS] = {0}; // 存储分割后的字符串数组
TaskHandle_t task2Handle = NULL;
TaskHandle_t task1Handle = NULL;
TaskHandle_t task3Handle = NULL;
TaskHandle_t taskADCHandle = NULL;
QueueHandle_t ESP01sResponseQueue = NULL;
typedef struct {
    bool success;           // 命令执行是否成功
    char command[20];       // 当前执行的命令
    char response[100];     // 完整的响应内容
} ESP01sResponse_t;

SemaphoreHandle_t xMutex;

/**
* @brief 手机发送信息给蓝牙，蓝牙发送到stm32,到pc
 * @param
 * @param
 * @retval true:
   */
/**
 * @brief 任务1，从 USART3 接收队列中读取数据，处理包含 '!' 的特定格式数据并进行分割
 * @param arg 任务创建时传递的参数，此任务未使用该参数
 * @retval 无
 */
void task1(void * arg) {
    // 打印任务开始信息
    // printf1("task1\r\n");
    // 用于存储从队列接收到的字符
    char receivedChar;
    // buf 数组的索引，用于记录当前存储位置
    uint16_t bufIndex = 0;
    // 记录接收到的 '!' 字符的数量
    uint8_t exclamationCount = 0;

    while (1) {
        // 从队列接收数据，等待时间为portMAX_DELAY（永久等待）
        BaseType_t xQueue = xQueueReceive(USART3_RxQueue, &receivedChar, portMAX_DELAY);

        if (xQueue == pdPASS) {
            // 将接收到的字符存入buf数组
            if (bufIndex < MAX_BUF_SIZE - 1) {
                // 将接收到的字符存入 buf 数组
                buf[bufIndex++] = receivedChar;
                // 确保字符串以 null 结尾
                buf[bufIndex] = '\0';

                // 如果收到'!'，可以进行处理
                if (receivedChar == '!') {
                    // '!' 字符数量加 1
                    exclamationCount++;

                    // 当收到两个 '!' 字符时进行处理
                    if (exclamationCount == 2) {
                        // 去掉最后的 '!'
                        buf[bufIndex - 1] = '\0';
                        // 去掉第一个 '!'
                        char *processedBuf = buf + 1;
                        // 打印处理后的字符串
                        printf1("%s \r\n", processedBuf);
                        // 用于存储分割后的子字符串
                        char *token;
                        // 当前的 token 数量
                        uint8_t tokenCount = 0;
                        // 以 '=' 为分隔符分割字符串
                        token = strtok(processedBuf, "=");

                        if (xMutex != NULL) {
                            // 获取互斥锁，确保线程安全
                            if (xSemaphoreTake(xMutex, pdMS_TO_TICKS(500)) == pdTRUE) {
                                // 循环处理每个分割后的字符串
                                while (token != NULL && tokenCount < MAX_TOKENS) {
                                    // 存储分割后的字符串
                                    tokenArray[tokenCount] = token;
                                    // 打印分割后的字符串
                                    printf1("[%s]", tokenArray[tokenCount]);
                                    tokenCount++;
                                    // 继续分割字符串
                                    token = strtok(NULL, "=");
                                }

                                // 释放互斥锁
                                xSemaphoreGive(xMutex);
                            } else {
                                // 打印获取互斥锁失败的信息
                                printf1("Failed to get mutex\r\n");
                            }
                        }

                        // 打印 token 的数量
                        printf1("\r\ntokenCount = %d\r\n", tokenCount);
                        // 打印第一个 token
                        //   printf1("Token 3: %s\r\n", tokenArray[0]);

                        // 如果 token 数量大于 1 且 task2 句柄有效，则通知 task2
                        if (tokenCount > 1 && task2Handle != NULL) {
                            // 通知 task2
                            xTaskNotify(task2Handle, 0x01, eSetBits) ;
                            // 打印通知信息
                            //printf1("xTaskNotify\r\n");
                            // vTaskDelay(pdMS_TO_TICKS(100));
                        }

                        // 打印第一个 token
                        //  printf1("Token 4: %s\r\n", tokenArray[1]);
                        // 清空 buf 数组
                        //  memset(buf, 0, sizeof(buf));
                        // 重置 buf 数组索引
                        bufIndex = 0;
                        // 重置 '!' 字符数量
                        exclamationCount = 0;
                    }
                }
            }
        } else  {
            // 打印从队列接收数据失败的信息
            printf1("Failed to receive data from queue\n");
            // 任务延迟 10 毫秒
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
}

/**
 * @brief  输入指令到ESP01s
 * @param
 * @param
 * @retval true:
   */
/**
 * @brief 任务2，负责与 ESP01S 模块进行通信，处理蓝牙数据并配置 ESP01S
 * @param arg 任务创建时传递的参数，此任务未使用该参数
 * @retval 无
 */
void task2(void * arg) {
    // 打印任务开始信息
    // printf1("task2\r\n");
    // 向 ESP01S 模块发送 AT 指令，用于测试模块是否正常响应
    // printf2(AT_COMMAND);
    // 任务延迟 1 秒，等待模块响应
    // vTaskDelay(pdMS_TO_TICKS(1000));
    // 打印提示信息
    //  printf1("666\r\n");
    // 向 ESP01S 模块发送设置工作模式的指令
    uint8_t retryCount = 0;
    const uint8_t MAX_RETRIES = 5;  // 最大重试5次
    
    // 设置工作模式
retry_mode_setting:
    printf2(AT_CWMODE_DEF);
    if (waitForResponse(5000)) {
        printf1("set_mode_successfully\r\n");
        retryCount = 0;
    } else {
        printf1("setting_mode_failed\r\n");
        if (retryCount++ < MAX_RETRIES) {
            vTaskDelay(pdMS_TO_TICKS(1000));
            goto retry_mode_setting;
        } else {
            printf1("max retries reached, giving up\r\n");
            retryCount = 0;
        }
    }

    // 任务延迟 1 秒，等待模块响应
    // vTaskDelay(pdMS_TO_TICKS(1000));
    // 定义变量用于存储任务通知值
    uint32_t ulNotificationValue;
    // 定义变量用于存储任务通知等待结果
    BaseType_t xResult;
    // 打印等待蓝牙数据的提示信息
    printf1("waiting_for_bluetooth_data...\r\n");

    // 等待任务通知，清除所有通知位并将通知值存储到 ulNotificationValue
    xResult = xTaskNotifyWait(0x00, 0xFFFFFFFF, &ulNotificationValue, portMAX_DELAY);
    // 任务延迟 100 毫秒
    vTaskDelay(pdMS_TO_TICKS(100));

    if (xResult == pdTRUE) {
        printf1("received_bluetooth_data\r\n");
        vTaskDelay(pdMS_TO_TICKS(100));

        if (xMutex != NULL) {
            if (xSemaphoreTake(xMutex, pdMS_TO_TICKS(500)) == pdTRUE) {
                // WiFi连接重试逻辑
            retry_wifi_connect:
                printf2("AT+CWJAP_DEF=\"%s\",\"%s\"\r\n", tokenArray[0], tokenArray[1]);
                vTaskDelay(pdMS_TO_TICKS(10000));
                if (waitForResponse(5000)) {
                    // 修改WiFi连接相关的打印
                    if (waitForResponse(5000)) {
                        printf1("wifi_connected_successfully\r\n");
                        retryCount = 0;
                        printf3("{\"status\":0,\"wifi_name\":\"%s\"}\r\n", tokenArray[0]);
                    } else {
                        printf1("wifi_connection_failed\r\n");
                        printf3("{\"status\":1,\"message\":\"wifi connection failed\"}\r\n");
                    }
                    if (retryCount++ < MAX_RETRIES) {
                        vTaskDelay(pdMS_TO_TICKS(1000));
                        goto retry_wifi_connect;  // 只重试WiFi连接部分
                    } else {
                        printf1("MAX RETRIES REACHED, GIVING UP\r\n");
                       // Buzzer_Beep(2, 200);
                        retryCount = 0;
                    }
                }
            }
            xSemaphoreGive(xMutex);
        }
    }

    // 向 ESP01S 模块发送查询 Wi-Fi 的指令
    retry_wifi_query:
        printf2(AT_CWJAP_DEF);
        // 等待响应并检查结果
        
            // 修改WiFi查询相关的打印
            if (waitForResponse(5000)) {
                printf1("wifi_query_successfully\r\n");
                retryCount = 0;
            }else {
                printf1("wifi_query_failed\r\n");
            
            if (retryCount++ < MAX_RETRIES) {
                vTaskDelay(pdMS_TO_TICKS(1000));
                goto retry_wifi_query;
            } else {
                printf1("MAX RETRIES REACHED, GIVING UP\r\n");
                //Buzzer_Beep(3, 500);
                retryCount = 0;
            }
        }
        
        // 任务延迟 1 秒
        vTaskDelay(pdMS_TO_TICKS(1000));
    
        // 向 ESP01S 模块发送建立 TCP/UDP 连接的指令
        retry_tcp_connection:
            printf2(CIPSTART_COMMAND);
            // 等待响应并检查结果
            if (waitForResponse(5000)) {
                printf1("TCP_CONNECTION_ESTABLISHED\r\n");
                retryCount = 0;
            } else {
                printf1("TCP_CONNECTION_FAILED\r\n");
                if (retryCount++ < MAX_RETRIES) {
                    vTaskDelay(pdMS_TO_TICKS(1000));
                    goto retry_tcp_connection;
                } else {
                    printf1("MAX RETRIES REACHED, GIVING UP\r\n");
                    //Buzzer_Beep(4, 300);
                    retryCount = 0;
                }
            }
            // 任务延迟 2 秒
            vTaskDelay(pdMS_TO_TICKS(2000));
    
            // 向 ESP01S 模块发送打开透传模式的指令
        retry_transparent_mode:
            printf2(AT_OPEN_CIPSEND);
            // 等待响应并检查结果
            if (waitForResponse(5000)) {
                printf1("transparent_mode_enabled\r\n");
                retryCount = 0;
            } else {
                printf1("transparent_mode_failed\r\n");
                if (retryCount++ < MAX_RETRIES) {
                    vTaskDelay(pdMS_TO_TICKS(1000));
                    goto retry_transparent_mode;
                } else {
                    printf1("max retries reached, giving up\r\n");
                    //Buzzer_Beep(6, 500);
                    retryCount = 0;
                }
            }
            // 任务延迟 1 秒
            vTaskDelay(pdMS_TO_TICKS(1000));

    // 向 ESP01S 模块发送开始透传发送数据的指令
    printf2(AT_CIPSEND);
    // 任务延迟 100 毫秒
    vTaskDelay(pdMS_TO_TICKS(1000));

    printf2("Hello,world\r\n");
    vTaskDelay(pdMS_TO_TICKS(1000));

    while (1) {
        printf2("%s\r\n", sensorData);
        vTaskDelay(pdMS_TO_TICKS(2000));
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == 0) {  // 按键按下（低电平）
            vTaskDelay(pdMS_TO_TICKS(20));  // 消抖
            if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == 0) {
                printf1("Button pressed, exiting sensor data loop\r\n");
                break;  // 跳出循环
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    printf2("+++");
    vTaskDelay(pdMS_TO_TICKS(1000));

    printf2(AT_CIPMODE);
    vTaskDelay(pdMS_TO_TICKS(1000));

    printf2(AT_CIPCLOSE);
    vTaskDelay(pdMS_TO_TICKS(1000));

    // 任务进入无限循环，保持任务存活
    while (1) {
        // 任务延迟 10 毫秒
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

/**
 * @brief  接收ESP01S响应并转发回USART1
 * @param
 * @param
 * @retval true:
   */
void task3(void * arg) {
    // printf1("task3\r\n");
    char receivedChar_task3;
    uint16_t bufIndex3 = 0;
    //bool responseComplete = false;
    //char currentCommand[20] = {0};

    while (1) {
        // 从USART2接收队列读取数据
        BaseType_t xQueue_task3 = xQueueReceive(USART2_RxQueue, &receivedChar_task3, portMAX_DELAY);

        if (xQueue_task3 == pdPASS) {
            // 通过USART1发送出去
            if (bufIndex3 < MAX_BUF_SIZE - 1) {
                buf2[bufIndex3++] = receivedChar_task3;
                buf2[bufIndex3] = '\0';

                //printf1("%s", buf2);
                if (receivedChar_task3 == '\n') {
                    printf1("%s\r\n", buf2);  // 添加这行来显示接收到的完整数据

                    if (strstr(buf2, "OK") != NULL) {
                        //printf1("OK\r\n");
                        ESP01sResponse_t response = {true};
                        strncpy(response.response, buf2, sizeof(response.response) - 1);
                        BaseType_t xQueueSendResult;
                        xQueueSendResult = xQueueSend(ESP01sResponseQueue, &response, pdMS_TO_TICKS(100));

                        if (xQueueSendResult != pdPASS) {
                            printf1("Failed to send response to queue\r\n");
                        }
                    } else if (strstr(buf2, "ERROR") != NULL || strstr(buf2, "FAIL") != NULL) {
                        printf1("ERROR\r\n");
                        ESP01sResponse_t response = {false};
                        strncpy(response.response, buf2, sizeof(response.response) - 1);
                        xQueueSend(ESP01sResponseQueue, &response, 0);
                    }

                    memset(buf2, 0, sizeof(buf2));
                    bufIndex3 = 0;
                }

            } else {
                printf1("Failed to receive data from queue3\n");
                vTaskDelay(pdMS_TO_TICKS(10));
                bufIndex3 = 0;
            }
        }
    }
}

/**
 * @brief
 * @param
 * @param
 * @retval true:
   */
bool waitForResponse(uint32_t timeout) {
    ESP01sResponse_t response;

    if (xQueueReceive(ESP01sResponseQueue, &response, pdMS_TO_TICKS(timeout)) == pdPASS) {
        return response.success;
    }

    printf1("waiting_for_response_timeout\r\n");
    return false;
}

/**
 * @brief  ADC任务
 * @param
 * @param
 * @retval true:
   */
void taskADC(void * arg) {
    // printf1("taskADC started\r\n");
    //vTaskDelay(pdMS_TO_TICKS(100));

    while (1) {
        getMSG(sensorData);
        OLED_ShowString(2, 1, "Sensor Data:");
        OLED_ShowString(3, 1, sensorData);
        printf1("Sensor JSON: %s\r\n", sensorData);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

/**
 * @brief  ADC监控任务函数
 * @param  - 任务参数
 * @retval None
 */
void ADC_Monitor_Task(void * arg) {
    const uint16_t ADC_THRESHOLD = 2000;  // ADC阈值设置为2000
    
    while(1) {
        // 获取PA4的ADC值
        uint16_t adcValue = AD_GetValue(ADC_Channel_4);
        
        // 判断是否超过阈值
        if(adcValue >= ADC_THRESHOLD) {
            // 触发蜂鸣器响一次，持续500ms
            Buzzer_Beep(5, 1000);
        }
        
        // 延时100ms再次检查
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

int main(void) {
    //printf1("main_Init\r\n");
    USART1_Init();
    USART2_Init();
    USART3_Init();
    Buzzer_Init();

    NVIC_Init1();
    NVIC_Init2();
    NVIC_Init3();
    
    AD_Init();
    

    OLED_Init();
    ESP01sResponseQueue = xQueueCreate(5, sizeof(ESP01sResponse_t));
    OLED_ShowString(1, 3, "KITCHEN!");
    xMutex = xSemaphoreCreateMutex();

    if (xMutex == NULL) {
        // 处理创建互斥锁失败的情况
        printf1("Failed to create mutex\n");
    }
    

    //GPIO_ResetBits(GPIOB, GPIO_Pin_14);
    //   Delay_S(1);

    xTaskCreate(task1, "Task1", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, &task1Handle);
    xTaskCreate(task2, "Task2", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, &task2Handle);
    xTaskCreate(task3, "Task3", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, &task3Handle);
    xTaskCreate(taskADC, "taskADC", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, &taskADCHandle);
    xTaskCreate(ADC_Monitor_Task, "ADC_Monitor", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
    vTaskStartScheduler();

    while (1) {
    }

}

