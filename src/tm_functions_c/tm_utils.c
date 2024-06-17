/*
 * Utils functions implementation file
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include "../tm_headers_h/tm_utils.h"
#include "../tm_headers_h/tm_definitions.h"

/*
 * Show usage message
 * 
 * Arguments:
 *  - man_page (char*) - man page to read
 * Returns:
 *  - ret (int) - success status
 *      0 - success
 *      1 - failure
 */
int read_man_page(char *man_page)
{
    FILE *fptr;
    int ret;

    char *path = set_path_var();

    strcat(path, _get_users_home());
    strcat(path, "/.tasks_manager_info_pages/");
    strcat(path, man_page);

    if ( !(fptr = fopen(path, "r")) )
    {
        fprintf(stderr, "[x] Failed to read man page: %s\n", man_page);
        free(path);
        ret = 1;
    }
    else
    {
        char ch = fgetc(fptr);
        
        while (ch != EOF)
        {
            fprintf(stdout, "%c", ch);
            ch = fgetc(fptr);
        }

        free(path);
        fclose(fptr);

        ret = 0;
    }

    return ret;
}

/*
 * End tasksmanager execution
 *
 * Arguments:
 *  - exit_code (int) - exit status:
 *  0 - success
 *  1 - failure
 */
void end_tm_execution(int exit_code)
{
    (! exit_code) ? exit(EXIT_SUCCESS) : exit(EXIT_FAILURE);
}

/*
 * Set memory for a path variable
 * 
 * Returns:
 *  - path (char*) - dynamic memory var [PATH_MAX]
 *                   Null terminated at index 0
 */
char *set_path_var()
{
    char *path = (char*)calloc(PATH_MAX, sizeof(char));
    path[0] = '\0';

    return path;
}

/*
 * Set memory for a description type variable
 *
 * Returns:
 *  - var (char*) - dynamic memory var [MAX_TASK_DESCRIPTION]
 *                  Null terminated at index 0
 */
char *set_description_var()
{
    char *var = (char*)calloc(MAX_TASK_DESCRIPTION, sizeof(char));
    var[0] = '\0';

    return var;
}

/* Set memory for a task name variable
 *
 * Returns:
 *  task_name (char*) - dynamic memory task_name [MAX_TASK_NAME]
 *                      Null terminated at index 0
 */
char *set_task_name_var()
{
    char *task_name = (char*)calloc(MAX_TASK_NAME, sizeof(char));
    task_name[0] = '\0';

    return task_name;
}

/*
 * Get current user home directory
 * 
 * Returns:
 *  - home_dir (char*) - tasks manager home dir
 *                       at users' $HOME dir with 
 *                       trailing slash '/'
 */
char *get_home_dir()
{
    char *tm_home_dir = set_path_var();
    strcat(tm_home_dir, _get_users_home());
    strcat(tm_home_dir, "/.tasks_manager/");
    
    return tm_home_dir;
}

/*
 * Auxiliar function to get user's home
 */
char *_get_users_home()
{
    return getenv("HOME");
}

/*
 * Verify if given path exists
 * 
 * Arguments:
 *  - path (char*) - absolute path to dir
 * Returns:
 *  - res (int) - dir status
 *      0 - exists
 *      1 - does not exist
 */
int dir_exists(char *path)
{
    int res = 1;
    DIR *dir;

    if ((dir = opendir(path)) != NULL)
    {
        res = 0;
        closedir(dir);
    }

    return res;
}

/*
 * Create directory
 * 
 * Arguments:
 *  - path (char*) - absolute path to dir
 * Returns:
 *  - res (int) - exit status
 *      0 - success
 *      1 - failure
 */
int create_dir(char *path)
{
    return mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO);
}

/*
 * Create tasks struct
 *
 * Returns:
 *  - task (Task*) - initialized task struct
 */
Task *set_new_task_struct()
{
    Task *task = (Task*)malloc(sizeof(Task));

    task->father_task = set_task_name_var();

    task->subtask = set_task_name_var();

    task->task_description = set_description_var();

    task->status = -1;

    return task;
}

/*
 * Free task struct
 *
 * Arguments:
 *  - task (Task*) - initialized task struct
 */
void free_task_struct(Task *task)
{
    free(task->father_task);
    free(task->subtask);
    free(task->task_description);

    free(task);
}

/*
 * Verify if a given file path exists
 *
 * Arguments:
 *  - path (char*) - absolute path to file
 * Returns:
 *  - ret (int) - Success status
 *      0 - exists
 *      1 - does not exist
 */
int file_exists(char *path)
{
    return access(path, F_OK);
}
/*
 * Create file
 *
 * Arguments:
 *  - path (char*) - abosulte path to file
 * Returns:
 *  - ret (int) - Success status
 *      0 - success
 *      1 - failure
 */
int create_file(char *path)
{
    FILE *fptr;
    int ret = 0;

    if (! (fptr = fopen(path, "w")))
        ret = 1;
    else
        fclose(fptr);

    return ret;
}

/*
 * Write file with given content
 * File supposed to exist at this point
 * 
 * Arguments:
 *  - path (char*) - absolute path to file
 *  - content (char*) - file's content
 *  - update (int) - Either overwrite or append to description file
 *      0 - overwrite
 *      1 - append
 * Retunrs
 *  - ret (int) - Success status
 *      0 - success
 *      1 - failure
 */
int write_file(char *path, char *content, int update)
{
    // TODO: verify if write fron scratch or append
    //       for append, verify total size against MAX_TASK_DESCRIPTION
    int ret = 0;
    FILE *fptr;

    if (! file_exists(path) && ! update)
    {
        if (! (fptr = fopen(path, "w")))
            ret = 1;
        else
        {
            fprintf(fptr, "%s", content);
            fclose(fptr);
        }
    }
    else if (! file_exists(path) && update)
    {
        if (! (fptr = fopen(path, "a")))
            ret = 1;
        else
        {
            fprintf(fptr, "%s", content);
            fclose(fptr);
        }
    }
    else
        ret = 1;

    return ret;
}

/*
 * Remove file
 *
 * Arguments:
 *  - path (char*) - abosulute path to file
 * Returns:
 *  - ret (int) - Success status
 *      0 - success
 *      1 - failure
 */
int remove_file(char *path)
{
    return remove(path);
}

/*
 * Read file
 *
 * Arguments:
 *  - path (char*) - abosulte path to file
 *  - dst (char*) - dst char memory space
 * Returns:
 *  - ret (int) - success status
 *                0 - success
 *                1 - failure
 */
int read_file(char *path, char *dst)
{
    FILE *fptr;
    int ret;

    // read file
    if ( !(fptr = fopen(path, "r")) )
    {
        fprintf(stderr, "[x] Failed to read file: %s\n", path);
        goto exit_failure;
    }
    else
    {
        char ch = fgetc(fptr);

        while (ch != EOF)
        {
            strncat(dst, &ch, 1);
            ch = (char)fgetc(fptr);

        }

        fclose(fptr);

        ret = 0;
    }

exit_func:
    return ret;
exit_failure:
    ret = 1;
    goto exit_func;
}