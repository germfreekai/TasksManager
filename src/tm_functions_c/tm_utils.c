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

    char *path = get_home_dir();

    strcat(path, "/man_pages/");
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

/*
 * Set memory for the number of subtasks at
 * .subtask file
 * 
 * Returns:
 *  - subtasks_n (char*) - dynamic memory [4] - 3 digits and null char
 *                         Null terminated at index 0
 */
char *set_subtasks_number_var()
{
    char *subtasks_n = (char*)calloc(4, sizeof(char));
    subtasks_n[0] = '\0';

    return subtasks_n;
}

/*
 * Convert int to char* for subtasks n
 *
 * Argument:
 *  - subtasks_n (int) - number of subtaks found at .subtask
 *  - dst (char*) - var to write into
 * Returns:
 *  - subtasks_s (char*) - string of number of subtasks
 */
void *subtasks_int_to_string(int subtasks_n, char *dst)
{
    int max_subtasks = subtasks_n < 10 ? 1 : subtasks_n < 100 ? 10 : 100;

    char *digit_c = (char*)calloc(2, sizeof(char));
    digit_c[0] = '\0';

    while (max_subtasks > 0)
    {
        int digit_n = subtasks_n / max_subtasks;
        digit_c[0] = digit_n + '0'; // cast to char with + '0'
        strcat(dst, digit_c);

        subtasks_n = subtasks_n % max_subtasks;
        max_subtasks = max_subtasks / 10;
    }

    free(digit_c);

    return NULL;
}

/*
 * Set memory for the content of a status file
 *
 * Returns:
 *  - status (char*) - dynamic memory [MAX_STATUS_FILE]
 *                     Null terminated at index 0
 */
char *set_status_file_content_var()
{
    char *status = (char*)calloc(MAX_STATUS_FILE, sizeof(char));
    status[0] = '\0';

    return status;
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
    int ret = 0;
    FILE *fptr;

    // Overwrite
    if ((! file_exists(path) && ! update) || file_exists(path))
    {
        if (! (fptr = fopen(path, "w")))
            ret = 1;
        else
        {
            fprintf(fptr, "%s", content);
            fclose(fptr);
        }
    }
    // Append
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

/*
 * Delete directory / task
 * Will remove everything inside
 * Arguments:
 *  - task (Task*) - task struct
 * Return:
 *  - ret (int) - success status
 *                0 - success
 *                1 - failure
 */
int remove_dir(Task *task)
{
// TODO: add option to remove "/.subtasks" file at father
// TODO: this function is way to long and repetitive - shorthen it
// TODO: if dir removed is a subtasks, substract 1 from .subtasks
    int ret;

    DIR *dir;
    struct dirent *drnt;
    char *task_path = get_home_dir();
    char *task_file_type = set_path_var();
    char *subtask_path = set_path_var();
    char *subtask_files = set_path_var();

    strcat(task_path, task->father_task);
    strcpy(task_file_type, task_path);
    strcat(task_file_type, "/.father");

    // This is a safe lock for us to delete only TasksManager dirs
    if (dir_exists(task_path) || file_exists(task_file_type))
    {
        fprintf(stderr, "[x] Father task either does not exist or is not a father task... aborting\n");
        goto return_failure;
    }

    if (strlen(task->subtask) > 0)
    {
        strcat(task_path, "/");
        strcat(task_path, task->subtask);
        strcpy(task_file_type, task_path);
        strcat(task_file_type, "/.subtask");

        if (dir_exists(task_path) || file_exists(task_file_type))
        {
            fprintf(stderr, "[x] Subtask either does not exist or is not subtask... aborting\n");
            goto return_failure;
        }
    }

    // we are opening either father task or subtask
    if (! (dir = opendir(task_path)))
    {
        fprintf(stderr, "[x] Failed to open task: %s\n", task_path);
        goto return_failure;
    }

    while ((drnt = readdir(dir)) != NULL)
    {
        if (!strcmp(drnt->d_name, ".") || !strcmp(drnt->d_name, ".."))
            continue;
        // we can assume that is we find a dir, it is subtask and we are sitting
        // at the father task, so if we find a dir, it is a subtask
        else if (drnt->d_type == DT_DIR)
        {
            strcpy(subtask_path, task_path);
            strcat(subtask_path, "/");
            strcat(subtask_path, drnt->d_name);

            // only 3 files to delete [.subtask|.father / status / description]
            strcpy(subtask_files, subtask_path);
            strcat(subtask_files, "/.subtask");
            // Double check we are deleting a valid subtask
            if (file_exists(subtask_files))
            {
                fprintf(stderr, "[x] Directory not a valid subtask\n");
                goto return_failure;
            }
            if (remove_file(subtask_files))
            {
                fprintf(stderr, "[x] Failed to remove .subtask file\n");
                goto return_failure;
            }
            strcpy(subtask_files, subtask_path);
            strcat(subtask_files, "/status");
            if (! file_exists(subtask_files))
            {
                if (remove_file(subtask_files))
                {
                    fprintf(stderr, "[x] Failed to remove file staus\n");
                    goto return_failure;
                }
            }
            strcpy(subtask_files, subtask_path);
            strcat(subtask_files, "/description");
            if (! file_exists(subtask_files))
            {
                if (remove_file(subtask_files))
                {
                    fprintf(stderr, "[x] Failed to remove file description\n");
                    goto return_failure;
                }
            }
            // remove subtask itself
            rmdir(subtask_path);
        }
        else
            continue;
    }

    // Either if we delete subtasks or not, we need to delete current task path
    // as it could be a father or subtask
    strcpy(task_file_type, task_path);
    strcat(task_file_type, "/.subtask");
    if (! file_exists(task_file_type))
    {
        if (remove_file(task_file_type))
        {
            fprintf(stderr, "[x] Failed to remove .subtask file\n");
            goto return_failure;
        }
    }
    strcpy(task_file_type, task_path);
    strcat(task_file_type, "/.father");
    if (! file_exists(task_file_type))
    {
        if (remove_file(task_file_type))
        {
            fprintf(stderr, "[x] Failed to remove .father file\n");
            goto return_failure;
        }
    }
    strcpy(task_file_type, task_path);
    strcat(task_file_type, "/status");
    if (! file_exists(task_file_type))
    {
        if (remove_file(task_file_type))
        {
            fprintf(stderr, "[x] Failed to remove status file\n");
            goto return_failure;
        }
    }
    strcpy(task_file_type, task_path);
    strcat(task_file_type, "/description");
    if (! file_exists(task_file_type))
    {
        if (remove(task_file_type))
        {
            fprintf(stderr, "[x] Failed to remove description file\n");
            goto return_failure;
        }
    }

    ret = rmdir(task_path);
exit_func:
    free(task_path);
    free(task_file_type);
    free(subtask_files);
    free(subtask_path);
    if (dir != NULL)
        closedir(dir);
    return ret;
return_failure:
    ret = 1;
    goto exit_func;
}