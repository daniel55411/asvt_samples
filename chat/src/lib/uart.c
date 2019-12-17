#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>


#define _CCU_BASE_ADDR  0x01C20000
#define _UART_BASE_ADDR 0x01C28000

#define _UART1_OFFSET 0x0400
#define _UART1_BIT    17

#define _UART2_OFFSET 0x0800
#define _UART2_BIT    18

const int* UART_OFFSETS = {_UART1_OFFSET, _UART2_OFFSET};
const int* UART_BITS = {_UART1_BIT, _UART2_BIT};

volatile unsigned* _CCU_REGS;
volatile unsigned* _UART_REGS;

/**
 * initialize uart and enable FIFO mode
 */
void _init_uart(int uart_number) {
    int uart_bit = UART_BITS[uart_number - 1];
    int uart_offset = UART_OFFSETS[uart_number - 1];

    _CCU_REGS[0x02D8/4] &= ~(1<<uart_bit); 
	_CCU_REGS[0x006C/4] |= 1<<uart_bit;    
	_CCU_REGS[0x02D8/4] |= 1<<uart_bit;

    _UART_REGS[(uart_offset + 0x0C)/4] |= 0b10000011;
    _UART_REGS[(uart_offset + 0x08)/4] |= 0b00000001;
	_UART_REGS[(uart_offset + 0x00)/4] = 78;
	_UART_REGS[(uart_offset + 0x04)/4] = 0;
	_UART_REGS[(uart_offset + 0x0C)/4] &= ~0b10000000;
}

/**
 * initialize all uarts
 */
void init_uarts(int memfd) {
    _CCU_REGS = (volatile unsigned *)mmap(NULL, 4096, PROT_READ | PROT_WRITE,
	                                     MAP_SHARED, memfd, _CCU_BASE_ADDR);

    if (_CCU_REGS == -1) {
        printf("ERROR: failed CCU Registers mapping\n");
        exit(1);
    }

    _UART_REGS = (volatile unsigned *)mmap(NULL, 4096, PROT_READ | PROT_WRITE,
	                                     MAP_SHARED, memfd, _UART_BASE_ADDR);

    if (_UART_REGS == -1) {
        printf("ERROR: failed UART Registers mapping\n");
        exit(1);
    }

    _init_uart(1);
    _init_uart(2);
}

void send_data_to_uart(int uart_number, char* data, int length) {
    int uart_offset = UART_OFFSETS[uart_number - 1];

    for (int i = 0; i < length; ++i) {
        while ((_UART_REGS[(uart_offset + 0x14)/4]  & (1 << 5)) == 0);

        _UART_REGS[(uart_offset + 0x00)/4] = data[i];
    }
}

char get_byte_from_uart(int uart_number) {
    int uart_offset = UART_OFFSETS[uart_number - 1];

    while ((_UART_REGS[(uart_offset + 0x14)/4]  & (1 << 0)) == 0) {
        return _UART_REGS[(uart_offset + 0x00)/4];
    }
}
