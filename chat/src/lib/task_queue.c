#include <stdint.h>
#include "ring_buffer.c"


#define _MAX_PRIORITY 10

typedef struct task_t {
    void(*exec) (void* args);
    void* args;
    task_t* next_task;
} task_t;

task_t* _TASK_QUEUE_HEAD[_MAX_PRIORITY];
task_t* _TASK_QUEUE_BASE[_MAX_PRIORITY];

void init_task_queue() {
    memset(_TASK_QUEUE_BASE, NULL, sizeof(task_t) * _MAX_PRIORITY);
    memset(_TASK_QUEUE_HEAD, NULL, sizeof(task_t) * _MAX_PRIORITY);
}

void add_task(void(*task)(void* args), void* args, int priority) {
    task_t task = {
        .exec = task,
        .args = args,
        .next_task = NULL,
    };
    task_t* task_ptr = &task;

    if (_TASK_QUEUE_BASE[priority] == NULL) {
        _TASK_QUEUE_HEAD[priority] = _TASK_QUEUE_BASE[priority] = task_ptr;
    } else {
        _TASK_QUEUE_HEAD[priority]->next_task = task_ptr;
        _TASK_QUEUE_HEAD[priority] = task_ptr;
    }
}

void run_task() {
    static round_robin = 0;

    while (round_robin < _MAX_PRIORITY && _TASK_QUEUE_BASE[round_robin] == NULL) {
        ++round_robin; 
    }

    if (round_robin == _MAX_PRIORITY) {
        round_robin = 0;
    } else {
        task_t* task = _TASK_QUEUE_BASE[round_robin];
        void *args = _TASK_QUEUE_BASE[round_robin]->args;

        task->exec(args);
        _TASK_QUEUE_BASE[round_robin] = task->next_task;
        round_robin = (round_robin + 1) % _MAX_PRIORITY;
        
        free(task);
        free(args);
    }
}