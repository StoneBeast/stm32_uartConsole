/*** 
 * @Author       : stoneBeast
 * @Date         : 2025-01-15 16:10:30
 * @Encoding     : UTF-8
 * @LastEditors  : stoneBeast
 * @LastEditTime : 2025-01-22 16:59:35
 * @Description  : 以stm32f103为平台，实现串口终端功能
 */

#include "stm32f10x.h"
#include "uartConsole.h"
#include <stdio.h>
#include <string.h>

volatile uint32_t g_Ticks = 0; 

int main(void)
{
    init_hardware();
    init_console_task();
    console_start();
}
