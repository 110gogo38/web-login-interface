// UART1.h
#ifndef __UART1_H__
#define __UART1_H__
#include "stm32f10x.h" 
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "stdbool.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "semphr.h"

#define MAX_BUF_SIZE 100
#define MAX_TOKENS 10 // 最大的token数量
#define AT_COMMAND "AT\r\n"
#define AT_CWMODE_DEF "AT+CWMODE_DEF=1\r\n"
#define AT_CWJAP_DEF "AT+CWJAP_DEF?\r\n"
//#define CWJAP_COMMAND "AT+CWJAP_DEF=\"OnePlus12\",\"12345asdfg\"\r\n"
#define CIPSTART_COMMAND "AT+CIPSTART=\"TCP\",\"14.103.233.120\",8000\r\n"
#define AT_OPEN_CIPSEND "AT+CIPMODE=1\r\n"
#define AT_CIPSEND "AT+CIPSEND\r\n"
//#define AT_EXIT +++
#define AT_CIPMODE "AT+CIPMODE=0\r\n"
#define AT_CIPCLOSE "AT+CIPCLOSE\r\n"

extern QueueHandle_t USART3_RxQueue;
extern QueueHandle_t USART1_RxQueue;
extern QueueHandle_t USART2_RxQueue;
extern char buf[100] ;
extern char buf2[100] ;
extern char sensorData[100];

void USART1_Init(void);
void USART2_Init(void);
void USART3_Init(void);
void USART3_IRQHandler(void);
void USART2_IRQHandler(void);
void USART1_IRQHandler(void);
void NVIC_Init1(void);
void NVIC_Init2(void);
void NVIC_Init3(void);
void AD_Init(void);
void getMSG(char * buf);
uint16_t AD_GetValue(uint8_t ADC_Channel);
void USART1_SendByte(uint8_t Byte);
void printf1(char *format, ...);
void USART2_SendByte(uint8_t Byte);
void printf2(char *format, ...);
void USART3_SendByte(uint8_t Byte);
void printf3(char *format, ...);
int fputc(int ch,FILE *p);
bool waitForResponse(uint32_t timeout);
void Buzzer_Init(void);
void Buzzer_Beep(uint8_t times, uint16_t duration);
#endif
