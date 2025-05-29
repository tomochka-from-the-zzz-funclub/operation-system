#include <zmq.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "zmq_struct.h"

int process = 1;
int children = 0;
void * context;
void * socket;


int main (){
    char * s = NULL;
    size_t len = 0;
    while(process) {
        getline(&s, &len, stdin);
        strtok(s, " ");
        if(!strcmp(s,"create")) {
            s = strtok(NULL, " ");
            int id = atoi(s);
            if(children == 0) {
                int rc;
                context = zmq_ctx_new();
                socket = zmq_socket(context, ZMQ_PAIR);
                create_conn(id, socket, context);
                char id_str[10];
                sprintf(id_str, "%d", id);
                if(fork() == 0) {
                    execlp("./server", "server", id_str, NULL);
                    perror("execlp");
                } else {
                    message_zmq reply = {CHECK, 0};
                    ping_send(socket, &reply);
                    message_zmq * data = ping_receive(socket);
                    if(data->key == SUCCESS) {
                        printf("OK:%d\n", data->id);
                        children = id;
                    }
                    if(data->key == FAIL_CREATE){
                        printf("Error: Port is busy\n");
                        rc = zmq_close(socket);
                        rc = zmq_ctx_destroy(context);
                    }
                }
            } else {
                message_zmq reply = {CREATE, id};
                ping_send(socket, &reply);
                message_zmq * data = ping_receive(socket);
                if(data->key == SUCCESS) {
                    printf("OK:%d\n", data->id);
                }
                if(data->key == FAIL) {
                    printf("Error: Already exists\n");
                }
                if(data->key == FAIL_CREATE || data->key == FAIL_PING) {
                    printf("Error: Port is busy\n");
                }
            }
        }
        if(!strcmp(s,"remove")) {
            s = strtok(NULL, " ");
            int id = atoi(s);
            if(children == 0) {
                printf("Error: Not found\n");
            } 
            else if(children == id) {
                message_zmq reply = {REMOVE, id};
                ping_send(socket, &reply);
                message_zmq * data = ping_receive(socket);
                if(data->key == FAIL) {
                    printf("Error: Not found\n");
                }
                if(data->key == SUCCESS) {
                    int rc;
                    printf("Ok\n");
                    rc = zmq_close(socket);
                    rc = zmq_ctx_destroy(context);
                    children = 0;
                }
            }
            else {
                message_zmq reply = {REMOVE, id};
                ping_send(socket, &reply);
                message_zmq * data = ping_receive(socket);
                if(data->key == FAIL) {
                    printf("Error: Not found\n");
                }
                if(data->key == FAIL_PING) {
                    printf("Error: Node is unavailable\n");
                }
                if(data->key == SUCCESS) {
                    printf("Ok\n");
                }
            }
        }
        if(!strcmp(s,"ping")) {
            s = strtok(NULL, " ");
            int id = atoi(s);
            if(children == 0) {
                printf("Error: Not found\n");
            }
            else {
                message_zmq reply = {PING, id};
                ping_send(socket, &reply);
                message_zmq * data = ping_receive(socket);
                if(data->key == FAIL) {
                    printf("Error: Not found\n");
                }
                if(data->key == SUCCESS) {
                    printf("Ok: 1\n");
                }
                if(data->key == FAIL_PING) {
                    printf("Ok: 0\n");
                }
            }
        }
        if(!strcmp(s,"exec")) {
            s = strtok(NULL, " ");
            int id = atoi(s);
            s = strtok(NULL, " ");
            int n = atoi(s);
            char * query = s;
            for(int i = 0; i < n; ++i) {
                s = strtok(NULL, " ");
                sprintf(query, "%s %s",query, s);
            }
            int size = strlen(query);
            if(children == 0) {
                printf("Error: Not found\n");
            }
            else {
                message_zmq reply = {EXEC, id, size};
                ping_send(socket, &reply);
                send_string(socket, query);
                message_zmq * data = ping_receive(socket);
                if(data->key == FAIL) {
                    printf("Error: Not found\n");
                }
                if(data->key == FAIL_PING) {
                    printf("Error: Node is unavailable\n");
                }
                if(data->key == SUCCESS) {
                    printf("OK:%d:%lld\n", data->id, data->size);
                }
            }
        }
        if(!strcmp(s,"down\n")) {
            process = 0;
        }
    }
    printf("Server is down\n");
    return 0;
}
