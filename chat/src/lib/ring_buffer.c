#include <string.h>
#include <stdlib.h>

typedef struct buffer_t
{
    unsigned int start_ptr;
    unsigned int end_ptr;
    unsigned int capacity;
    char* buffer;
} buffer_t;


buffer_t* init_ring_buffer(size_t size) {
    buffer_t buffer = {
        .start_ptr = 0,
        .end_ptr = 0,
        .buffer = (char *)malloc(size),
        .capacity = size,
    };

    return &buffer;
}

size_t buffer_length(buffer_t* buffer) {
    if (buffer->end_ptr < buffer->start_ptr) {
        return buffer->capacity - buffer->start_ptr + buffer->end_ptr + 1;
    }

    return buffer->end_ptr - buffer->start_ptr + 1;
}

void add_to_buffer(buffer_t* buffer, char item) {
    buffer->end_ptr = (buffer->end_ptr + 1) % buffer->capacity;
    buffer->buffer[buffer->end_ptr] = item;
}

int extract_from_buffer(char* dst, buffer_t* buffer, size_t count) {
    size_t buffer_size = buffer_length(buffer);
    size_t slice_size = count < buffer_size ? count : buffer_size;

    if (buffer->start_ptr + slice_size - 1 > buffer->capacity) {
        size_t part_1 = buffer->capacity - buffer->start_ptr;
        size_t part_2 = slice_size - part_1;

        memcpy(dst, buffer->buffer[buffer->start_ptr], part_1);
        memcpy(dst[part_1], buffer->buffer[0], part_2);
    } else {
        memcpy(dst, buffer->buffer[buffer->start_ptr], slice_size);
    }

    buffer->start_ptr = (buffer->start_ptr + slice_size) % buffer->capacity; 

    return slice_size;
}
