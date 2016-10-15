#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h> 
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define READ 0
#define WRITE 1
#define OVERWRITE 2
#define DELETE 3

typedef struct message {
	int operation;
	uint32_t key;
	int value_length;
} message;