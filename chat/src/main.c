#include "lib/timer.c"
#include "lib/uart.c"
#include "lib/ring_buffer.c"
#include "lib/proto.c"
#include "lib/common.c"


int main() {
    int memfd = get_shared_memory_fd();
    init_uarts(memfd);
    init_timer(memfd);
}