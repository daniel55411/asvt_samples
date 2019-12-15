#include "ring_buffer.c"


buffer_t* _TASK_QUEUE;

void add_task(void(*task)(void* args), void* args) {
    char* ptr_val;
    ptr_val = (char*)&task;
    
    for (int i = 0; i < sizeof(char*); ++i) {
        add_to_buffer(_TASK_QUEUE, ptr_val[i]);
    }

    ptr_val = (char*)&args;

    for (int i = 0; i < sizeof(char*); ++i) {
        add_to_buffer(_TASK_QUEUE, ptr_val[i]);
    }

    free(ptr_val);
}

void run_task() {
    char* ptr_val;
    void(*task)(void *args);
    void* args;

    extract_from_buffer(ptr_val, _TASK_QUEUE, sizeof(char*));
    task = *ptr_val;
    extract_from_buffer(ptr_val, _TASK_QUEUE, sizeof(char*));
    args = *ptr_val;

    task(args);
    
    free(ptr_val);
    free(task);
    free(args);
}