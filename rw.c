#include <stdio.h>
#include <fcntl.h>


int main() {
    
    int fd = open('fcopy', O_RDONLY|O_CLOEXEC);

    return 0;
}