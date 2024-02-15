/*
 * Tasks write functions implementation file
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../tm_headers_h/tm_tasks_write.h"
#include "../tm_headers_h/tm_definitions.h"
#include "../tm_headers_h/tm_utils.h"

/*
 * Create new father task
 *
 * Arguments:
 *  - task (Task*) - task structure
 * Returns:
 *  - ret (int) - success status
 *      0 - success
 *      1 - failure
 * 
 */
int create_father_task(Task *task)
{
    if (! strlen(task->father_task))
    {
        fprintf(stderr, "[x] Missing new father task name.\n");
        return 1;
    }

    // Staus: READY by default
    if (task->status == -1)
        task->status = 0;

    int ret;

    char *task_path = get_home_dir();
    strcat(task_path, task->father_task);

    if (_create_task(task_path))
    {
        fprintf(stderr, "[x] Failed to create father task: %s\n", task->father_task);
        goto return_failure;
    }

    // Create .father file
    if(_set_task_type(task_path, 0))
    {
        fprintf(stderr, "[x] Could not create father file for task: %s\n", task->father_task);
        goto return_failure;
    }

    // Create status file
    if (_write_status_file(task_path, task->status))
    {
        fprintf(stderr, "[x] Failed to write status file for task: %s\n", task->father_task);
        goto return_failure;
    }

    // Create description file if given
    if (_write_description_file(task_path, task->task_description, 0))
        goto return_failure;

    ret = 0;
exit_func:
    free(task_path);
    return ret;
return_failure:
    ret = 1;
    goto exit_func;
}

/*
 * Create new sub task for father task
 *
 * Arguments:
 *  - task (Task*) - task struct
 * Returns:
 *  - ret (int) - Success status
 *      0 - success
 *      1 - failure
 */
int create_sub_task(Task *task)
{
    if (! strlen(task->father_task) || ! strlen(task->subtask))
    {
        fprintf(stderr, "[x] Missing father or subtask name.\n");
        return 1;
    }

    // Staus: READY by default
    if (task->status == -1)
        task->status = 0;
    
    int ret;

    // Get father task path
    char *task_path = get_home_dir();

    // Verify father task path
    strcat(task_path, task->father_task);
    if (dir_exists(task_path))
    {
        fprintf(stderr, "[x] Father task does not exists: %s\n", task->father_task);
        goto return_failure;
    }

    // Create subtask dir
    strcat(task_path, "/");
    strcat(task_path, task->subtask); // /path/to/father/subtask

    if (_create_task(task_path))
    {
        fprintf(stderr, "[x] Failed to create subtask: %s\n", task->father_task);
        goto return_failure;
    }

    if (_set_task_type(task_path, 1))
    {
        fprintf(stderr, "[x] Could not create subtask file: %s\n", task->subtask);
        goto return_failure;
    }

    // Create status file
    if (_write_status_file(task_path, task->status))
    {
        fprintf(stderr, "[x] Failed to write status file for subtask: %s\n", task->subtask);
        goto return_failure;
    }

    // Create description file if given
    if (strlen(task->task_description) != 0)
    {
        if (_write_description_file(task_path, task->task_description, 0))
        {
            fprintf(stderr, "[x] Failed to create description file for subtask: %s\n", task->subtask);
            goto return_failure;
        }
    }
    
    ret = 0;

exit_func:
    free(task_path);
    return ret;
return_failure:
    ret = 1;
    goto exit_func;
}

/*
 * Task creation auxiliary function
 * Verifies if given path exists, if it does
 * not exist, created it
 * 
 * Arguments:
 *  - path (char*) - absolute path to create [sub]task
 * Returns:
 *  - ret (int) - success status
 *      0 - success
 *      1 - failure
 * 
 */
int _create_task(char *path)
{
    if (! dir_exists(path))
    {
        fprintf(stdout, "[x] Father task already exists\n");
        return 1;
    }

    return create_dir(path);
}

/*
 * Set created task type
 *
 * Arguments:
 *  - task_path (char*) - abosulte path to task
 *  - type (int)        - task type
 *      0 - father
 *      1 - subtask
 * Returns:
 *  - ret (int) - Success status
 *      0 - success
 *      1 - failure
 * 
 */
int _set_task_type(char *task_path, int type)
{
    int ret = 0;
    char *type_file = set_path_var();
    strcat(type_file, task_path);
    strcat(type_file, type ? "/.subtask" : "/.father");

    if(create_file(type_file))
        ret = 1;

    free(type_file);
    return ret;
}
/*
 * Create status file
 * Write new one or overwrite
 * 
 * Argumnets:
 *  - task_path (char*) - absolute path to task
 *  - status (int)      - task status
 * Returns:
 *  - ret (int) - Success status
 *      0 - success
 *      1 - failure
 * 
 */
int _write_status_file(char *task_path, int status)
{
    int ret = 0;

    char *status_file = set_path_var();
    strcat(status_file, task_path);
    strcat(status_file, "/status");

    if (! file_exists(status_file))
        remove_file(status_file);
    
    if (create_file(status_file))
    {
        fprintf(stderr, "[x] Failed to create status file\n");
        ret = 1;
    }
    if (write_file(status_file, STATUS_STR(status), 0))
    {
        fprintf(stderr, "[x] Failed to write status file\n");
        ret = 1;
    }
    
    free(status_file);
    return ret;
}

/*
 * Create/write description file
 * Verifies if we want to overwrite or append more details
 * 
 * Arguments:
 *  - task_path (char*)   - absolute path to task
 *  - description (char*) - task description
 *  - update (int)        - Either overwrite or append to description file
 *      0 - overwrite
 *      1 - append
 * Returns:
 *  - ret (int) - Success status
 *      0 - success
 *      1 - failure
 *
 */
int _write_description_file(char *task_path, char *description, int update)
{
    int ret = 0;
    char *description_file = set_path_var();

    // If no description given - skip
    if (strlen(description) == 0)
        goto exit_func;

    // TODO - implement case when append is required
    strcat(description_file, task_path);
    strcat(description_file, "/description");

    // Add new line to description
    strcat(description, "\n");

    if (! update)
    {
        if (! file_exists(description_file))
            remove_file(description_file);

        if (create_file(description_file))
        {
            fprintf(stderr, "[x] Failed to create description file\n");
            goto return_failure;
        }
        if (write_file(description_file, description, update))
        {
            fprintf(stderr, "[x] Failed to write description file\n");
            goto return_failure;
        }
    }

exit_func:
    if (description_file)
        free(description_file);
    return ret;
return_failure:
    ret = 1;
    goto exit_func;
}