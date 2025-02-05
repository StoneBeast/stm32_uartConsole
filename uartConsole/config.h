/*
 * @Author       : stoneBeast
 * @Date         : 2025-01-21 16:35:23
 * @Encoding     : UTF-8
 * @LastEditors  : stoneBeast
 * @LastEditTime : 2025-02-05 18:45:17
 * @Description  : 存放硬件的某些配置项，但配置项也是根据特定的平台来定义的，没有做到真正的完全解耦
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include "stm32f10x.h"

#define TICKS_PER_SECOND 1000

#define STATUS_LED_RCC RCC_APB2Periph_GPIOA
#define STATUS_LED_PORT GPIOA
#define STATUS_LED_PIN GPIO_Pin_8

#define CONSOLE_UART_RCC RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1
#define CONSOLE_UART_PORT GPIOA
#define CONSOLE_UART_TX_PIN GPIO_Pin_9
#define CONSOLE_UART_RX_PIN GPIO_Pin_10
#define CONSOLE_UART USART1
#define CONSOLE_UART_IRQ USART1_IRQn
#define CONSOLE_UART_BAUDRATE 115200
#define CONSOLE_IT_RXEN USART_IT_RXNE
#define CONSOLE_IT_IDLE USART_IT_IDLE
#define CONSOLE_UART_IRQ_HANDLER USART1_IRQHandler
#define CONSOLE_TIMEBASE_HANDLER SysTick_Handler

#define TASK_NAME_LEN           32
#define TASK_DESC_LEN           256

#define USE_PRIVATE_TICKS       1

#define G_TICKS         get_ticks()

#endif // !__CONFIG_H
