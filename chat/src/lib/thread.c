#include <stdio.h>
#include <string.h>
#include <sched.h>
#include <signal.h>
#include <unistd.h>


#define _STACK_SIZE 4096

int new_thread(int (*routine)(void* args), void* args) {
    void** stack = (void*) malloc(_STACK_SIZE);

    return clone(routine, &stack, CLONE_THREAD, args);
}