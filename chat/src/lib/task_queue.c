#include "ring_buffer.c"

typedef struct task_t {
    void(*exec) (void* args);
    void* args;
    task_t* next_task;
} task_t;

task_t* _TASK_QUEUE_HEAD;
task_t* _TASK_QUEUE_BASE;

void init_task_queue() {
    _TASK_QUEUE_HEAD = NULL;
    _TASK_QUEUE_BASE = NULL;
}

//TODO: make thread safe 
void add_task(void(*task)(void* args), void* args) {
    task_t task = {
        .exec = task,
        .args = args,
        .next_task = NULL,
    };
    task_t* task_ptr = &task;

    if (_TASK_QUEUE_BASE == NULL) {
        _TASK_QUEUE_HEAD = _TASK_QUEUE_BASE = task_ptr;
    } else {
        _TASK_QUEUE_HEAD->next_task = task_ptr;
        _TASK_QUEUE_HEAD = task_ptr;
    }
}

//TODO: make thread safe
void run_task() {
    if (_TASK_QUEUE_BASE != NULL) {
        _TASK_QUEUE_BASE->exec(_TASK_QUEUE_BASE->args);
        _TASK_QUEUE_BASE = _TASK_QUEUE_BASE->next_task;
    }
}