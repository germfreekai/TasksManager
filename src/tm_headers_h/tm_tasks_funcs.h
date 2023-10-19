#ifndef TM_TASKS_FUNCS_H
#define TM_TASKS_FUNCS_H

/*
 * Tasks functions header file
 *
 * Functions to manage tasks
 */

#include "./tm_definitions.h"

int create_father_task(Task *task);
int create_sub_task(Task *task);
int _create_task(char *path);
int _set_task_type(char *path, int type);
int _write_status_file(char *path, int status);
int _write_description_file(char *path, char *description, int update);

#endif // TM_TASKS_FUNCS_H
