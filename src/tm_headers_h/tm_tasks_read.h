#ifndef TM_TASKS_READ_H
#define TM_TASKS_READ_H

/*
 * Tasks read functions header file
 *
 * Read current existing tasks
 */

#include "./tm_definitions.h"

/*
 * Helper struct to print current tasks
 */
typedef struct
{
    char *task_name;
    int subtasks_num;
    char *status;
}Current_Task;

Current_Task *_set_current_task_struct();
void _free_current_task_struct(Current_Task *current_task);

int list_tasks(Task *task);
int _get_father_task_info(Current_Task *current_task);
int _print_father_task_info(Current_Task *current_task);

#endif // TM_TASKS_READ_H