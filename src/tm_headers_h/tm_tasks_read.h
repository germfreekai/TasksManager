#ifndef TM_TASKS_READ_H
#define TM_TASKS_READ_H

/*
 * Tasks read functions header file
 *
 * Read current existing tasks
 */

#include "./tm_definitions.h"

int list_tasks(char *list_option);

int _display_tasks(char *list_option, int flag_subtasks);
int _print_sub_tasks(char *father_path);

#endif // TM_TASKS_READ_H