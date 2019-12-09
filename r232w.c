#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>

int getch();
int getche();

#define ccu_base_addr  0x01C20000
#define uart_base_addr 0x01C28000

#define uart1_offset 0x0400
#define uart1_bit    17

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

  char input_char;
	int state = 0;

  while (input_char = getch()) {
    while ((uart_regs[(uart1_offset + 0x14)/4]  & (1 << 5)) == 0);

    uart_regs[(uart1_offset + 0x00)/4] = input_char;

		if (state == 0 && input_char == 0x1b) {
			state = 1;
		} else if (state == 1 && input_char == 0x5b) {
			state = 2;
		} else if (state == 2 && input_char == 0x32) {
			return 0;
		} else {
			state = 0;
		}
  }

	return 0;
}
