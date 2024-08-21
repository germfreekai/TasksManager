/*
 * Tasks read functions implementation file
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "../tm_headers_h/tm_tasks_read.h"
#include "../tm_headers_h/tm_definitions.h"
#include "../tm_headers_h/tm_utils.h"

enum OPTIONS {
    ALL = 0,
    FATHER = 1,
    OTHER = 2
};
/*
 * List existing tasks at $TASKS_HOME
 *
 * Arguments:
 *  - list_option (char*) - father task name
 *  - status (int) - status to list
 * Returns:
 *  - ret (int) - success status
 *                0 - success
 *                1 - failure
 */
int list_tasks(char *list_option, int status)
{
    int ret;

    enum OPTIONS opt = (!strcmp(list_option, "f")) ? FATHER : \
                       (!strcmp(list_option, "a")) ? ALL : OTHER;

    switch (opt)
    {
        // Print ALL
        case 0:
            ret = _display_tasks(NULL, 0, status);
            goto exit_func;
            break;
        // Print FATHER
        case 1:
            ret = _display_tasks(NULL, 1, status);
            goto exit_func;
            break;
        // PRINT TASK_NAME
        case 2:
            ret = _display_tasks(list_option, 0, status);
            goto exit_func;
            break;
    }
 
    ret = 0;

exit_func:
    return ret;
exit_failure:
    ret = 1;
    goto exit_func;
}

/*
 * Describe given task
 * Prints description and status
 *
 * Arguments:
 *  - task (Task*) - task structure
 * Returns:
 *  - ret (int) - success status
 *                0 - success
 *                1 - failure
 */
int describe_tasks(Task *task)
{
    int ret;
    int subtask;
    subtask = 0;

    char *home_dir = get_home_dir();
    char *task_path = set_path_var();
    char *father_file = set_path_var();
    char *subtask_file = set_path_var();
    char *description_file = set_path_var();
    char *description_file_content = set_description_var();
    char *status_file = set_path_var();
    char *status_file_content = set_status_file_content_var();

    if (! strlen(task->father_task))
    {
        fprintf(stderr, "[x] Missing father task\n");
        goto exit_failure;
    }
    if (strlen(task->subtask))  // if we need to print just subtask info
        subtask = 1;

    strcpy(task_path, home_dir);
    strcat(task_path, task->father_task);

    strcpy(father_file, task_path);
    strcat(father_file, "/.father");

    if (file_exists(father_file))
    {
        fprintf(stderr, "[x] Task %s is not a father task\n", task->father_task);
        goto exit_failure;
    }

    if (subtask)
    {
        strcat(task_path, "/");
        strcat(task_path, task->subtask);

        strcpy(subtask_file, task_path);
        strcat(subtask_file, "/.subtask");
        if (file_exists(subtask_file))
        {
            fprintf(stderr, "[x] Task %s is not a subtask\n", task->subtask);
            goto exit_failure;
        }
    }

    strcpy(description_file, task_path);
    strcat(description_file, "/description");

    strcpy(status_file, task_path);
    strcat(status_file, "/status");

    if (file_exists(description_file))
    {
        if (subtask)
            fprintf(stderr, "[x] Task %s, subtask %s is missing description file\n", task->father_task, task->subtask);
        else
            fprintf(stderr, "[x] Task %s is missing description file\n", task->father_task);
        goto exit_failure;
    }
    if (file_exists(status_file))
    {
        if (subtask)
            fprintf(stderr, "[x] Task %s, subtask %s is missing status file\n", task->father_task, task->subtask);
        else
            fprintf(stderr, "[x] Task %s is missing status file\n", task->father_task);
        goto exit_failure;
    }

    read_file(description_file, description_file_content);
    read_file(status_file, status_file_content);
    if (subtask)
        fprintf(stdout, "[+] Father task: %s -> Subtask: %s\n", task->father_task, task->subtask);
    else
        fprintf(stdout, "[+] Father task: %s\n", task->father_task);
    char last_char = status_file_content[strlen(status_file_content)];
    fprintf(stdout, "[+] STATUS: %s\n", status_file_content);
    if (last_char == '\0' || last_char == 10)
        fprintf(stdout, "[+] DESCRIPTION: %s\n", description_file_content);
    else
        fprintf(stdout, "[+] DESCRIPTION: %s\n", description_file_content);


    if (! subtask)
    {
        fprintf(stdout, "STRUCTURE:\n");
        ret = _display_tasks(task->father_task, 0, -1);  // default dipslay tasks = all = -2

        if (ret)
            goto exit_failure;
    }

    ret = 0;

exit_func:
    free(home_dir);
    free(task_path);
    free(description_file);
    free(father_file);
    free(subtask_file);
    free(description_file_content);
    free(status_file);
    free(status_file_content);

    return ret;

exit_failure:
    ret = 1;
    goto exit_func;
}

/*
 * Get number of subtasks
 * File should already been verified to exist
 *
 * Arguments:
 *  - subtasks_file_path (char*) - absolute path to subtasks file
 *  - dst (char*) - var to write into
 *
 * Returns:
 *  - subtasks_n (char*) - number of current subtasks
 */

void *get_subtasks_number(char *subtasks_file_path, char *dst)
{
    // char *subtasks_n = set_subtasks_number_var();
    if (read_file(subtasks_file_path, dst))
        fprintf(stderr, "[x] Could not read file .subtask\n");

    return NULL;
}

/*
 * Display all existing tasks
 * 
 * Arguments:
 *  - list_option (char*) - father task name
 *  - flag_subtasks (int) - display subtasks
 *                          0 - display
 *                          1 - do not display
 *  - status (int) - status tasks to display
 *                   if -1 given, print all
 * Returns:
 *  - ret (int) - success status
 *                0 - success
 *                1 - failure
 */
int _display_tasks(char *list_option, int flag_subtasks, int status)
{
    // Here we will just manage father tasks
    int ret = 0;

    // Path vars
    char *home_dir = get_home_dir();
    char *father_path = set_path_var();
    strcpy(father_path, home_dir);

    // General file
    char *file = set_path_var();

    char *list_option_s = NULL;

    // Print just desired option, avoid iterating if possible
    if (list_option)
    {
        strcat(father_path, list_option);

        strcpy(file, father_path);
        strcat(file, "/.father");

        // make sure dir exists and that it is a father task
        if (! dir_exists(father_path) && ! file_exists(file))
        {
            ret = _print_father_task(list_option, father_path, flag_subtasks, status);
            goto exit_func;
        }
        else
        {
            fprintf(stderr, "[x] Father task %s does not exist.", list_option);
            goto exit_failure;
        }
    }

    // Iterate and print all or desired status

    // Dir struct
    DIR *dir;
    struct dirent *drnt;

    if (! (dir = opendir(home_dir)))
    {
        fprintf(stderr, "[X] Failed to open home dir.\n");
        goto exit_failure;
    }

    int do_once = 1;
    while ( (drnt = readdir(dir)) != NULL)
    {
        // skip . and ..
        if (! strcmp(drnt->d_name, ".") || ! strcmp(drnt->d_name, ".."))
            continue;

        // When we find a dir
        if (drnt->d_type == DT_DIR)
        {
            strcpy(father_path, home_dir);
            strcat(father_path, drnt->d_name);

            strcpy(file, father_path);
            strcat(file, "/.father");

            if (! file_exists(file))
            {
                if (! list_option && do_once)
                {
                    list_option_s = set_task_name_var();
                    do_once = 0;
                }
                //else
                //    list_option[0] = '\0';
                strcpy(list_option_s, drnt->d_name);
                if (! _print_father_task(list_option_s, father_path, flag_subtasks, status))
                    continue;
                else
                    goto exit_failure;
            }
            father_path[0] = '\0';
            file[0] = '\0';
        }
    }

    if (dir)
        closedir(dir);

    ret = 0;

exit_func:
    // Paths vars
    free(home_dir);
    free(father_path);
    free(file);

    if (list_option_s)
        free(list_option_s);

    return ret;
exit_failure:
    ret = 1;
    goto exit_func;
}

/*
 * _display_all_tasks auxiliary function
 * to print all subtaks on a father task
 * Here we must already know if it is a father task
 * 
 * Arguments:
 *  - list_option (char*) - father task to print
 *  - father_path (char*) - path to a father task
 *  - flag_subtasks (int) - print subtasks
 *                          0 - display
 *                          1 - do not display
 *  - status (int) - status to print
 *                   -1 - print all
 * Returns:
 *  - ret (int) - success status
 *                0 - success
 *                1 - failure
 */
int _print_father_task(char *list_option, char *father_path, int flag_subtasks, int status)
{
    // fprintf(stdout, "++ _print_father_task ++");
    // fprintf(stdout, "list opt : %s | father path : %s | flag subtasks : %d | status : %d\n", list_option, father_path, flag_subtasks, status);
    int ret = 0;

    // General file
    char *file = set_path_var();

    // Status vars
    char *current_status = set_status_file_content_var();
    char *subtasks_s = set_subtasks_number_var();
    int subtasks_n;

    // Subtasks string var
    char *subtasks_to_print = set_description_var();

    // Get subtasks info
    strcpy(file, father_path);
    strcat(file, "/.subtasks_n");
    get_subtasks_number(file, subtasks_s);
    subtasks_n = atoi(subtasks_s);

    file[0] = '\0';

    if (subtasks_n != 0 && ! flag_subtasks)
    {
        if (_get_subtasks_to_print(father_path, status, subtasks_to_print))
            goto exit_failure;
    }

    // fprintf(stdout, "subtasks to print %s\n", subtasks_to_print);

    // Get father task status
    strcpy(file, father_path);
    strcat(file, "/status");
    if (! read_file(file, current_status))
    {
        //fprintf(stdout, "previous to compare\n");
        //fprintf(stdout, "status : %d | current status : %s | status_str : %s | subtasks to print len : %ld | flag subtasks : %d\n", status, current_status, STATUS_STR(status), strlen(subtasks_to_print), flag_subtasks);
        if (status == -1 || ! strcmp(current_status, STATUS_STR(status)) || (strlen(subtasks_to_print) > 0 && ! flag_subtasks))
        {
            fprintf(stdout, "+ %s -> %s\n", list_option, current_status);
            if (subtasks_to_print)
                fprintf(stdout, "%s", subtasks_to_print);
        }
    }

exit_func:
    // General file
    free(file);

    // Status vars
    free(current_status);
    free(subtasks_s);

    // Subtasks string var
    free(subtasks_to_print);

    return ret;
exit_failure:
    ret = 1;
    goto exit_func;
}

/*
 * Get string of subtasks to print
 *
 * Arguments:
 *  - father_path (char*) - absolute path to father task
 *  - status (int) - status to print
 *                   -1 print all
 *  - dst (char*) - var to write into
 * Returns:
 *  - tet (int) - success status
 *                0 - success
 *                1 - failure
 */
int _get_subtasks_to_print(char *father_path, int status, char *dst)
{
    int ret = 0;

    // Subtask path
    char *subtask_path = set_path_var();

    // General file
    char *file = set_path_var();

    // Status vars
    char *current_status = set_status_file_content_var();
    char *subtasks_s = set_subtasks_number_var();
    int subtasks_n;

    // Format string
    char *formatted_string = set_description_var();

    DIR *dir;
    struct dirent *drnt;

    if (! (dir = opendir(father_path)))
    {
        fprintf(stderr, "[x] Failed to open father task\n");
        goto exit_failure;
    }

    while ( (drnt = readdir(dir)) != NULL)
    {
        // skip . and ..
        if (! strcmp(drnt->d_name, ".") || ! strcmp(drnt->d_name, ".."))
            continue;

        // Found a dir
        if (drnt->d_type == DT_DIR)
        {
            if (strlen(file) > 0)
                file[0] = '\0';

            // We need to make sure we are at a subtask
            strcpy(subtask_path, father_path);
            strcat(subtask_path, "/");
            strcat(subtask_path, drnt->d_name);

            strcpy(file, subtask_path);
            strcat(file, "/.subtask");
            if (file_exists(file))
                continue;

            file[0] = '\0';

            // Now we need to know if the subtask status is wanted
            strcpy(file, subtask_path);
            strcat(file, "/status");
            if (! file_exists(file))
            {
                if (strlen(dst) > 0)
                    strcat(dst, "\n");
                read_file(file, current_status);
                if (status == -1 | ! strcmp(current_status, STATUS_STR(status)))
                {
                    snprintf(formatted_string, MAX_TASK_DESCRIPTION, "  ├ %s - %s", drnt->d_name, current_status);
                    strcat(dst, formatted_string);
                }
                current_status[0] = '\0';
                file[0] = '\0';
            }
            else
                goto exit_failure;
        }
        else
            continue;
    }

    if (strlen(dst) > 0)
        strcat(dst, "\n");

    ret = 0;

exit_func:
    // Subtask path
    free(subtask_path);

    // General file
    free(file);

    // Status vars
    free(current_status);
    free(subtasks_s);

    // Formatted string
    free(formatted_string);

    if (dir)
        closedir(dir);

    return ret;
exit_failure:
    ret = 1;
    goto exit_func;
}