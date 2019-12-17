#include <string.h>


char* _message(char type, const char* data, char size) {
    char* message = (char*)malloc(size);

    message[0] = '\\';
    message[1] = type;
    message[2] = size;
    memcpy(message[3], data, size);

    return message;
}

char* ping_message() {
    return "\\P0";
}

char* who_message() {
    return "\\W0";
}

char* handshake_message(const char* name, char size) {
    return _message('H', name, size);
}

char* message_message(const char* user_message, char size) {
    return _message('M', user_message, size);
}
