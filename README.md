# TasksManager

TasksManager tool for Linux CLI.
This tool aims to provide an interactive way to handle tasks locally. Great coding practice and helps to keep some ideas on mind for the terminal lovers.

This tool is heavily inspired in [TaskWarrior](https://taskwarrior.org/), please give some love to them.

## Install
A ```install.sh``` script is provided, just execute it and it will create the necessary structure.
```bash
$ chmod +x install.sh
$ $ ./install.sh
[+] gcc: 11.4.0
[+] make: 4.3
[+] successfully created env dirs
[+] successfully copied info pages
[+] successfully copied funcs file
[+] custom function aliases added to .bashrc
[+] successfully created executable
[+] successfully installed
[+] successfully cleaned build
$ source ~/.bashrc
```
Following structure will be created
```bash
$ tree ~/.tasks_manager/
/home/renegarc/.tasks_manager/
├── custom_functions
│   └── tasksmanager_alias
└── man_pages
    └── usage.txt

2 directories, 2 files
```
- .tasks_manager: main directory, every file and task will be created here.
- custom_functions: custom functions directory -- tasksmanager_alias: cli function aliases for ease
- man_pages: info pages -- usage.txt: usage info txt file

Executable created at:
```bash
$ ls -l ~/.local/bin/ | grep tasksmanager
-rwxr-xr-x  1 renegarc renegarc 35808 Nov  5 16:47 tasksmanager
```
### Usage
Supported options:
```bash
$ tasksmanager -h
tasksmanager tool is a TODO list like bash utility.
Designed to work on bash, allows you to create tasks and subtasks
assigning them different status.

Usage:
tasksmanager [options]

Flags:

        -h | --help                             Show usage help
        --new-task                              Create new father task
        --new-subtask                           Create new subtask
        --describe-tasks                        Get tasks details
                                                Provide -f for father task description
                                                Provide -f and -z for subtask description
        -f | --father-task [task_name]          Task name [MAX 128]
        -z | --subtask [subtask_name]           Subtask name [MAX 128]
        -d | --description [description]        Task's description [MAX 16384]
        --u-description [option]                Update description file
                                                Provide -f and -z as required
                                                0 - overwrite
                                                1 - append
        -s | --status [status]                  Task status
                                                0 - READY (DEFAULT)
                                                1 - IN PROGRESS
                                                2 - DONE
                                                3 - ABANDONED
                                                4 - BLOCKED
                                                5 - UNDER REVIEW
        --u-status                              Update status file
                                                Provide -f and -z as required
        -l | --list-tasks [option] [status]     List tasks
                                                f - prints only father tasks
                                                a - prints father and subtasks
                                                Provide -s with a specific status to show
                                                [father_task_name] - prints only given father task
        --remove                                Delete [sub]task, will delete all contents inside, be cautious
                                                Provide -f and -z as required
```
For ease following aliases / functions are provided when installed with ```./install.sh```
- tm: tasksmanager short name
```bash
$ tm [OPTIONS]
```
- tm_describe: show task details
```bash
$ tm_describe [FATHER_TASK] (SUBTASK_NAME)
```
- tm_new-task: create new (sub)task
```bash
$ tm_new_task [FATHER_TASK_NAME] (SUBTASK_NAME)
```
- tm_remove_task: remove a (sub)task
```bash
$ tm_remove_task [FATHER_TASK_NAME] (SUBTASK_NAME)
```
- tm_update_d: update (sub)task's description
```
$ tm_update_d [FATHER_TASK_NAME] (SUBTASK_NAME)
```
- tm_update_s: update (sub)task's status
```
$ tm_update_s [FATHER_TASK_NAME] (SUBTASK_NAME)
```
## Structure
Tasks created have following structure
```bash
$ tree -a ~/.tasks_manager/task1
/home/renegarc/.tasks_manager/task1
├── .father
├── .subtasks_n
├── description
├── status
└── subtask1
    ├── .subtask
    ├── description
    └── status

1 directory, 7 files
```
- .father: hidden file that marks a task as a father task
- .subtasks_n: hidden file with N of subtasks
- description: description file
- status: status file
- subtask dirs: N subtasks dirs
- .subtask: hidden file that marks a task as a subtask
## Compilation
You can complie this project using ```gcc``` or ```make```

**REGULAR**
```bash
$ make
gcc -c src/main.c -o obj/main.o
gcc -c src/tm_functions_c/tm_tasks_read.c -o obj/tm_tasks_read.o
gcc -c src/tm_functions_c/tm_tasks_write.c -o obj/tm_tasks_write.o
gcc -c src/tm_functions_c/tm_utils.c -o obj/tm_utils.o
gcc  obj/main.o  obj/tm_tasks_read.o  obj/tm_tasks_write.o  obj/tm_utils.o -o bin/tasksmanager
```
**VALGRIND**
```bash
$ make valgrind
gcc -c src/main.c -o obj/main.o
gcc -c src/tm_functions_c/tm_tasks_read.c -o obj/tm_tasks_read.o
gcc -c src/tm_functions_c/tm_tasks_write.c -o obj/tm_tasks_write.o
gcc -c src/tm_functions_c/tm_utils.c -o obj/tm_utils.o
gcc  obj/main.o  obj/tm_tasks_read.o  obj/tm_tasks_write.o  obj/tm_utils.o -o bin/tasksmanager.out
```
**CLEAN**
```bash
$ make clean
rm -f obj/* obj_debug/* bin/*
```