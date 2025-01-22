/*** 
 * @Author       : stoneBeast
 * @Date         : 2025-01-21 16:30:58
 * @Encoding     : UTF-8
 * @LastEditors  : stoneBeast
 * @LastEditTime : 2025-01-21 17:39:54
 * @Description  : 可移植层，针对不同的平台实现规定的硬件api
 */

#include "stm32f10x.h"
#include "config.h"

void init_systick(void)
{
    SysTick_Config(SystemCoreClock / TICKS_PER_SECOND);
}

void init_status_led(void)
{
    RCC_APB2PeriphClockCmd(STATUS_LED_RCC, ENABLE);

    GPIO_InitTypeDef gpioDef;
    gpioDef.GPIO_Mode = GPIO_Mode_Out_PP;
    gpioDef.GPIO_Pin = STATUS_LED_PIN;
    gpioDef.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(STATUS_LED_PORT, &gpioDef);
}

void init_console_uart(void)
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

void enable_uart_interrupt(void)
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

int get_console_uart_it_flag(uint16_t it)
{
    return USART_GetITStatus(CONSOLE_UART, it);
}

unsigned char get_console_uart_data(void)
{
    return USART_ReceiveData(CONSOLE_UART);
}

void clear_console_uart_idel_it_flag(void)
{
    uint32_t temp;
    temp = CONSOLE_UART->SR;
    temp |= CONSOLE_UART->DR;
}

int _write(int fd, char* ptr, int len) 
{
    while (len--)
    {
            USART_SendData(CONSOLE_UART, *ptr);
            while (USART_GetFlagStatus(CONSOLE_UART, USART_FLAG_TXE) == RESET);
            ptr++;
    }

    return len;
}

void delay_ms(uint32_t ms)
{
    uint32_t start = g_Ticks;
    while (g_Ticks - start < ms);
}

void led_blink(void)
{
    GPIO_WriteBit(STATUS_LED_PORT, STATUS_LED_PIN, !GPIO_ReadOutputDataBit(STATUS_LED_PORT, STATUS_LED_PIN));
}
