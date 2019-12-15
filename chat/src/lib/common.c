#include <fcntl.h>
#include <stdio.h>


int get_shared_memory_fd() {
        int memfd = open("/dev/mem", O_RDWR | O_DSYNC);

	if (memfd == -1) {
		printf("Error: can not open /dev/mem\n");
		exit(1);
	}

    return memfd;
}