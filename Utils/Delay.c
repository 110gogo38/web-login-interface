#include "Delay.h"

/**
  * @brief  提供微秒级延时
  * @param  us 延时时长，范围：0~233015
  * @retval 无
  * @note   在系统时钟为 72 MHz 时，最大延时为 233015 微秒
  */
void Delay_Us(uint32_t us) {
    if (us > 233015) {
        return; // 超出范围，直接返回
    }
    SysTick->LOAD = 72 * us;        // 设置定时器重装值
    SysTick->VAL = 0x00;           // 清空当前计数值
    SysTick->CTRL = 0x00000005;    // 设置时钟源为 HCLK，启动定时器

    while (!(SysTick->CTRL & 0x00010000)); // 等待计数到 0

    SysTick->CTRL = 0x00000004;    // 关闭定时器
}

/**
  * @brief  提供毫秒级延时
  * @param  ms 延时时长，范围：0~4294967295
  * @retval 无
  */
void Delay_Ms(uint32_t ms) {
    while (ms--) {
        Delay_Us(1000);
    }
}

/**
  * @brief  提供秒级延时
  * @param  s 延时时长，范围：0~4294967295
  * @retval 无
  */
void Delay_S(uint32_t s) {
    while (s--) {
        Delay_Ms(1000);
    }
}
