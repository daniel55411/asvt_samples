#include <stdio.h>

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

char* unknown = "unknown";
char name[15];
int name_length;

char write_buffer[160]; 
int write_ptr;

volatile unsigned *ccu_regs;
volatile unsigned *uart_regs;

int uarts_offsets[2] = {uart1_offset, uart2_offset};
int uarts_ptrs[2] = {0, 0};
int uarts_status[2] = {0, 0};
int uarts_states[2] = {0, 0};
char* uarts_buffers[2];
char* uarts_names[2] = {unknown, unknown};
char uarts_lens[2] = {0, 0};
char uarts_timers[2] = {0, 0};

volatile unsigned* rtc_vaddr;

int getch();
int kbhit();

int strlen(char* buffer) {
    int i = 0;

    while (buffer[i] != '\0') {
        ++i;
    }

    return i;
}

void init_memfd() {
    memfd = open("/dev/mem", O_RDWR | O_DSYNC);

	if (memfd == -1) {
		printf("Ошибка открытия файла /dev/mem\n");
		return 1;
	}
}

void init_timer() {
    rtc_vaddr = (volatile unsigned*)mmap(NULL, 4096, P
                    ROT_READ | PROT_WRITE, MAP_SHARED, memfd, rtc_base_addr);
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
	uart_regs[(uart1_offset + 0x00)/4] = 78;
	uart_regs[(uart1_offset + 0x04)/4] = 0;
	uart_regs[(uart1_offset + 0x0C)/4] &= ~0b10000000;

    uart_regs[(uart2_offset + 0x0C)/4] |= 0b10000011;
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
}

void get_name() {
    printf("\x1b[H\x1b[2J");
    printf("\x1b[%d;%df", terminal_height / 2, terminal_width / 2 - 20);
    printf("Введите логин: ");
    scanf("%15s", name);
    getchar();
    fflush(0);
}

void clear_msg_wnd() {
    for (int i = 21; i < 25; i++) {
        printf("\x1b[%d;0f", i);
        printf("%*s", terminal_width, "");
    }

    printf("\x1b[%d;%df", 21, 2);
}

void send_raw_message(char* buffer, int length) {
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < 2; j++) {
            if (uarts_status[j] == 1) {
                while ((uart_regs[(uarts_offsets[j] + 0x14)/4]  & (1 << 5)) == 0);

                uart_regs[(uarts_offsets[j] + 0x00)/4] = buffer[i];
            }
        }
    }
}

int send_message() {
    send_raw_message("\\M%c", write_ptr);
    send_raw_message(write_buffer, write_ptr);

    write_ptr = 0;
    clear_msg_wnd();
}

void process_kb() {
    if (kbhit() > 0) {
        char c = getch();

        if (c == '\n' || writer_ptr >= 160) {
            send_message();

            if (c != '\n') {
                writer_buffer[write_ptr] = c;
                ++write_ptr;
                printf("%c", c);
            }
        } else {
            writer_buffer[write_ptr] = c;
            ++write_ptr;
            printf("%c", c);
        }
    }
}

void update_uart_timer(int uart_number) {
    uarts_timers[uart_number] = rtc_vaddr[5];
    uarts_status[uart_number] = 1;
}

void flush_msg(int uart_number) {
    // TODO: impl
}

void flush_name(int uart_number) {
    printf("\x1b[s");
    printf("\x1b[%d;61f", uart_number);
    print_uart_msg(uart_number);
    printf("\x1b[u");
}

void clear_name(int uart_number) {
    printf("\x1b[s");
    printf("\x1b[%d;61f", uart_number);
    prinft("%*s", 19, "");
    printf("\x1b[u");
}

void print_uart_msg(int uart_number) {
    for (int i = 0; i < uarts_ptrs[uart_number]; i++) {
        printf("%c", uarts_buffers[uart_nubmer][i]);
    }

    uarts_ptrs[uart_number] = 0;
}

void process_uarts() {
    for (int  i = 0; i < 2; i++) {
        if ((uart_regs[(uarts_offsets[i] + 0x14)/4]  & (1 << 0)) == 1) {
            char c = uart_regs[(uart1_offset + 0x00)/4];

            if (uarts_states[j] == 0 && c == '\\') {
                uarts_states[j] = 1;
            } else if (uarts_states[j] == 1 && (c == 'M' || c == 'H')){
                uarts_states[j] = c * 1000;
            } else if (uarts_states[j] % 1000 == 0) {
                if (c == 0) {
                    uarts_names[j] = unknown;
                    uarts_status[j] = 0;
                    clear_name(j);
                }

                uarts_lens[j] = c;
                uarts_states[j] += 1;
            } else if (uarts_states[j] % 1000 - 1 < uarts_lens[j]) {
                uarts_states[j] += 1;

                if (uarts_states[j] - 1 % 1000 >= 45) {
                    if ((uarts_states[j] / 1000) == 'H') {
                        flush_name(j);
                    } else {
                        flush_msg(j);
                    }
                } else {
                    uarts_buffers[j][uarts_ptrs[j]] = c;
                    uarts_ptrs[j] += 1;
                }
            } else if (uarts_states[j] % 1000 - 1 == uarts_lens[j]) {
                if ((uarts_states[j] / 1000) == 'H') {
                    flush_name(j);
                } else {
                    flush_msg(j);
                }

                uarts_states[j] = 0;
            } else if (uarts_states[j] == 1 && c == 'P') {
                uarts_states[j] = 5;

                if (uarts_status[j] == 0) {
                    who();
                }

                update_uart_timer(j);
            } else if (uarts_states[j] == 1 && c == 'W') {
                uarts_states[j] = 5;
                handshake();
            } else {
                uarts_states[j] = 0;
            }
        }
    }
}

void ping() {
    send_raw_message("\\P0", 3);
}

void who() {
    send_raw_message("\\W0", 3);
}

void handshake() {
    int len = strlen(name);

    send_raw_message("\\H%c", len);
    send_raw_message(name, len);
}

void check_timers() {
    for (int i = 0; i < 2; i++) {
        if (uarts_status[i] == 1) {
            if (rtc_vaddr[5] - uarts_timers[i] > 3) {
                uarts_status[i] = 0;
                uarts_names[i] = unknown;
                clear_name(i);
            }
        }
    }
}

int main() {
    init_memfd();
    init_timer();
    init_uarts();

    get_name();
    draw_ui();

    for (;;) {
        process_uarts();
        process_kb();
        ping();
        check_timers();
    }

    return 0;
}
