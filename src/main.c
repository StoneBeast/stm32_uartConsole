/*** 
 * @Author       : stoneBeast
 * @Date         : 2025-01-15 16:10:30
 * @Encoding     : UTF-8
 * @LastEditors  : stoneBeast
 * @LastEditTime : 2025-01-21 17:40:47
 * @Description  : 以stm32f103为平台，实现串口终端功能
 */

#include "stm32f10x.h"
#include "uartConsole.h"

volatile uint32_t g_Ticks = 0; 

int main(void)
{
    init_hardware();
    console_start();
}
