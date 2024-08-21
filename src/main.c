/*
 * Main file for TasksManager
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "tm_headers_h/tm_utils.h"
#include "tm_headers_h/tm_definitions.h"
#include "tm_headers_h/tm_tasks_write.h"
#include "tm_headers_h/tm_tasks_read.h"

// TODO - info page for task creation operations and bounds

int main(int argc, char *argv[argc + 1])
{
    int ret = 0;

    if (argc == 1)
    {
        ret = read_man_page("usage.txt");
        end_tm_execution(ret);
    }

    int c;

    // Static flags
    static int new_task_flag;
    static int new_sub_task_flag;
    static int describe;
    static int u_status;
    static int remove;

    // Declarations
    Task *task = set_new_task_struct();
    char *tm_home_dir = NULL;
    char *list_option = NULL;
    int u_description = 0;
    int update_description = -1;
    int list_tasks_flag = 0;

    // Status var
    int status = -1;

    while (1)
    {
        static struct option long_options[] = {
            {"help", no_argument, NULL, 'h'},
            {"new-task", no_argument, &new_task_flag, 1},
            {"new-subtask", no_argument, &new_sub_task_flag, 1},
            {"describe-tasks", no_argument, &describe, 1},
            {"remove", no_argument, &remove, 1},
            {"u-status", no_argument, &u_status, 1},
            {"list-tasks", required_argument, NULL, 'l'},
            {"father-task", required_argument, NULL, 'f'},
            {"description", required_argument, NULL, 'd'},
            {"u-description", required_argument, NULL, 'u'},
            {"status", required_argument, NULL, 's'},
            {"subtask", required_argument, NULL, 'z'}
        };

        int option_index = 0;

        c = getopt_long (argc, argv, "hf:z:d:s:l:", long_options, &option_index);

        /* Detect the end of the options */
        if (c == -1)
            break;
        
        switch(c)
        {
            case 0:
                if (long_options[option_index].flag != 0)
                    break;
            // Show help
            case 'h':
                read_man_page("usage.txt");
                goto end_execution;
                break;
            // Father task name
            case 'f':
                if (strlen(task->father_task) != 0)
                    break;
                if (strlen(optarg) >= MAX_TASK_NAME)
                {
                    fprintf(stderr, "[x] Task name to long.");
                    goto exit_failure;
                } 
                strcat(task->father_task, optarg);
                break;
            // Subtask name
            case 'z':
                if (strlen(task->subtask) != 0)
                    break;
                if (strlen(optarg) >= MAX_TASK_NAME)
                {
                    fprintf(stderr, "[x] Task name to long.");
                    goto exit_failure;
                }
                strcat(task->subtask, optarg);
                break;
            // Description field
            case 'd':
                if (strlen(task->task_description) != 0)
                    break;
                if (strlen(optarg) >= MAX_TASK_DESCRIPTION)
                {
                    fprintf(stderr, "[x] Task description to long.");
                    goto exit_failure;
                }
                strcat(task->task_description, optarg);
                break;
            // Update description flag
            // 0 overwrite
            // 1 append
            case 'u':
                u_description = 1;
                update_description = atoi(optarg);
                break;
            // Status
            case 's':
                if (task->status != -1)
                    break;
                status = atoi(optarg);
                task->status = status;
                break;
            // List tasks
            // father - father tasks
            // all - all tasks
            // task_name - father task and subtasks
            case 'l':
                list_option = set_task_name_var();
                strcat(list_option, optarg);
                list_tasks_flag = 1;
                break;
            case '?':
                break;
            default:
                abort();
        }
    }

    // Look for task manager home dir
    tm_home_dir = get_home_dir();
    if (dir_exists(tm_home_dir))
    {
        fprintf(stderr, "[x] Missing TaskManager home dir.\n");
        goto exit_failure;
    }

    // List options
    if (list_tasks_flag)
    {
        ret = list_tasks(list_option, status);
        //if (list_option)
        free(list_option);
        goto end_execution;
    }

    // Remove a task
    if (remove)
    {
        ret = remove_dir(task);
        if (ret)
            goto exit_failure;
        else
            goto exit_success;
    }

    // Get tasks details
    if (describe)
    {
        ret = describe_tasks(task);

        if (ret)
            goto exit_failure;
        else
            goto exit_success;
    }

    // Update task (description)
    if (u_description || u_status)
    {
        // Make sure we have the values we want to update
        if (u_description && (! strlen(task->task_description) || update_description == -1))
        {
            fprintf(stderr, "[X] Missing description to update or update mode\n");
            goto exit_failure;
        }
        else if (u_status && task->status == -1)
        {
            fprintf(stderr, "[X] Missing new status value\n");
            goto exit_failure;
        }
        ret = update_task(task, update_description, u_description, u_status);
        if (ret)
            goto exit_failure;
        else
            goto exit_success;
    }

    // Create new father task
    if (new_task_flag)
    {
        new_sub_task_flag = 0;
        if (strlen(task->task_description) == 0)
        {
            fprintf(stderr, "[x] Father tasks must contain a description\n");
            goto exit_failure;
        }
        ret = create_father_task(task);
        if (ret)
            goto exit_failure;
        else
            goto exit_success;
    }

    // Create new subtask
    if (new_sub_task_flag)
    {
        if (strlen(task->task_description) == 0)
        {
            fprintf(stderr, "[x] Missing description\n");
            goto exit_failure;
        }
        ret = create_sub_task(task);
        if (ret)
            goto exit_failure;
        else
            goto exit_success;
    }

    goto exit_success;

end_execution:
    if (tm_home_dir)
        free(tm_home_dir);
    free_task_struct(task);
    end_tm_execution(ret);
exit_failure:
    ret = 1;
    goto end_execution;
exit_success:
    ret = 0;
    goto end_execution;
}
