#ifndef TM_DEFINITIONS_H
#define TM_DEFINITIONS_H

/*
 * Definitions header file
 *
 * Structs and constants used accross tool
 */

#define PATH_MAX 4096
#define MAX_TASK_NAME 128
#define MAX_TASK_DESCRIPTION 16384
#define MAX_STATUS_FILE 15
#define MAX_SUBTASKS 100

#define STATUS_STR(stat)                            \
    (0 == stat ? "READY"        :                   \
    (1 == stat ? "IN PROGRESS"  :                   \
    (2 == stat ? "DONE"         :                   \
    (3 == stat ? "ABANDONED"    :                   \
    (4 == stat ? "BLOCKED"      :                   \
    (5 == stat ? "UNDER REVIEW" :                   \
                 "NOT DEFINED"))))))

typedef struct
{
    char *father_task;
    char *subtask;
    char *task_description;
    int status;
}Task;

#endif // TM_DEFINITIONS_H