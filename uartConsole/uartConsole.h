/*
 * @Author       : stoneBeast
 * @Date         : 2025-01-21 16:26:22
 * @Encoding     : UTF-8
 * @LastEditors  : stoneBeast
 * @LastEditTime : 2025-01-22 10:56:16
 * @Description  : uartConsole包对外暴露的接口
 */

#ifndef __UART_CONSOLE_H
#define __UART_CONSOLE_H

#include "config.h"

/* task结构体 */
typedef struct 
{
    char task_name[TASK_NAME_LEN];              /* task名称 */
    char task_desc[TASK_DESC_LEN];              /* task描述 */
    int (*task_func)(int argc, char* argv[]);   /* task函数 */
}Task_t;

void init_hardware(void);
void init_console_task(void);
void console_start(void);
void console_task_register(Task_t* task);

#endif // !__UART_CONSOLE_H
