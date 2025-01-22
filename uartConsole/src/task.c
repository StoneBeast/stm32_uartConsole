/*** 
 * @Author       : stoneBeast
 * @Date         : 2025-01-22 10:41:13
 * @Encoding     : UTF-8
 * @LastEditors  : stoneBeast
 * @LastEditTime : 2025-01-22 14:31:01
 * @Description  : 实现任务管理
 */

#include "console.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static void free_argv(int argc, char **argv);

static int help_task(int argc, char *argv[])
{
    PRINTF("help task\r\n");
    return 1;
}

static Task_t default_task[] = 
{
    {"help", "display help info", help_task},
    {"", "", NULL}
};

void add_default_task(void)
{
    uint16_t i = 0;

    while (strlen(default_task[i].task_name) != 0)
    {
        g_console_task_list->add2list(g_console_task_list->list, &default_task[i], sizeof(Task_t), default_task[i].task_name, strlen(default_task[i].task_name));
        i++;
    }
    
}

int task_handler(uint8_t *submit, uint16_t submit_len)
{
    int argc = 0;
    char **argv;
    int arg_start = -1;
    int arg_end = 0;
    int temp_argc = 0;
    Task_t *task;

    if (submit_len == 0)
        return 0;

    for (uint16_t i = 0; i < submit_len; i++)
    {
        if (argc == 0)
        {
            if (submit[i] != ' ')
                argc++;
        }
        else
        {
            if ((submit[i] == ' ') && i != (submit_len - 1) && (submit[i+1] != ' '))
                argc++;
        }
    }

    argv = (char **)malloc(argc * sizeof(char *));

    for (uint16_t i = 0; i < submit_len; i++)
    {
        if ((submit[i] != ' ') && ((i == 0) || (submit[i-1] == ' ')))
            arg_start = i;

        if ((arg_start != -1) 
            && ((submit[i] != ' ') 
                && ((submit[i+1] == ' ') || (i == (submit_len - 1)))))
        {
            arg_end = i;
            argv[temp_argc] = (char *)malloc(arg_end - arg_start + 1 + 1);
            memset(argv[temp_argc], 0, arg_end - arg_start + 1 + 1);

            memcpy(argv[temp_argc], submit + arg_start, arg_end - arg_start + 1);
            temp_argc++;
        }
    }

    task = g_console_task_list->find_by_id(g_console_task_list->list, argv[0]);
    if (task != NULL)
    {
        task->task_func(argc, argv);
        free_argv(argc, argv);
        return 1;
    }
    else
    {
        free_argv(argc, argv);
        return -1;
    }

}

static void free_argv(int argc, char** argv)
{
    for (int i = 0; i < argc; i++)
    {
        free(argv[i]);
    }

    free(argv);
}
