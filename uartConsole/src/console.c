#include "hardware.h"
#include "console.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "link_list.h"

console_struct g_console;
link_list_manager *g_console_task_list;

static void init_console_struct(void)
{
    g_console.edit_frame_flag = 0;
    g_console.edit_frame_len = 0;
    g_console.edit_flag = 0;
    g_console.cursor = 0;
    g_console.relog_flag = 0;
    g_console.edit_len = 0;
    g_console.submit_len = 0;
    g_console.edit_buffer_changed_flag = 0;
    g_console.delete_flag = 0;
}

static void console_out(const char* __format, ...)
{
    CONSOLE_TITLE();
    va_list args;
    va_start(args, __format);
    vprintf(__format, args);
    va_end(args);
    fflush(stdout);
}

void init_hardware(void)
{
    init_systick();
    init_status_led();
    init_console_uart();
    enable_uart_interrupt();
}

void console_start(void)
{
    uint8_t temp_buffer[BUFFER_LEN] = { 0 };
    uint16_t temp_len = 0;

    init_console_struct();
    g_console_task_list = link_list_manager_get();

    console_out("");

    while (1)
    {
        if (g_console.edit_frame_flag)
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
                    memcpy(g_console.submit_buffer, g_console.edit_buffer, g_console.edit_len);
                    g_console.submit_buffer[g_console.edit_len] = '\0';
                    g_console.submit_len = g_console.edit_len;
                    g_console.edit_len = 0;
                    g_console.cursor = 0;
                    g_console.edit_flag = 1;
                    CONSOLE_NEW_LINE();
                    // CONSOLE_TITLE();
                }
            }

        }
        if (g_console.edit_buffer_changed_flag)
        {
            if ((g_console.cursor == g_console.edit_len) && !g_console.delete_flag) /* 追加 */
            {
                PRINTF("%s", temp_buffer);
            }
            else    /* 插入或删除 */
            {
                g_console.delete_flag = 0;
                CURSOR_INVISIBLE();
                CLEAR_LINE();
                CONSOLE_TITLE_LINE(g_console.edit_buffer, g_console.cursor+1+strlen(CONSOLE_TITLE_TEXT));
                CURSOR_VISIBLE();
            }
            g_console.edit_buffer_changed_flag = 0;
        }
        if (g_console.edit_flag)
        {

            PRINTF("submit: %s\r\n", g_console.submit_buffer);
            g_console.edit_flag = 0;
            g_console.edit_len = 0;
            CONSOLE_TITLE();
        }
    }
}

void CONSOLE_UART_IRQ_HANDLER(void)
{
    char rc;

    if (get_console_uart_it_flag(CONSOLE_IT_RXEN) != 0)
    {
        rc = get_console_uart_data();
        g_console.edit_frame[g_console.edit_frame_len++] = rc;
        g_console.edit_frame[g_console.edit_frame_len] = '\0';
    }

    if (get_console_uart_it_flag(CONSOLE_IT_IDLE) != 0)
    {
        g_console.edit_frame_flag = 1;
        
        clear_console_uart_idel_it_flag();
    }
}

// void register_task()
