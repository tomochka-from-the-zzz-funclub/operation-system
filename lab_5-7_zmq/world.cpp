#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "zmq.h"

#include <zmq.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <assert.h>
#include <signal.h>


int main (int argc, char const *argv[]) 
{
	void* context = zmq_ctx_new();
	void* respond = zmq_socket(context, ZMQ_REP);
	
	zmq_bind(respond, "tcp://127.0.0.1:4041");
	printf("Starting…\n");
	
	for(;;) 
	{
		zmq_msg_t request;
		zmq_msg_init(&request);
        int size = zmq_msg_recv(&request, respond, 0);
        if (size == -1) return NULL;
        char* string = (char*)malloc(size + 1);
        memcpy(string, zmq_msg_data(&request), size);
        zmq_msg_close(&request);

		printf("Received: %s\n", string);
		zmq_msg_close(&request);
		sleep(1); // sleep one second
		zmq_msg_t reply;
		zmq_msg_init_size(&reply, strlen("world"));
		memcpy(zmq_msg_data(&reply), "world", 5);
		zmq_msg_send(&reply, respond, 0);
		zmq_msg_close(&reply);
	}
	zmq_close(respond);
	zmq_ctx_destroy(context);
	
	return 0;
}