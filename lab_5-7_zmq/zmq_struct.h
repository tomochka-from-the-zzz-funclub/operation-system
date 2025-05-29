#include <zmq.h>
#include <string.h>

typedef struct{
    int key;
    int id;
    long long size;
} message_zmq;

enum Status{
    CHECK,
    PING,
    CREATE,
    REMOVE,
    EXEC,
    SUCCESS,
    FAIL,
    FAIL_PING,
    FAIL_CREATE
};

int create_conn(int id, void * socket, void * context) {
    int rc;
    char path[40] = "tcp://*:";
    int new_port = id + 5555;
    char port[40];
    sprintf(port, "%d", new_port);
    strcat(path, port);
    rc = zmq_bind(socket, path);
    return 1;
}
int ping_send(void * socket, message_zmq * msg) {
    int rc;
    zmq_msg_t message;
    zmq_msg_init(&message);
    rc = zmq_msg_init_size(&message, sizeof(message_zmq));
    rc = zmq_msg_init_data(&message, msg, sizeof(message_zmq), NULL, NULL);
    rc = zmq_msg_send(&message, socket, 0);
    if (rc == -1) {
        zmq_msg_close(&message);
        printf("Error: sending timeout\n");
        return 0;
    }
    return 1;
}

message_zmq * ping_receive(void * socket) {
    int rc;
    zmq_msg_t ans;
    zmq_msg_init(&ans);
    rc = zmq_msg_recv(&ans, socket, 0);
    if (rc == -1) {
        zmq_msg_close(&ans);
        return NULL;
    }
    //assert(rc == sizeof(message_zmq));
    message_zmq * data = (message_zmq *)zmq_msg_data(&ans);
    rc = zmq_msg_close(&ans);
    //assert(rc == 0);
    return data;
}
void send_string(void *socket, const char *string) {
	zmq_send(socket, strdup(string), strlen(string), 0);
}
char * recv_string (void *socket, long long size) {
    char buffer[size];
    zmq_recv (socket, buffer, size-1, 0);
    buffer [size] = '\0';
    return strdup (buffer);
}