// UART1.c
#include "UART1.h"
QueueHandle_t USART3_RxQueue = NULL;
QueueHandle_t USART1_RxQueue = NULL;
QueueHandle_t USART2_RxQueue = NULL;

/**
 * @brief  初始化USART3
 * @note   配置USART3的引脚为PB10和PB11，波特率为9600，
 * @param  
 * @param  
 * @retval true: 
   */
void USART3_Init(void) {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    // 初始化引脚B10,B11
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PB10:USART3_TX
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PB11:USART3_RX
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // USART3配置
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART3, &USART_InitStructure);

    //启动USART3
    USART_Cmd(USART3, ENABLE);
}

/**
 * @brief  初始化USART2
 * @note   配置USART2的引脚为PA2和PA3，波特率为115200，
 * @param  
 * @param  
 * @retval true: 
   */
  void USART2_Init(void) {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // 初始化引脚A2,A3
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;//PA2:USART2_TX
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA3:USART2_RX
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // USART2配置
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART2, &USART_InitStructure);


    // 启动USART2
    USART_Cmd(USART2, ENABLE);
}

/**
 * @brief  初始化USART1
 * @note   配置USART2的引脚为PA9和PA10，波特率为115200，
 * @param  
 * @retval true: 
   */
void USART1_Init(void) {
    // 开启时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);


    // 初始化引脚A9,A10
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;//PA9:USART2_TX
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10:USART2_RX
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // USART1配置
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART1, &USART_InitStructure);

    // 启动USART1
    USART_Cmd(USART1, ENABLE);

}

/**
 * @brief  初始化USART3输入中断1
 * @note   创建消息队列
 * @param  
 * @param  
 * @retval true: 
   */
void NVIC_Init1(void) {
    // 初始化中断: 用中断, 读取串口1回传的信息
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 12;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	
    USART3_RxQueue = xQueueCreate(MAX_BUF_SIZE, sizeof(char));
    if (USART3_RxQueue == NULL) {
        // 队列创建失败处理
        printf1("error");
    }
}
/**
 * @brief  初始化USART1中断
 * @param  
 * @param  
 * @retval true: 
   */
void NVIC_Init2(void) {
    // 初始化中断: 用中断, 读取串口1回传的信息
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 12;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	
    USART1_RxQueue = xQueueCreate(MAX_BUF_SIZE, sizeof(char));
    if (USART1_RxQueue == NULL) {
        // 队列创建失败处理
        printf1("error");
    }
}
/**
 * @brief  初始化USART2中断
 * @param  
 * @param  
 * @retval true: 
   */
  void NVIC_Init3(void) {
    // 初始化中断: 用中断, 读取串口1回传的信息
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 12;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	
    USART2_RxQueue = xQueueCreate(MAX_BUF_SIZE, sizeof(char));
    if (USART2_RxQueue == NULL) {
        // 队列创建失败处理
        printf1("error");
    }
}

//串口3的中断处理函数
void USART3_IRQHandler(void) {
    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) {
        // 读取一个字节
        uint8_t data = USART_ReceiveData(USART3);

        // 将数据发送到队列
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xQueueSendFromISR(USART3_RxQueue, &data, &xHigherPriorityTaskWoken);
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
    }
}
/**
 * @brief  串口1的中断处理函数
 * @param  
 * @param  
 * @retval true: 
   */
void USART1_IRQHandler(void) {
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
        // 读取一个字节
        uint8_t data = USART_ReceiveData(USART1);
        // 将数据发送到队列
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xQueueSendFromISR(USART1_RxQueue, &data, &xHigherPriorityTaskWoken);
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}
/**
 * @brief  串口2的中断处理函数
 * @param  
 * @param  
 * @retval true: 
   */
  void USART2_IRQHandler(void) {
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
        // 读取一个字节
        uint8_t data = USART_ReceiveData(USART2);
        // 将数据发送到队列
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xQueueSendFromISR(USART2_RxQueue, &data, &xHigherPriorityTaskWoken);
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
    }
}
/**
 * @brief  输出重定向到USART2
 * @param  
 * @param  
 * @retval true: 
   */
// int fputc(int ch,FILE *p)  //函数默认的，在使用printf函数时自动调用
// {
// 	USART_SendData(USART2,(u8)ch);	
// 	while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);
// 	return ch;
// }

// USART1发送字节
void USART1_SendByte(uint8_t Byte) {
    USART_SendData(USART1, Byte);

    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}


void printf1(char *format, ...) {
    char strs[100] = {0};

    va_list list;
    va_start(list, format);
    vsprintf(strs, format, list);
    va_end(list);

    for (uint8_t i = 0; strs[i] != '\0'; i++) {
        USART1_SendByte(strs[i]);
    }
}

// USART2发送字节
void USART2_SendByte(uint8_t Byte) {
    USART_SendData(USART2, Byte);

    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
}


void printf2(char *format, ...) {
    char strs[100] = {0};

    va_list list;
    va_start(list, format);
    vsprintf(strs, format, list);
    va_end(list);

    for (uint8_t i = 0; strs[i] != '\0'; i++) {
        USART2_SendByte(strs[i]);
    }
}


//// USART3发送字节
//void USART3_SendByte(uint8_t Byte) {
//    USART_SendData(USART3, Byte);

//    while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
//}


//void printf3(char *format, ...) {
//    char strs[100] = {0};

//    va_list list;
//    va_start(list, format);
//    vsprintf(strs, format, list);
//    va_end(list);

//    for (uint8_t i = 0; strs[i] != '\0'; i++) {
//        USART3_SendByte(strs[i]);
//    }
//}
