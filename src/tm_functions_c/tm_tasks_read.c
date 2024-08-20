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
        case 0:
            ret = _display_tasks(NULL, 0, status);
            goto exit_func;
            break;
        case 1:
            ret = _display_tasks(NULL, 1, status);
            goto exit_func;
            break;
        default:
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

// TODO: rewrite - review _display_tasks and _print_subtaks

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
    int ret;
    
    //ShowTasks show_tasks = set_show_tasks();
    char *home_dir = get_home_dir();
    char *father_path = set_path_var();
    char *father_file = set_path_var();
    char *status_file = set_path_var();
    char *current_status = set_status_file_content_var();
    char *father_subtasks_path = set_path_var();
    char *subtasks_s = set_subtasks_number_var();
    int subtasks_n;
    char *display_subtasks = NULL;

    int compare_flag = 0;
    if (list_option)
        compare_flag = 1;

    DIR *dir;
    struct dirent *drnt;

    if (!(dir = opendir(home_dir)))
    {
        fprintf(stderr, "[x] Failed to open home dir\n");
        goto exit_failure;
    }

    while ((drnt = readdir(dir)) != NULL)
    {
        // skip '.' and '..'
        if (!strcmp(drnt->d_name, ".") || !strcmp(drnt->d_name, ".."))
            continue;
        // Found a directory
        else if (drnt->d_type == DT_DIR)
        {
            if (compare_flag)
                if (strcmp(drnt->d_name, list_option))
                    continue;

            strcpy(father_path, home_dir);  // like creating a copy for home_dir
            strcat(father_path, drnt->d_name);
            
            // how do we know it is a fater dir? Double check
            strcpy(father_file, father_path);
            strcat(father_file, "/.father");
            if (file_exists(father_file)) // not a father file? skip
                continue;

            // Get number of subtasks
            strcpy(father_subtasks_path, father_path);
            strcat(father_subtasks_path, "/.subtasks_n");
            get_subtasks_number(father_subtasks_path, subtasks_s);
            subtasks_n = atoi(subtasks_s);
            
            if (!flag_subtasks)
                display_subtasks = _print_sub_tasks(father_path);

            strcpy(status_file, father_path);
            strcat(status_file, "/status");
            if (! file_exists(status_file))
            {
                read_file(status_file, current_status);
                if (status == -1 || ! strcmp(current_status, STATUS_STR(status)) || (subtasks_n != 0 && strlen(display_subtasks) != 0))
                    fprintf(stdout, "+ %s - %s\n", drnt->d_name, current_status);
                current_status[0] = '\0';
            }
            else
                // we know it is a father task
                // status files are supposed to always exist, but
                // print alone in case user removed it
                fprintf(stdout, "+ %s\n", drnt->d_name);

            if (strlen(display_subtasks) != 0)
                fprintf(stdout, "%s", display_subtasks);
        }
        else
            continue;
    }

    closedir(dir);

    ret = 0;

exit_func:
    free(home_dir);
    free(father_path);
    free(father_file);
    free(current_status);
    free(status_file);
    free(father_subtasks_path);
    free(subtasks_s);
    if (strlen(display_subtasks) != 0)
    {
        fprintf(stdout, "FREEEEEEING\n\n");
        free(display_subtasks);
    }

    return ret;
exit_failure:
    ret = 1;
    goto exit_func;
}

/*
 * _display_all_tasks auxiliary function
 * to print all subtaks on a father task
 * 
 * Arguments:
 *  - father_path (char) - path to a father task
 * 
 * Returns:
 *  - display_subtasks (char*) - subtaks to display
 *                               NULL if none
 */
char *_print_sub_tasks(char *father_path)
{

    DIR *dir;
    struct dirent *drnt;
    char *subtask_path = set_path_var();
    char *subtask_file = set_path_var();
    char *status_file = set_path_var();
    char *current_status = set_task_name_var();

    char *display_subtasks = set_description_var();  // MAX_TASK_DESCRIPTION
    char *formatted_string = set_description_var();

    if (!(dir = opendir(father_path)))
    {
        fprintf(stderr, "[x] Failed to open father task: %s\n", father_path);
        goto exit_failure;
    }

    while ((drnt = readdir(dir)) != NULL)
    {
        if (!strcmp(drnt->d_name, ".") || !strcmp(drnt->d_name, ".."))
            continue;
        else if (drnt->d_type == DT_DIR)
        {
            strcpy(subtask_path, father_path);
            strcat(subtask_path, "/");
            strcat(subtask_path, drnt->d_name);

            // make sure it is a subtask
            strcpy(subtask_file, subtask_path);
            strcat(subtask_file, "/.subtask");
            if (file_exists(subtask_file))
                continue;

            strcpy(status_file, subtask_path);
            strcat(status_file, "/status");
            if (! file_exists(status_file))
            {
                read_file(status_file, current_status);
                snprintf(formatted_string, MAX_TASK_DESCRIPTION, "  ├ %s - %s\n", drnt->d_name, current_status);
                strcat(display_subtasks, formatted_string);
                current_status[0] = '\0';
            }
            else
            {
                // we know it is a father task
                snprintf(formatted_string, MAX_TASK_DESCRIPTION, "  ├ %s\n", drnt->d_name);
                strcat(display_subtasks, formatted_string);
            }
        }
        else
            continue;
    }

    closedir(dir);

exit_func:
    free(subtask_file);
    free(subtask_path);
    free(status_file);
    free(current_status);
    free(formatted_string);
    return display_subtasks;
exit_failure:
    display_subtasks = NULL;
    goto exit_func;
}