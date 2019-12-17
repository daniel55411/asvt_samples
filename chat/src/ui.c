#include <stdio.h>


#define _TERMINAL_HEIGHT 25
#define _TERMINAL_WIDTH 80 

char _NAME[25];
int _LAST_FILLED_LINE;

void clr() {
    printf("\x1b[H\x1b[2J");
}

void input_name() {
    clr();
    printf("\x1b[%d;%df", _TERMINAL_HEIGHT / 2, _TERMINAL_WIDTH / 2 - 20);
    printf("Введите логин: ");
    scanf("%s", _NAME);
    fflush(0);
}

void draw_interface() {
    clr();
    printf("\x1b[%dE", _TERMINAL_HEIGHT - 5);
    printf("\x1b[47m");
    printf("%*s", _TERMINAL_WIDTH, "");

    for (int i = 60; i >= 0; i--) {
        printf("\x1b[%dG", _TERMINAL_WIDTH - 20);
        printf(" ");
        printf("\x1b[1F");
    }

    printf("\x1b[16E");
    printf("\x1b[%dG", _TERMINAL_WIDTH - 20);
    printf("%*s", 21, "");

    printf("\x1b[49m");
    printf("\x1b[2E");
    printf("\x1b[%dG", _TERMINAL_WIDTH - 17);
    printf("%s", _NAME);

    printf("\x1b[23,3f");
}

void clear_message_windows() {
    for (int i = 22; i < 25; i++) {
        printf("\x1b[%d;0f", i);
        printf("%*s", _TERMINAL_WIDTH, "");
    }

    printf("\x1b[23;3f");
}

void save_cursor_pos() {
    printf("\x1b[s");
}

void restore_cursor_pos() {
    printf("\x1b[u");
}

void set_cursor_in_line(int line) {
    printf("\x1b[%d;2f", line);
}

void set_cursor_in_name(int uart_number) {
    printf("\x1b[%d;64f", uart_number + 2);
}

void write_message_to_window(client_t* client, char* message) {
    //TODO: add size to insert null-char in ends of message and client name
    if (client->name == NULL) {
        set_unknown_name(client);
    }
    ++_LAST_FILLED_LINE;

    save_cursor_pos();
    set_cursor_in_line(_LAST_FILLED_LINE);
    printf("%s: %s", client->name, message);
    restore_cursor_pos();
}

void write_client_name(client_t* client, int uart_number) {
    save_cursor_pos();
    set_cursor_in_name(uart_number);
    printf("%s", client->name);
    restore_cursor_pos();
}

void clear_client_name(int uart_number) {
    save_cursor_pos();
    set_cursor_in_name(uart_number);
    printf("%*s", 19, "");
    restore_cursor_pos();
}

const char* get_name() {
    return _NAME;
}