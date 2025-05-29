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
	printf("Client Starting….\n");
	
	void* request = zmq_socket(context, ZMQ_REQ);
	zmq_connect(request, "tcp://127.0.0.1:4041");
	
	int count = 0;
	
	for(;;) 
	{
		zmq_msg_t req;
		zmq_msg_init_size(&req, strlen("hello"));
		memcpy(zmq_msg_data(&req), "hello", 5);
		printf("Sending: hello - %d\n", count);
		zmq_msg_send(&req, request, 0);
		zmq_msg_close(&req);
		zmq_msg_t reply;
		zmq_msg_init(&reply);
        int size = zmq_msg_recv(&reply, request, 0);
        if (size == -1) return NULL;
        char* string = (char*)malloc(size + 1);
        memcpy(string, zmq_msg_data(&reply), size);
        zmq_msg_close(&reply);
		printf("Received: hello - %s %d\n", string, count);
		count++;
	}
	// We never get here though.
	zmq_close(request);
	zmq_ctx_destroy(context);
	
	return 0;
}
