/*
 * Setup functions implementation file
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "../tm_headers_h/tm_setup.h"
#include "../tm_headers_h/tm_utils.h"

/*
 * Create ~/.tasks_manager directory
 * 
 * Returns:
 *  - exit status:
 *      0 - success
 *      1 - failure
 */
int setup_home_dir()
{
    char *tm_home_dir = get_home_dir();

    int tm_home_dir_exists = dir_exists(tm_home_dir);

    if (! tm_home_dir_exists)
        goto exit_exists;
    
    if (create_dir(tm_home_dir))
        goto exit_failed;
    
    fprintf(stdout, "TasksManager home dir successfully created.\n");
    free(tm_home_dir);
    return EXIT_SUCCESS;

exit_exists:
    fprintf(stdout, "TasksManager home dir already exists.\n");
    free(tm_home_dir);
    return EXIT_SUCCESS;
exit_failed:
    fprintf(stderr, "TasksManager home dir failed to be created.\n");
    free(tm_home_dir);
    return EXIT_FAILURE;
}