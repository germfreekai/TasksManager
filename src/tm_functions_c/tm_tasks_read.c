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

/*
 * Set current task struct dynamic memory allocation
 * 
 * Returns:
 *  - current_task (Current_Task*) - initialized struct
 */
Current_Task *_set_current_task_struct()
{
    Current_Task *current_task = (Current_Task*)malloc(sizeof(Current_Task));
    
    current_task->task_name = set_path_var();
    
    current_task->status = set_description_var();

    current_task->subtasks_num = 0;

    return current_task;
}

/*
 * Free allocated memory for Current_Task struct
 *
 * Arguments:
 *  - current_task (Current_Task*) - initialized struct
 */
void _free_current_task_struct(Current_Task *current_task)
{
    free(current_task->task_name);
    free(current_task->status);

    free(current_task);
}

/*
 * List existing tasks at $TASKS_HOME
 *
 * Arguments:
 *  - task (Task*) - pointer to task struct
 * Returns:
 *  - ret (int) - success status
 *                0 - success
 *                1 - failure
 */
int list_tasks(Task *task)
{
    int ret;
    char *home_dir = get_home_dir();  //../.tasks_manager/

    // Initiliaze helper struct
    Current_Task* current_task = _set_current_task_struct();

    char *father_file = NULL;

    if(dir_exists(home_dir))
    {
        fprintf(stderr, "[x] TasksManager home dir does not exits.\n");
        goto return_failure;
    }

    DIR *dir;
    struct dirent *drnt;

    if (!(dir = opendir(home_dir)))
    {
        fprintf(stderr, "[x] Failed to read TasksManager home dir.\n");
        goto return_failure;
    }

    father_file = set_path_var();

    // Read through tasks manager home dir 
    while ((drnt = readdir(dir)) != NULL)
    {
        // Skip father and current dir
        if (! strcmp(drnt->d_name, ".") || ! strcmp(drnt->d_name, ".."))
            continue;
        else if (drnt->d_type == DT_DIR)  // Found a dir
        {
            // Make sure it is a father dir
            strcpy(father_file, home_dir);
            strcat(father_file, drnt->d_name);
            strcat(father_file, "/.father");
            if (file_exists(father_file))
            {
                // It is not a father task
                continue;
            }
            fprintf(stdout, "[+] father task exist: %s\n", drnt->d_name);
            // Get task to investigate
            strcpy(current_task->task_name, home_dir);
            strcat(current_task->task_name, drnt->d_name);
            strcat(current_task->task_name, "/");  //../.tasks_manager/task1/

            fprintf(stdout, "[~] current task name before getting father task info: %s\n", current_task->task_name);
            // Populate struct
            if (_get_father_task_info(current_task))
            {
                fprintf(stderr, "[x] Failed to read father task: %s\n", current_task->task_name);
                goto return_failure;
            }

            strcpy(current_task->task_name, drnt->d_name);
            // Print info
            if (_print_father_task_info(current_task))
            {
                fprintf(stderr, "[x] Failed to print father task info: %s\n", current_task->task_name);
                goto return_failure;
            }

            // Return to 0 
            current_task->subtasks_num = 0;
            current_task->status[0] = '\0';
        }
        
    }

    ret = 0;

exit_func:
    free(home_dir);
    _free_current_task_struct(current_task);
    if(father_file)
        free(father_file);
    closedir(dir);
    return ret;
return_failure:
    ret = 1;
    goto exit_func;
}

/*
 * Get father task info helper func
 * Get n of subtasks and status file
 * 
 * Arguments:
 *  - current_task (Current_Task*) - initialized struct
 * Returns:
 *  - ret (int) - success status
 *                0 - success
 *                1 - failure
 * Note: task_name passed as: ../.tasks_manager/task1/
 */
int _get_father_task_info(Current_Task *current_task)
{
    DIR *dir;
    struct dirent *drnt;
    char *subtask_file = NULL;
    char *status_file = NULL;

    int ret;

    // Open father task
    if (!(dir = opendir(current_task->task_name)))
        goto exit_failure;
    
    fprintf(stdout, "current_task->father_name -> %s\n", current_task->task_name);
    fprintf(stdout, "current_task->subtask: %d\n", current_task->subtasks_num);

    subtask_file = set_path_var();
    status_file = set_path_var();

    while ((drnt = readdir(dir)) != NULL)
    {
        if (! strcmp(drnt->d_name, ".") || ! strcmp(drnt->d_name, ".."))
            continue;
        else if (drnt->d_type == DT_DIR)
        {
            // Make sure it is a subtask
            strcpy(subtask_file, current_task->task_name);
            strcat(subtask_file, drnt->d_name);
            strcat(subtask_file, "/.subtask");

            fprintf(stdout, "subtask before validation: %s\n", subtask_file);
            if (file_exists(subtask_file))
            {
                // It is not a subtask
                continue;
            }
            
            // At this point we know we are at a valid subtask dir
            current_task->subtasks_num++;
        }
        // Look for status file
        else
        {
            // Other than status
            if (strcmp(drnt->d_name, "status"))
                continue;

            strcat(status_file, current_task->task_name);
            strcat(status_file, "status");
            
            if (read_file(status_file, current_task->status))
            {
                fprintf(stderr, "[x] Failed to read status file for task: %s\n", current_task->task_name);
                goto exit_failure;
            }
        }
    }

    ret = 0;

exit_func:
    if (subtask_file)
        free(subtask_file);
    if (status_file)
        free(status_file);
    closedir(dir);
    return ret;
exit_failure:
    ret = 1;
    goto exit_func;
}

/*
 * Print father task info template
 *
 * Arguments:
 *  - current_task (Current_Task*) - initialized struct
 * Returns:
 *  - ret (int) - success status
 *                0 - success
 *                1 - failure
 */
int _print_father_task_info(Current_Task *current_task)
{
    fprintf(stdout, "[+] Task Name: %s\n", current_task->task_name);
    if (current_task->status)
        fprintf(stdout, "[+] Status: %s\n", current_task->status);
    fprintf(stdout, "[+] Subtasks: %d\n", current_task->subtasks_num);

    return 0;
}