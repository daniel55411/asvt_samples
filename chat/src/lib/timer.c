#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>


#define	_RTC_BASE_ADDR	0x01f00000
#define _INIT_HANDLERS_CAPACITY 10

typedef struct handler_t
{
    void (*handler)(time_t cur_time);
    time_t interval;
    time_t last_call;
} handler_t;

typedef unsigned time_t;

volatile time_t* _RTC_ADDR;
handler_t* _HANDLERS[_INIT_HANDLERS_CAPACITY];
char _HANDLERS_COUNT;

void init_timer(int memfd) {
    _RTC_ADDR = (volatile unsigned*)mmap(NULL, 4096, 
                PROT_READ | PROT_WRITE, MAP_SHARED, memfd, _RTC_BASE_ADDR);
    
    if (_RTC_ADDR == -1) {
        printf("ERROR: failed RTC mapping\n");
        exit(1);
    }

    memset(_HANDLERS, NULL, sizeof(handler_t) * _INIT_HANDLERS_CAPACITY);
}

void add_timer_handler(void (*handler)(time_t cur_time), char interval) {
    //TODO: add dynamic expanding of handlers' array
    handler_t h = {
        .handler = handler,
        .interval = interval,
    };

    _HANDLERS[_HANDLERS_COUNT] = &h;
    ++_HANDLERS_COUNT;
}

time_t get_current_time() {
    return _RTC_ADDR[5];
}

void run_timer() {
    for (;;) {
        time_t current_time = _RTC_ADDR[5];

        for (int i = 0; i < _HANDLERS_COUNT; ++i) {
            handler_t* handler = _HANDLERS[i];

            if (handler->last_call == 0 
                || current_time - handler->last_call >= handler->interval) {
                    handler->handler(current_time);
                    handler->last_call = current_time;
                }
        }
    }
}