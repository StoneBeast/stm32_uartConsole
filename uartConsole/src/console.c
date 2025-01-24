/*** 
 * @Author       : stoneBeast
 * @Date         : 2025-01-21 16:25:45
 * @Encoding     : UTF-8
 * @LastEditors  : stoneBeast
 * @LastEditTime : 2025-01-24 10:59:43
 * @Description  : 串口终端程序的主要逻辑实现
 */

#include "hardware.h"
#include "console.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "link_list.h"
#include "task.h"

 /*
     TODO: 添加后台任务机制
     TODO: 优化解耦合逻辑
  */

console_struct g_console;                   /* 全局console结构体 */
link_list_manager *g_console_task_list;     /* 全局taks链表 */

/*** 
 * @brief 初始化console结构体
 * @return [void]
 */
static void init_console_struct(void)
{
    memset(&g_console, 0, sizeof(console_struct));
}

/*** 
 * @brief 带有标题的输出，本质是对printf的再包装
 * @param __format [char*]   模板字符串 
 * @return [void]
 */
static void console_out(const char* __format, ...)
{
    /* 先输出标题 */
    CONSOLE_TITLE();
    va_list args;
    va_start(args, __format);
    vprintf(__format, args);
    va_end(args);
    fflush(stdout);
}

/*** 
 * @brief 初始化使用到的硬件
 * @return [void]
 */
void init_hardware(void)
{
    init_systick();
    init_status_led();
    init_console_uart();
    enable_uart_interrupt();
}

/*** 
 * @brief 初始化task机制
 * @return [void]
 */
void init_console_task(void)
{
    // TODO: 从console_start中拆分出来本质是为task_register预留钩子，之后可以修改为其他机制
    g_console_task_list = link_list_manager_get();
    add_default_task();
}

/*** 
 * @brief 启动主程序
 * @return [void]
 */
void console_start(void)
{
    uint8_t temp_buffer[BUFFER_LEN] = { 0 };    /* 暂时存放接收到的数据 */
    uint16_t temp_len = 0;                      /* 接收的数据的长度 */
    int task_ret = 0;                           /* 接收task执行的结果 */

    /* 初始胡console_struct */
    init_console_struct();

    /* 打印title */
    console_out("");

    while (1)   /* 循环判断标志位 */
    {
        if (g_console.edit_frame_flag)  /* 一帧数据帧接收完成 */
        {
            /* 马上将数据取出，并复位标志位 */
            memcpy(temp_buffer, g_console.edit_frame, g_console.edit_frame_len+1);
            temp_len = g_console.edit_frame_len;
            g_console.edit_frame_len = 0;
            g_console.edit_frame_flag = 0;

            /* 处理数据 */
            if (temp_len > 1) /* 长度大于1，可能是特殊字符 */
            {
                if (ASSERENT_KEY(temp_buffer, KEY_RIGHT) && (g_console.cursor < g_console.edit_len))    /* 方向右 */
                {
                    g_console.cursor++;
                    CURSOR_RIGHT(1);
                }
                else if (ASSERENT_KEY(temp_buffer, KEY_LEFT) && (g_console.cursor > 0))                 /* 方向左 */
                {
                    g_console.cursor--;
                    CURSOR_LEFT(1);
                }
                else if (ASSERENT_KEY(temp_buffer, KEY_UP))                                             /* UP键 */
                {
                    if ((g_console.history_len > 0) && (0 != strcmp((char*)(g_console.history_buffer), (char*)(g_console.edit_buffer)))) /* 历史记录不为空且当前输入不为空 */
                    {

                        memcpy(g_console.temp_edit_buffer, g_console.edit_buffer, g_console.edit_len+1);
                        g_console.temp_edit_len = g_console.edit_len;

                        memcpy(g_console.edit_buffer, g_console.history_buffer, g_console.history_len+1);
                        g_console.edit_len = g_console.history_len;

                        g_console.cursor = g_console.edit_len;
                        g_console.edit_buffer_changed_flag = 1;
                        g_console.history_flag = 1;
                    }
                }
                else if (ASSERENT_KEY(temp_buffer, KEY_DOWN))                                           /* DOWN键 */
                {
                    if (0 != strcmp((char*)(g_console.temp_edit_buffer), (char*)(g_console.edit_buffer))) /* 临时记录不为空且当前输入不为空 */
                    {
                        memcpy(g_console.history_buffer, g_console.edit_buffer, g_console.edit_len + 1);
                        g_console.history_len = g_console.edit_len;

                        memcpy(g_console.edit_buffer, g_console.temp_edit_buffer, g_console.temp_edit_len+1);
                        g_console.edit_len = g_console.temp_edit_len;

                        g_console.cursor = g_console.edit_len;
                        g_console.edit_buffer_changed_flag = 1;
                        g_console.history_flag = 1;
                    }
                }
                else if (ASSERENT_KEY(temp_buffer, KEY_HOME))                                           /* HOME键 */
                {
                    if (g_console.cursor > 0)
                    {
                        CURSOR_LEFT(g_console.cursor);
                        g_console.cursor = 0;
                    }
                }
                else if (ASSERENT_KEY(temp_buffer, KEY_END))                                            /* END键 */
                {
                    if (g_console.cursor < g_console.edit_len)
                    {
                        CURSOR_RIGHT(g_console.edit_len - g_console.cursor);
                        g_console.cursor = g_console.edit_len;
                    }
                }
                else if (ASSERENT_KEY(temp_buffer, KEY_DELETE) && (g_console.cursor < g_console.edit_len)) /* 删除键 */
                {
                    memmove(g_console.edit_buffer + g_console.cursor, g_console.edit_buffer + g_console.cursor + 1, g_console.edit_len - g_console.cursor);
                    g_console.edit_len--;
                    g_console.edit_buffer[g_console.edit_len] = '\0';
                    g_console.delete_flag = 1;
                    g_console.edit_buffer_changed_flag = 1;
                }
            }
            else if(temp_len == 1)    /* 可能是可视字符 */
            {
                if (temp_buffer[0] >= 0x20 && temp_buffer[0] <= 0x7e) /* 可视字符 */
                {
                    memmove(g_console.edit_buffer + g_console.cursor + 1, g_console.edit_buffer + g_console.cursor, g_console.edit_len - g_console.cursor + 1);
                    g_console.edit_buffer[g_console.cursor] = temp_buffer[0];
                    g_console.edit_len++;
                    g_console.cursor++;
                    g_console.edit_buffer_changed_flag = 1;
                }
                else if (temp_buffer[0] == KEY_BACKSPACE && (g_console.cursor > 0))            /* 退格键 */
                {
                    memmove(g_console.edit_buffer + g_console.cursor - 1, g_console.edit_buffer + g_console.cursor, g_console.edit_len - g_console.cursor + 1);
                    g_console.edit_len--;
                    g_console.edit_buffer[g_console.edit_len] = '\0';
                    g_console.cursor--;
                    CURSOR_LEFT(1);
                    g_console.delete_flag = 1;
                    g_console.edit_buffer_changed_flag = 1;
                }
                else if (temp_buffer[0] == KEY_ENTER) /* 回车键 */
                {
                    /* 将当前的edit_buffer存放到submit_buffer中，并置位edit_flag */
                    memcpy(g_console.submit_buffer, g_console.edit_buffer, g_console.edit_len);
                    g_console.submit_buffer[g_console.edit_len] = '\0';
                    g_console.submit_len = g_console.edit_len;
                    CLEAR_BUFFER(g_console.edit_buffer);
                    g_console.edit_len = 0;
                    g_console.cursor = 0;
                    g_console.edit_flag = 1;
                    CONSOLE_NEW_LINE();
                }
            }

        }
        if (g_console.edit_buffer_changed_flag) /* edit_buffer发生更改 */
        {
            if (g_console.history_flag)
            {
                g_console.history_flag = 0;
                goto RePrint;
            }
            if ((g_console.cursor == g_console.edit_len) && !g_console.delete_flag) /* 追加 */
            {
                PRINTF("%s", temp_buffer);
            }
            else    /* 插入或删除 */
            {
RePrint:
                g_console.delete_flag = 0;
                CURSOR_INVISIBLE();
                CLEAR_LINE();
                CONSOLE_TITLE_LINE(g_console.edit_buffer, g_console.cursor+1+strlen(CONSOLE_TITLE_TEXT));
                CURSOR_VISIBLE();
            }
            g_console.edit_buffer_changed_flag = 0;
        }
        if (g_console.edit_flag)    /* 提交输入 */
        {
            if (g_console.submit_len > 0) /* 提交长度大于0 */
            {
                memcpy(g_console.history_buffer, g_console.submit_buffer, g_console.submit_len + 1);
                g_console.history_len = g_console.submit_len;
            }

            /* 调用处理函数 */
            task_ret = task_handler(g_console.submit_buffer, g_console.submit_len);
            if (task_ret == -1)
            {
                PRINTF("command not found, type 'help' to get more info\r\n");
            }
            g_console.edit_flag = 0;
            CLEAR_BUFFER(g_console.edit_buffer);
            g_console.edit_len = 0;
            CLEAR_BUFFER(g_console.temp_edit_buffer);
            g_console.temp_edit_len = 0;
            CONSOLE_TITLE();
        }
    }
}

/*** 
 * @brief 串口中断函数，函数名是由宏定义替换，保证可移植性
 * @return [void]
 */
void CONSOLE_UART_IRQ_HANDLER(void)
{
    char rc;

    if (get_console_uart_it_flag(CONSOLE_IT_RXEN) != 0)
    {
        rc = get_console_uart_data();
        g_console.edit_frame[g_console.edit_frame_len++] = rc;
        g_console.edit_frame[g_console.edit_frame_len] = '\0';
    }

    if (get_console_uart_it_flag(CONSOLE_IT_IDLE) != 0) /* idle中断触发，完成一帧数据帧接收 */
    {
        g_console.edit_frame_flag = 1;
        
        clear_console_uart_idel_it_flag();
    }
}

/*** 
 * @brief 注册task
 * @param task [Task_t*]    
 * @return [void]
 */
void console_task_register(Task_t* task)
{
    g_console_task_list->add2list(&(g_console_task_list->list), task, sizeof(Task_t), task->task_name, strlen(task->task_name));
}
