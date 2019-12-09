#include <stdio.h>

#define terminal_width  80
#define terminal_height 25

#define ccu_base_addr  0x01C20000
#define uart_base_addr 0x01C28000

#define uart1_offset 0x0400
#define uart1_bit    17
#define uart2_offset 0x0800
#define uart2_bit    18

volatile char name[15];
int name_length;

volatile char write_buffer[160]; 
volatile int write_ptr;

volatile unsigned *ccu_regs;
volatile unsigned *uart_regs;

int uarts_offsets[2] = {uart1_offset, uart2_offset};
volatile int uarts_ptrs[2] = {0, 0};
volatile int uarts_status[2] = {0, 0};
volatile int uarts_states[2] = {0, 0};
volatile char* uarts_buffers[2];

int getch();
int kbhit();

int strlen(char* buffer) {
    int i = 0;

    while (buffer[i] != '\0') {
        ++i;
    }

    return i;
}

void init_uarts() {
    int memfd = open("/dev/mem", O_RDWR | O_DSYNC);

	if (memfd == -1) {
		printf("Ошибка открытия файла /dev/mem\n");
		return 1;
	}

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

void 

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

void flush_message(int uart_number) {

}

void process_uarts() {
    for (int  i = 0; i < 2; i++) {
        if ((uart_regs[(uarts_offsets[i] + 0x14)/4]  & (1 << 0)) == 1) {
            char c = uart_regs[(uart1_offset + 0x00)/4];

            if (c == '\n') {
                flush_message(i);
            } else {
                uarts_buffers[i][uarts_ptrs[i]] = c;
                ++uarts_ptrs[i];
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

int main() {
    init_uarts();
    get_name();
    draw_ui();
    handshake();

    for (;;) {
        process_uarts();
        process_kb();
        ping();
    }

    return 0;
}
