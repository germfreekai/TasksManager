/*
 * Main file for TasksManager
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "tm_headers_h/tm_utils.h"
#include "tm_headers_h/tm_setup.h"
#include "tm_headers_h/tm_definitions.h"
#include "tm_headers_h/tm_tasks_write.h"
#include "tm_headers_h/tm_tasks_read.h"

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

    // Declarations
    Task *task = set_new_task_struct();
    char *tm_home_dir = NULL;
    char *list_option = NULL;

    while (1)
    {
        static struct option long_options[] = {
            {"help", no_argument, NULL, 'h'},
            {"setup", no_argument, NULL, 'i'},
            {"new-task", no_argument, &new_task_flag, 1},
            {"new-subtask", no_argument, &new_sub_task_flag, 1},
            {"describe-tasks", no_argument, &describe, 1},
            {"list-tasks", required_argument, NULL, 'l'},
            {"father-task", required_argument, NULL, 'f'},
            {"description", required_argument, NULL, 'd'},
            {"status", required_argument, NULL, 's'},
            {"subtask", required_argument, NULL, 'z'}
        };

        int option_index = 0;

        c = getopt_long (argc, argv, "hif:z:d:s:l:", long_options, &option_index);

        /* Detect the end of the options */
        if (c == -1)
            break;
        
        switch(c)
        {
            case 0:
                if (long_options[option_index].flag != 0)
                    break;
            case 'h':
                read_man_page("usage.txt");
                goto end_execution;
                break;
            case 'i':
                ret = setup_home_dir();
                goto end_execution;
                break;
            case 'f':
                if (strlen(task->father_task) != 0)
                    break;
                if (strlen(optarg) >= MAX_TASK_NAME)
                {
                    fprintf(stderr, "[x] Task name to long.");
                    goto exit_failure;
                } 
                strcat(task->father_task, optarg);
                fprintf(stdout, "[+] Task name: %s\n", task->father_task);
                break;
            case 'z':
                if (strlen(task->subtask) != 0)
                    break;
                if (strlen(optarg) >= MAX_TASK_NAME)
                {
                    fprintf(stderr, "[x] Task name to long.");
                    goto exit_failure;
                }
                strcat(task->subtask, optarg);
                fprintf(stdout, "[+] Subtask name: %s\n", task->subtask);
                break;
            case 'd':
                if (strlen(task->task_description) != 0)
                    break;
                if (strlen(optarg) >= MAX_TASK_DESCRIPTION)
                {
                    fprintf(stderr, "[x] Task description to long.");
                    goto exit_failure;
                }
                strcat(task->task_description, optarg);
                fprintf(stdout, "[+] Task description: %s\n", task->task_description);
                break;
            case 's':
                if (task->status != -1)
                    break;
                int stat = atoi(optarg);
                fprintf(stdout, "[+] Status: %s\n", STATUS_STR(stat));
                task->status = stat;
                break;
            case 'l':
                list_option = set_task_name_var();
                strcat(list_option, optarg);
                ret = list_tasks(list_option);
                free(list_option);
                goto end_execution;
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

    // Get tasks details
    if (describe)
    {
        fprintf(stdout, "[+] Describing tasks\n");
        ret = describe_tasks(task);

        if (ret)
            goto exit_failure;
        else
            goto exit_success;
    }

    // Create new father task
    if (new_task_flag)
    {
        new_sub_task_flag = 0;
        fprintf(stdout, "[+] Create new father task\n");
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

    if (new_sub_task_flag)
    {
        fprintf(stdout, "[+] Create new subtask\n");
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
    // TODO - info page for task creation operations and bounds
    ret = 1;
    goto end_execution;
exit_success:
    ret = 0;
    goto end_execution;
}
