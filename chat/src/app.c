#include "lib/timer.c"
#include "lib/uart.c"
#include "lib/ring_buffer.c"
#include "lib/proto.c"
#include "lib/common.c"
#include "lib/thread.c"
#include "lib/client.c"
#include "lib/task_queue.c"
#include "ui.c"


#define _BUFFER_CAP 1024

buffer_t* _KB_RING_BUFFER;
client_t* _CLIENTS[2];
client_t* _ME;

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

void write_and_send_task(void* args) {
    char** parsed_args = (char**)args;
    char size = parsed_args[0];
    char* data_ptr = parsed_args[1];
    char* message = message_message(data_ptr, size);

    for (int i = 0; i < 2; ++i) {
        client_t* client = _CLIENTS[i];
        
        if (client->status == Online) {
            send_data_to_uart(i + 1, *message, size + 3);
        }
    }

    write_message_to_window(_ME, data_ptr);
}

void clear_client_name_task(void* args) {
    int uart_number;
    client_t* client;

    memcpy(&uart_number, args, sizeof(int));
    memcpy(client, args + sizeof(int), sizeof(client_t*));

    client->name = NULL;
    client->status = Offline;

    clear_client_name(uart_number);
}

void view_client_name_task(void* args) {
    int uart_number;
    client_t* client;

    memcpy(&uart_number, args, sizeof(int));
    memcpy(client, args + sizeof(int), sizeof(client_t*));

    write_client_name(client, uart_number);
}

void view_client_message_task(void* args) {
    //TODO: malloc for client?
    client_t* client;
    char* data;

    memcpy(data, args, sizeof(char*));
    memcpy(client, args + sizeof(char*), sizeof(client_t*));
    
    write_message_to_window(client, data);
}

void send_who_task(void* args) {
    int uart_number = *((int*)args);
    char* message = who_message();

    send_data_to_uart(uart_number, message, 3);
}

void send_handshake_task(void *args) {
    int uart_number = *((int*)args);
    const char* name = get_name();

    send_data_to_uart(uart_number, name, strlen(name));
}

int kb_read_routine(void* args) {
    const int priority = 1;
    char c;
    int length = 0;

    for (;;) {
        c = getchar();

        if (c == 0x0a) {
            char* data = (char*)malloc(length);
            char** args = {&length, data};
            extract_from_buffer(data, _KB_RING_BUFFER, length);
            add_task(write_and_send_task, args, priority);
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
    const int priority = 2 + uart_number;

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
                client->status = Offline;

                void* args = malloc(sizeof(int) + sizeof(client_t));
                memcpy(args, &uart_number, sizeof(int));
                memcpy(args + sizeof(int), client, sizeof(client_t));

                add_task(clear_client_name_task, args, priority);
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
                client->name = data;

                void* args = malloc(sizeof(int) + sizeof(client_t));
                memcpy(args, &uart_number, sizeof(int));
                memcpy(args + sizeof(int), client, sizeof(client_t));

                add_task(view_client_name_task, args, priority);
            } else {
                void* args = malloc(sizeof(char*) + sizeof(client_t*));
                memcpy(args, data, sizeof(char*));
                memcpy(args + sizeof(char*), client, sizeof(client_t*));

                add_task(view_client_message_task, args, priority);
            }

            state = 0;
            update_client_activity(client);
        } else if (state == 1 && c == 'P') {
            state = 5;

            if (client->status == Offline) {
                add_task(send_who_task, &uart_number, priority);
            }
        } else if (state == 1 && c == 'W') {
            state = 5;

            add_task(send_handshake_task, &uart_number, priority);
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
    _ME = init_client();
    _ME->name = "me";

    int memfd = get_shared_memory_fd();

    init_uarts(memfd);
    init_timer(memfd);
    add_timer_handler(send_ping_handler, 2);
    add_timer_handler(check_clients_dead, 1);

    int args_1 = 1;
    int args_2 = 2;
    new_thread(timer_routine, NULL);
    new_thread(kb_read_routine, NULL);
    new_thread(uart_read_routine, &args_1);
    new_thread(uart_read_routine, &args_2);

    task_queue_routine(NULL);    
}