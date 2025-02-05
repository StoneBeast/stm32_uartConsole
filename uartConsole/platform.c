/*** 
 * @Author       : stoneBeast
 * @Date         : 2025-01-21 16:30:58
 * @Encoding     : UTF-8
 * @LastEditors  : stoneBeast
 * @LastEditTime : 2025-02-05 18:33:24
 * @Description  : 可移植层，针对不同的平台实现规定的硬件api
 */

#include "stm32f10x.h"
#include "hardware.h"
#include "config.h"

#if USE_PRIVATE_TICKS
volatile uint32_t g_Ticks = 0;              /* 全局滴答计数 */
#endif  // USE_PRIVATE_TICKS

void __USER init_timebase(void)
{
    SysTick_Config(SystemCoreClock / TICKS_PER_SECOND);
}

void __USER init_status_led(void)
{
    RCC_APB2PeriphClockCmd(STATUS_LED_RCC, ENABLE);

    GPIO_InitTypeDef gpioDef;
    gpioDef.GPIO_Mode = GPIO_Mode_Out_PP;
    gpioDef.GPIO_Pin = STATUS_LED_PIN;
    gpioDef.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(STATUS_LED_PORT, &gpioDef);
}

void __USER init_console_uart(void)
{
    GPIO_InitTypeDef gpioDef;
    USART_InitTypeDef usartDef;

    RCC_APB2PeriphClockCmd(CONSOLE_UART_RCC, ENABLE);

    gpioDef.GPIO_Mode = GPIO_Mode_AF_PP;
    gpioDef.GPIO_Pin = CONSOLE_UART_TX_PIN;
    gpioDef.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(CONSOLE_UART_PORT, &gpioDef);

    gpioDef.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpioDef.GPIO_Pin = CONSOLE_UART_RX_PIN;
    GPIO_Init(CONSOLE_UART_PORT, &gpioDef);

    usartDef.USART_BaudRate = CONSOLE_UART_BAUDRATE;
    usartDef.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    usartDef.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    usartDef.USART_Parity = USART_Parity_No;
    usartDef.USART_StopBits = USART_StopBits_1;
    usartDef.USART_WordLength = USART_WordLength_8b;
    USART_Init(CONSOLE_UART, &usartDef);

    USART_Cmd(CONSOLE_UART, ENABLE);
}

void __USER enable_uart_interrupt(void)
{
    NVIC_InitTypeDef nvicDef;
    nvicDef.NVIC_IRQChannel = CONSOLE_UART_IRQ;
    nvicDef.NVIC_IRQChannelCmd = ENABLE;
    nvicDef.NVIC_IRQChannelPreemptionPriority = 0;
    nvicDef.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&nvicDef);

    USART_ITConfig(CONSOLE_UART, USART_IT_RXNE, ENABLE);
    USART_ITConfig(CONSOLE_UART, USART_IT_IDLE, ENABLE);
}

int __USER get_console_uart_it_flag(uint16_t it)
{
    return USART_GetITStatus(CONSOLE_UART, it);
}

unsigned char __USER get_console_uart_data(void)
{
    return USART_ReceiveData(CONSOLE_UART);
}

void __USER clear_console_uart_idel_it_flag(void)
{
    uint32_t temp;
    temp = CONSOLE_UART->SR;
    temp |= CONSOLE_UART->DR;
}

void __USER console_uart_send_data(uint8_t data)
{
    USART_SendData(CONSOLE_UART, data);
    while (USART_GetFlagStatus(CONSOLE_UART, USART_FLAG_TXE) == RESET);
}

void delay_ms(uint32_t ms)
{
    uint32_t start = G_TICKS;
    while (G_TICKS - start < ms);
}

void running_led_blink(void)
{
    GPIO_WriteBit(STATUS_LED_PORT, STATUS_LED_PIN, !GPIO_ReadOutputDataBit(STATUS_LED_PORT, STATUS_LED_PIN));
}

uint32_t __USER get_ticks(void)
{
#if USE_PRIVATE_TICKS
    return g_Ticks;
#else
    // __USER get ticks;
    /* don't use private ticks */
#endif
}

void __USER inc_ticks(void)
{
#if USE_PRIVATE_TICKS
    g_Ticks++;
#else
    // __USER increase ticks;
    /* don't use private ticks */
#endif
}


