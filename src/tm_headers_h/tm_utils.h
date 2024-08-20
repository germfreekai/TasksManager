#ifndef TM_UTILS_H
#define TM_UTILS_H

/*
 * Utils functions header file
 * 
 * Functions present at this header file aim
 * for general usage.
 * 
 */

#include "./tm_definitions.h"

int read_man_page(char *man_page);

void end_tm_execution(int exit_code);

char *set_path_var();
char *set_description_var();
char *set_task_name_var();
char *set_status_file_content_var();
char *set_subtasks_number_var();
void *subtasks_int_to_string(int subtasks_n, char *dst);

char *get_home_dir();
char *_get_users_home();
int dir_exists(char *path);
int create_dir(char *path);
int remove_dir(Task *task);

Task *set_new_task_struct();
void free_task_struct(Task *task);

int file_exists(char *path);
int create_file(char *path);
int write_file(char *path, char *content, int update);
int remove_file(char *path);
int read_file(char *path, char *dst);

#endif // TM_UTILS_H
