/*
 * @Author       : stoneBeast
 * @Date         : 2025-01-22 11:18:54
 * @Encoding     : UTF-8
 * @LastEditors  : stoneBeast
 * @LastEditTime : 2025-01-22 11:19:11
 * @Description  : task.h
 */

#ifndef __TASK_H_
#define __TASK_H_

#include "uartConsole.h"

void add_default_task(void);
int task_handler(uint8_t* submit, uint16_t submit_len);

#endif // !__TASK_H_
