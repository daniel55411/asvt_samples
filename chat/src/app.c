#include "lib/timer.c"
#include "lib/uart.c"
#include "lib/ring_buffer.c"
#include "lib/proto.c"
#include "lib/common.c"
#include "lib/thread.c"
#include "lib/client.c"
#include "lib/task_queue.c"


#define _BUFFER_CAP 1024

buffer_t* _KB_RING_BUFFER;
client_t* _CLIENTS[2];

void send_ping_handler(unsigned cur_time) {
    send_data_to_uart(1, ping_message(), 3);
}

void check_clients_dead(unsigned cur_time) {
    for (int i = 0; i < 2; ++i) {
        check_client_status(_CLIENTS[i], cur_time);
    }
}

int timer_routine(void* args) {
    run_timer();
}

int kb_read_routine(void* args) {
    char c;
    int length = 0;

    for (;;) {
        c = getchar();

        if (c == 0x0a) {
            char* data = (char*)malloc(length);
            extract_from_buffer(data, _KB_RING_BUFFER, length);
            //TODO: perform read buffer and send to online uarts
            //TODO: free data ptr
            length = 0;
            continue;
        }

        add_to_buffer(_KB_RING_BUFFER, c);
        ++length;
    }
}

int uart_read_routine(void* args) {
    int uart_number = ((int*)args)[0];
    int state = 0;
    char message_len = 0;
    client_t* client = _CLIENTS[uart_number - 1];
    char c;

    for (;;) {
        c = get_byte_from_uart(uart_number);

        if (state == 0 && c == '\\') {
            state = 1;
        } else if (state == 1 && (c == 'M' || c == 'H')){
            state = c * 1000;
        } else if (state > 1000 && state % 1000 == 0) {
            if (c == 0) {
                client->name = _DEFAULT_NAME;
                client->status = Online;
                // TODO: clear client name in UI
            }

            message_len = c;
            state += 1;
        } else if (state > 1000 && state % 1000 - 1 < message_len) {
            state += 1;
            add_to_buffer(client->buffer, c);
        } else if (state > 1000 && state % 1000 - 1 == message_len) {
            //TODO: free memory
            char* data = (char*)malloc(message_len);
            extract_from_buffer(data, client->buffer, message_len);

            if ((state / 1000) == 'H') {
                // TODO: show client name
            } else {
                // TODO: show client message
            }

            state = 0;
            update_client_activity(client);
        } else if (state == 1 && c == 'P') {
            state = 5;

            if (client->status == Offline) {
                // TODO: send who request
            }
        } else if (state == 1 && c == 'W') {
            state = 5;
            // TODO: send handshake request
        } else if (state == 5) {
            state = 0;
            update_client_activity(client);
        } else {
            state = 0;
        }
    }
}

int task_queue_routine(void* args) {
    init_task_queue();

    for (;;) {
        run_task();
    }
}

void run_app() {
    _KB_RING_BUFFER = init_ring_buffer(_BUFFER_CAP);
    _CLIENTS[0] = init_client();
    _CLIENTS[1] = init_client();

    int memfd = get_shared_memory_fd();

    init_uarts(memfd);
    init_timer(memfd);
    add_timer_handler(send_ping_handler, 2);
    add_timer_handler(check_clients_dead, 1);

    int args[1];
    new_thread(timer_routine, NULL);
    new_thread(kb_read_routine, NULL);
    args[0] = 1;
    new_thread(uart_read_routine, args);
    args[0] = 2;
    new_thread(uart_read_routine, args);

    task_queue_routine(NULL);    
}