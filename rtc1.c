#include <stdio.h>
#include <assert.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>

#define	rtc_base_addr		0x01f00000	// базовый адрес регистров RTC
volatile unsigned* databuf;			// указатель на виртуальный адрес

int main() { 
	int memfd = open("/dev/mem", O_RDWR | O_DSYNC);	
	databuf = (volatile unsigned*)mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, rtc_base_addr);
	printf("RTC Year-Month-Day Register      (0x10) : %08x\n", databuf[4]);
	printf("RTC Hour-Minute-Second Register  (0x14) : %08x\n", databuf[5]);

	databuf[5] = 249;
	printf("RTC Hour-Minute-Second Register  (0x14) : %08x\n", databuf[5]);
	return 0;
}
