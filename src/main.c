/*** 
 * @Author       : stoneBeast
 * @Date         : 2025-01-15 16:10:30
 * @Encoding     : UTF-8
 * @LastEditors  : stoneBeast
 * @LastEditTime : 2025-01-22 17:12:05
 * @Description  : 以stm32f103为平台，实现串口终端功能
 */

#include "stm32f10x.h"
#include "uartConsole.h"

int main(void)
{
    init_hardware();
    init_console_task();
    console_start();
}
