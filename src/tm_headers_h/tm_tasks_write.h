#ifndef TM_TASKS_WRITE_H
#define TM_TASKS_WRITE_H

/*
 * Tasks write functions header file
 *
 * Create/write tasks
 */

#include "./tm_definitions.h"

int create_father_task(Task *task);
int create_sub_task(Task *task);
int _create_task(char *path);
int _set_task_type(char *path, int type);
int _write_status_file(char *path, int status);
int _write_description_file(char *path, char *description, int update);
int _write_subtasks_count_file(char *task_path);
int update_task(Task *task, int update_description, int u_description, int u_status);

#endif // TM_TASKS_WRITE_H
