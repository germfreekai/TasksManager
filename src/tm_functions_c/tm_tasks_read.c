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
 *  - task (Task*) - pointer to task struct
 * Returns:
 *  - ret (int) - success status
 *                0 - success
 *                1 - failure
 */
int list_tasks(char *list_option)
{
    int ret;

    // Will a function per option
    // pro code readability

    enum OPTIONS opt = (!strcmp(list_option, "father")) ? FATHER : \
                       (!strcmp(list_option, "all")) ? ALL : OTHER;

    switch (opt)
    {
        case 0:
            ret = _display_tasks(NULL, 0);
            goto exit_func;
            break;
        case 1:
            ret = _display_tasks(NULL, 1);
            goto exit_func;
            break;
        default:
            ret = _display_tasks(list_option, 0);
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
 * Display all existing tasks
 * 
 * Arguments:
 *  - flag_subtasks (int) - display subtasks
 * Returns:
 *  - ret (int) - success status
 *                0 - success
 *                1 - failure
 */
int _display_tasks(char *list_option, int flag_subtasks)
{
    int ret;
    
    //ShowTasks show_tasks = set_show_tasks();
    char *home_dir = get_home_dir();
    char *father_path = set_path_var();
    char *father_file = set_path_var();

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
            
            // we know it is a father task
            fprintf(stdout, "+ %s\n", drnt->d_name);

            if (!flag_subtasks)
                _print_sub_tasks(father_path);
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
 *  - ret (int) - success status
 *                0 - success
 *                1 - failure
 */
int _print_sub_tasks(char *father_path)
{
    int ret;

    DIR *dir;
    struct dirent *drnt;
    char *subtask_path = set_path_var();
    char *subtask_file = set_path_var();

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

            fprintf(stdout, "  ├ %s\n", drnt->d_name);

        }
        else
            continue;
    }

    closedir(dir);

    ret = 0;

exit_func:
    free(subtask_file);
    free(subtask_path);
    return ret;
exit_failure:
    ret = 1;
    goto exit_func;
}