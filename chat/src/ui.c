#include <stdio.h>


#define _TERMINAL_HEIGHT 25
#define _TERMINAL_WIDTH 80 

char _NAME[25];

void clr() {
    printf("\x1b[H\x1b[2J");
}

void get_name() {
    printf("\x1b[H\x1b[2J");
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

void clear_msg_wnd() {
    for (int i = 22; i < 25; i++) {
        printf("\x1b[%d;0f", i);
        printf("%*s", _TERMINAL_WIDTH, "");
    }

    printf("\x1b[23;3f");
}