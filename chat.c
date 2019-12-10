#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>

#define	rtc_base_addr	0x01f00000

#define terminal_width  80
#define terminal_height 25

#define ccu_base_addr  0x01C20000
#define uart_base_addr 0x01C28000

#define uart1_offset 0x0400
#define uart1_bit    17
#define uart2_offset 0x0800
#define uart2_bit    18

int memfd;

const char* unknown = "unknown";
char name[15];
int name_length;

char write_buffer[160]; 
char write_ptr;

volatile unsigned *ccu_regs;
volatile unsigned *uart_regs;

int uarts_offsets[2] = {uart1_offset, uart2_offset};
int uarts_ptrs[2] = {0, 0};
int uarts_status[2] = {0, 0};
int uarts_states[2] = {0, 0};
char* uarts_buffers[2];
char* uarts_names[2];
char uarts_lens[2] = {0, 0};
unsigned uarts_timers[2] = {0, 0};

volatile unsigned* rtc_vaddr;
unsigned last_time;

int last_chat_line;

int getch();
int kbhit();

int init_memfd() {
    memfd = open("/dev/mem", O_RDWR | O_DSYNC);

	if (memfd == -1) {
		printf("Ошибка открытия файла /dev/mem\n");
		return 1;
	}

    return 0;
}

void init_timer() {
    rtc_vaddr = (volatile unsigned*)mmap(NULL, 4096, 
                PROT_READ | PROT_WRITE, MAP_SHARED, memfd, rtc_base_addr);
    
    last_time = rtc_vaddr[5];
}

void init_uarts() {
    ccu_regs = (volatile unsigned *)mmap(NULL, 4096, PROT_READ | PROT_WRITE,
	                                     MAP_SHARED, memfd, ccu_base_addr);

    uart_regs = (volatile unsigned *)mmap(NULL, 4096, PROT_READ | PROT_WRITE,
	                                     MAP_SHARED, memfd, uart_base_addr);

    ccu_regs[0x02D8/4] &= ~(1<<uart1_bit); 
	ccu_regs[0x006C/4] |= 1<<uart1_bit;    
	ccu_regs[0x02D8/4] |= 1<<uart1_bit; 

    ccu_regs[0x02D8/4] &= ~(1<<uart2_bit); 
	ccu_regs[0x006C/4] |= 1<<uart2_bit;    
	ccu_regs[0x02D8/4] |= 1<<uart2_bit; 

	uart_regs[(uart1_offset + 0x0C)/4] |= 0b10000011;
    uart_regs[(uart1_offset + 0x08)/4] |= 0b00000001;
	uart_regs[(uart1_offset + 0x00)/4] = 78;
	uart_regs[(uart1_offset + 0x04)/4] = 0;
	uart_regs[(uart1_offset + 0x0C)/4] &= ~0b10000000;

    uart_regs[(uart2_offset + 0x0C)/4] |= 0b10000011;
    uart_regs[(uart2_offset + 0x08)/4] |= 0b00000001;
	uart_regs[(uart2_offset + 0x00)/4] = 78;
	uart_regs[(uart2_offset + 0x04)/4] = 0;
	uart_regs[(uart2_offset + 0x0C)/4] &= ~0b10000000;
}

void draw_ui() {
    printf("\x1b[H\x1b[2J");
    printf("\x1b[%dE", terminal_height - 5);
    printf("\x1b[47m");
    printf("%*s", terminal_width, "");

    for (int i = 60; i >= 0; i--) {
        printf("\x1b[%dG", terminal_width - 20);
        printf(" ");
        printf("\x1b[1F");
    }

    printf("\x1b[16E");
    printf("\x1b[%dG", terminal_width - 20);
    printf("%*s", 21, "");

    printf("\x1b[49m");
    printf("\x1b[2E");
    printf("\x1b[%dG", terminal_width - 17);
    printf("%s", name);

    printf("\x1b[23,3f");
}

void get_name() {
    printf("\x1b[H\x1b[2J");
    printf("\x1b[%d;%df", terminal_height / 2, terminal_width / 2 - 20);
    printf("Введите логин: ");
    scanf("%s", name);
    fflush(0);
}

void clear_msg_wnd() {
    for (int i = 22; i < 25; i++) {
        printf("\x1b[%d;0f", i);
        printf("%*s", terminal_width, "");
    }

    printf("\x1b[23;3f");
}

void flush_me_msg() {
    if (last_chat_line < 59) {
        printf("\x1b[s");
        printf("\x1b[%d;2f", last_chat_line);
        write_buffer[write_ptr] = '\0';
        printf("me: %s", write_buffer);
        printf("\x1b[u");
        last_chat_line += 1;
    } else {
        //TODO: scrolling
        printf("\x1b[s");
        printf("\x1b[%d;2f", last_chat_line);
        write_buffer[write_ptr] = '\0';
        printf("me: %s", write_buffer);
        printf("\x1b[u");
    }
}


void send_raw_message(char* buffer, int length, int force) {
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < 2; j++) {
            if (force == 1 || uarts_status[j] == 1) {
                while ((uart_regs[(uarts_offsets[j] + 0x14)/4]  & (1 << 5)) == 0);

                uart_regs[(uarts_offsets[j] + 0x00)/4] = buffer[i];
            }
        }
    }
}

void ping() {
    if (rtc_vaddr[5] - last_time > 2) {
        send_raw_message("\\P0", 3, 1);
        last_time = rtc_vaddr[5];
    }
}

void who() {
    send_raw_message("\\W0", 3, 1);
}

void handshake() {
    char len = strlen(name);

    send_raw_message("\\H", 2, 0);
    send_raw_message(&len, 1, 0);
    send_raw_message(name, len, 0);
}

int send_message() {
    send_raw_message("\\M", 2, 0);
    send_raw_message(&write_ptr, 1, 0);
    send_raw_message(write_buffer, write_ptr, 0);

    flush_me_msg();
    write_ptr = 0;
    clear_msg_wnd();
}

void process_kb() {
    if (kbhit() > 0) {
        char c = getch();

        if (c == '\n' || write_ptr >= 160) {
            send_message();

            if (c != '\n') {
                write_buffer[write_ptr] = c;
                ++write_ptr;
                printf("%c", c);
            }
        } else {
            write_buffer[write_ptr] = c;
            ++write_ptr;
            printf("%c", c);
        }
    }
}

void update_uart_timer(int uart_number) {
    uarts_timers[uart_number] = rtc_vaddr[5];
    uarts_status[uart_number] = 1;
}

void print_uart_msg(int uart_number) {
    for (int i = 0; i < uarts_ptrs[uart_number]; i++) {
        printf("%c", uarts_buffers[uart_number][i]);
    }

    uarts_ptrs[uart_number] = 0;
}

void flush_msg(int uart_number) {
    if (last_chat_line < 59) {
        printf("\x1b[s");
        printf("\x1b[%d;2f", last_chat_line);
        printf("%s: ", uarts_names[uart_number]);
        print_uart_msg(uart_number);
        printf("\x1b[u");
        last_chat_line += 1;
    } else {
        //TODO: scrolling
        printf("\x1b[s");
        printf("\x1b[%d;2f", last_chat_line);
        printf("%s: ", uarts_names[uart_number]);
        print_uart_msg(uart_number);
        printf("\x1b[u");
    }
}

void clear_uart_name(int uart_number) {
    printf("\x1b[s");
    printf("\x1b[%d;61f", uart_number + 2);
    printf("%*s", 19, "");
    printf("\x1b[u");
}

void flush_uart_name(int uart_number) {
    clear_uart_name(uart_number);

    if (uarts_ptrs[uart_number] > 0 
        && (uarts_states[uart_number] / 1000) == 'H') {
        memcpy(uarts_names[uart_number], uarts_buffers[uart_number], uarts_ptrs[uart_number]);
        uarts_names[uart_number][uarts_ptrs[uart_number]] = '\0';
    }

    printf("\x1b[s");
    printf("\x1b[%d;64f", uart_number + 2);
    printf("%s", uarts_names[uart_number]);
    printf("\x1b[u");
}

void process_uarts() {
    for (int  j = 0; j < 2; j++) {
        while ((uart_regs[(uarts_offsets[j] + 0x14)/4]  & (1 << 0)) == 1) {
            char c = uart_regs[(uarts_offsets[j] + 0x00)/4];

            // printf("%d %d %c %d-", j, uarts_states[j], c, uarts_ptrs[j]); 

            if (uarts_states[j] == 0 && c == '\\') {
                uarts_states[j] = 1;
            } else if (uarts_states[j] == 1 && (c == 'M' || c == 'H')){
                uarts_states[j] = c * 1000;
            } else if (uarts_states[j] > 1000 && uarts_states[j] % 1000 == 0) {
                if (c == 0) {
                    memcpy(uarts_names[j], unknown, 7);
                    uarts_status[j] = 0;
                    clear_uart_name(j);
                }

                uarts_lens[j] = c;
                uarts_states[j] += 1;
                // printf("%d %d -- ", j, uarts_lens[j]);
            } else if (uarts_states[j] > 1000 && uarts_states[j] % 1000 - 1 < uarts_lens[j]) {
                uarts_states[j] += 1;
                // printf("%d %d ++ ", j, uarts_states[j] % 1000 - 1);

                if ((uarts_states[j] - 1) % 1000 >= 45) {
                    if ((uarts_states[j] / 1000) == 'H') {
                        flush_uart_name(j);
                    } else {
                        flush_msg(j);
                    }
                    uarts_ptrs[j] = 0;
                    update_uart_timer(j);
                } else {
                    uarts_buffers[j][uarts_ptrs[j]] = c;
                    uarts_ptrs[j] += 1;
                }
            } else if (uarts_states[j] > 1000 && uarts_states[j] % 1000 - 1 == uarts_lens[j]) {
                // printf("%d %d %c %d-", j, uarts_states[j], c, uarts_ptrs[j]); 
                if ((uarts_states[j] / 1000) == 'H') {
                    flush_uart_name(j);
                } else {
                    flush_msg(j);
                }

                uarts_states[j] = 0;
                uarts_ptrs[j] = 0;
                update_uart_timer(j);
            } else if (uarts_states[j] == 1 && c == 'P') {
                uarts_states[j] = 5;

                if (uarts_status[j] == 0 || strcmp(uarts_names[j], unknown) != 0) {
                    who();
                    flush_uart_name(j);
                }
            } else if (uarts_states[j] == 1 && c == 'W') {
                uarts_states[j] = 5;
                handshake();
            } else if (uarts_states[j] == 5) {
                uarts_states[j] = 0;
                uarts_ptrs[j] = 0;
                update_uart_timer(j);
            } else {
                uarts_states[j] = 0;
                uarts_ptrs[j] = 0;
                update_uart_timer(j);
            }
        }
    }
}

void check_timers() {
    for (int i = 0; i < 2; i++) {
        if (uarts_status[i] == 1) {
            if (rtc_vaddr[5] - uarts_timers[i] > 5) {
                uarts_status[i] = 0;
                memcpy(uarts_names[i], unknown, 7);
                clear_uart_name(i);
            }
        }
    }
}

int main() {
    if (init_memfd() != 0) {
        return 1;
    }

    init_timer();
    init_uarts();

    for (int i = 0; i < 2; i++) {
        uarts_names[i] = malloc(250);
        memcpy(uarts_names[i], unknown, 7);
        uarts_buffers[i] = malloc(250);
    }

    get_name();
    draw_ui();

    for (;;) {
        ping();
        process_uarts();
        process_kb();
        check_timers();
        fflush(0);
    }

    return 0;
}
