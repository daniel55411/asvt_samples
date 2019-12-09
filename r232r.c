#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>

#define ccu_base_addr  0x01C20000
#define uart_base_addr 0x01C28000

#define uart1_offset 0x0800
#define uart1_bit    18

volatile unsigned *ccu_regs;
volatile unsigned *uart_regs;

int main()
{
	int memfd = open("/dev/mem", O_RDWR | O_DSYNC);
	if (memfd == -1)
	{
		printf("Ошибка открытия файла /dev/mem\n");
		return 1;
	}

	ccu_regs = (volatile unsigned *)mmap(
	                                     NULL, 4096, PROT_READ | PROT_WRITE,
	                                     MAP_SHARED, memfd, ccu_base_addr);
	uart_regs = (volatile unsigned *)mmap(
	                                     NULL, 4096, PROT_READ | PROT_WRITE,
	                                     MAP_SHARED, memfd, uart_base_addr);

  ccu_regs[0x02D8/4] &= ~(1<<uart1_bit);      // делаем ему reset
	ccu_regs[0x006C/4] |= 1<<uart1_bit;      // включаем часы у uart1
	ccu_regs[0x02D8/4] |= 1<<uart1_bit;      // делаем ему reset

	uart_regs[(uart1_offset + 0x0C)/4] |= 0b10000011;
	uart_regs[(uart1_offset + 0x00)/4] = 78;
	uart_regs[(uart1_offset + 0x04)/4] = 0;
	uart_regs[(uart1_offset + 0x0C)/4] &= ~0b10000000;

	printf("\x1b[H\x1b[2J");

	int state = 0;
	char symbol;

  for (; ;) {
    while ((uart_regs[(uart1_offset + 0x14)/4]  & (1 << 0)) == 0);

		symbol = uart_regs[(uart1_offset + 0x00)/4];

		if (state == 0 && symbol == 0x1b) {
			state = 1;
		} else if (state == 0) {
			printf("%c", symbol);
		} else if (state == 1 && symbol == 0x5b) {
			state = 2;
		} else if (state == 1 && symbol == 0x4f) {
			state = 3;
		} else if (state == 1) {
			state = 0;
			printf("%c", symbol);
		} else if (state == 2 && symbol >= 0x41 && symbol <= 0x44) {
			state = 0;
			printf("\x1b[%c", symbol);
		} else if (state == 2 && symbol == 0x31) {
			state = 4;
		} else if (state == 2 && symbol == 0x32) {
			return 0;
		} else if (state == 3 && symbol >= 0x50 && symbol <= 0x52) {
			state = 0;
			symbol = symbol - 0x50 + 0x30;
			printf("\x1b[3%cm", symbol);
		} else if (state == 3 && symbol == 0x53) {
			state = 0;
			printf("\x1b[39m");
		} else if (state == 4 && symbol >= 0x35 && symbol <= 0x37) {
			state = 5;
			printf("\x1b[4%cm", symbol);
		} else if (state == 4 && symbol == 0x38) {
			state = 5;
			printf("\x1b[49m");
		} else if (state == 4 && symbol == 0x39) {
			state = 5;
			printf("\x1b[H\x1b[2J");
		} else if (state == 5) {
			state = 0;
		}

		fflush(0);
  }
  printf("\n");


	return 0;
}
