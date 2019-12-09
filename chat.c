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
volatile char write_buffer[160]; 
volatile int write_ptr;

volatile unsigned *ccu_regs;
volatile unsigned *uart_regs;

int uarts_offsets[2] = {uart1_offset, uart2_offset};
volatile int uarts_ptrs[2] = {0, 0};
volatile char* uarts_buffers[2];

int getche();
int kbhit();

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

void 

int send_raw_message(char* buffer, int length) {

}

int send_message() {

}

void process_kb() {
    if (kbhit() > 0) {
        char c = getche();

        if (c == '\n') {
            send_message();
        } else {
            writer_buffer[write_ptr] = c;
            ++write_ptr;
            // mod;
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
                // mod;
            }
        }
    }
}

int main() {
    init_uarts();
    get_name();
    draw_ui();

    for (;;) {
        process_uarts();
        process_kb();
    }

    return 0;
}
