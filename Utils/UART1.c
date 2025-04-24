// UART1.c
#include "UART1.h"
QueueHandle_t USART3_RxQueue = NULL;
QueueHandle_t USART1_RxQueue = NULL;
QueueHandle_t USART2_RxQueue = NULL;

/**
 * @brief  控制蜂鸣器
 * @note   控制蜂鸣器的引脚为PB14，
 * @param
 * @param
 * @retval true:
   */
void Buzzer_Beep(uint8_t times, uint16_t duration) {
    for(uint8_t i = 0; i < times; i++) {
        GPIO_ResetBits(GPIOB, GPIO_Pin_14);  // 蜂鸣器响
        vTaskDelay(pdMS_TO_TICKS(duration));
        GPIO_SetBits(GPIOB, GPIO_Pin_14);    // 蜂鸣器停
        if(i < times - 1) {  // 如果不是最后一次，则等待一下
            vTaskDelay(pdMS_TO_TICKS(200));  // 间隔200ms
        }
    }
}
/**
 * @brief  初始化蜂鸣器
 * @note   配置蜂鸣器的引脚为PB14，
 * @param
 * @param
 * @retval true:
   */
void Buzzer_Init(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  // 上拉输入
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

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
 * @brief  初始化ADC
 * @note   配置ADC1的引脚为PA1和PA4，采样模拟数据，
 * @param  
 * @param  
 * @retval true: 
   */
void AD_Init(void)
{
	// 开启时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	// 设置ADC时钟: 选择时钟6分频(即: 72MHz/6 = 12MHz)
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
	// GPIO初始化
	GPIO_InitTypeDef gpioInit;
	gpioInit.GPIO_Mode = GPIO_Mode_AIN;
	gpioInit.GPIO_Pin =  GPIO_Pin_1 | GPIO_Pin_4; // 使用PA1和PA4采样模拟数据
	gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInit);
	
	// ADC初始化
	ADC_InitTypeDef ADC_InitStructure;	
// ADC模式选择: 暂时选择默认独立工作模式(即只使用ADC1)
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
// 数据对齐(和采样数据精度相关)
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
// 选择触发采集模式(不用外部硬件触发, 使用软件触发)
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
// 不进行连续转化, 不开启, 必须触发过来, 才进行采样
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
// 不开启扫描模式, 因为我们准备每个组设置一个采样通道(通过设置, 采样, 修改设置, 采样的方式)
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
// 开启的通道数(给扫描模式指明扫描几个通过个数用的)
	ADC_InitStructure.ADC_NbrOfChannel = 1;
// 初始化
	ADC_Init(ADC1, &ADC_InitStructure);
	// 启动ADC
	ADC_Cmd(ADC1, ENABLE);
	
	// ADC校准: 提高ADC采样精度的固定硬件电路
	ADC_ResetCalibration(ADC1);	
	while (ADC_GetResetCalibrationStatus(ADC1) == SET);
	ADC_StartCalibration(ADC1);
	while (ADC_GetCalibrationStatus(ADC1) == SET);
}

/**
 * @brief  用于获取 ADC（模数转换器）采样值的函数。这个函数接收一个通道参数，并返回该通道的 ADC 转换结果。
 * @param  
 * @param  
 * @retval true: 
   */
uint16_t AD_GetValue(uint8_t ADC_Channel)
{
// 在每次AD转换前配置规则组，这样可以灵活更改AD转换的通道
// 第一个参数: 采用ADC1还是ADC2
// 第二个参数: 使用那个通道, 比如ADC_Channel_1表示PA1通道; ADC_Channel_4表示PA4通道
// 第三个参数: 规则组可以有最多16个通道，可以指定该通道在规则组中的采样顺序
// 第四个参数: 对输入电压采样的时间长短; ADC_SampleTime_55Cycles5表示采样55.5个ADC时钟节拍
	ADC_RegularChannelConfig(ADC1, ADC_Channel, 1, ADC_SampleTime_55Cycles5);

// 软件触发一次ADC1采样
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);	
// 等待采样结束: 采样是否结束, 高根据寄存器EOC标志位来判断
	while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
// 读取采样结果, 返回
	return ADC_GetConversionValue(ADC1);
}
/**
 * @brief  获取两个传感器的 ADC 采样值，并将结果格式化为字符串存储在传入的缓冲区中
 * @param  
 * @param  
 * @retval true: 
   */
void getMSG(char * bufADC){

	// 获得红外反射传感器的状态
	//单次启动ADC，转换通道1: PA1
	uint16_t AD1 = AD_GetValue(ADC_Channel_1);		
	
	// 获得光敏电阻传感器的状态
	//单次启动ADC，转换通道3
	uint16_t AD4 = AD_GetValue(ADC_Channel_4);		

	// 将传感器数据格式化为JSON数组格式
	sprintf(bufADC, "[\"%u\",\"%u\",\"sensor\"]", AD1, AD4);
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


// USART3发送字节
void USART3_SendByte(uint8_t Byte) {
   USART_SendData(USART3, Byte);
   while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
}
void printf3(char *format, ...) {
   char strs[100] = {0};
   va_list list;
   va_start(list, format);
   vsprintf(strs, format, list);
   va_end(list);
   for (uint8_t i = 0; strs[i] != '\0'; i++) {
       USART3_SendByte(strs[i]);
   }
}




