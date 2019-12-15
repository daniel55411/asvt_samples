#include "ring_buffer.c"
#include "timer.c"


#define _DEFAULT_NAME "unknown"

enum Status {
    Offline,
    Online,
};

typedef struct client_t {
    buffer_t* buffer;
    char* name;
    enum Status status;
    time_t last_activity;
} client_t;

client_t* init_client() {
    client_t client = {
        .buffer = init_ring_buffer(1024),
        .name = NULL,
        .status = Offline,
        .last_activity = get_current_time(),
    };

    return &client;
}

void check_client_status(client_t* client, time_t cur_time) {
    if (cur_time - client->last_activity >= 4) {
        client->status = Offline;
        client->name = NULL;
    }
}

void update_client_activity(client_t* client) {
    client->last_activity = get_current_time();
    client->status = Online;
}
