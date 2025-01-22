#include "console.h"

void add_default_task(void)
{
    Task_t task;
    task.task_func = console_task;
    strcpy(task.task_name, "console");
    strcpy(task.task_desc, "console task");
    g_console_task_list->add2list(g_console_task_list->list, &task, sizeof(Task_t), task.task_name, strlen(task.task_name));
}