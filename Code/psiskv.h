#include <stdlib.h>
#include <stdio.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "message.h"

int kv_connect(char * kv_server_ip, int kv_server_port);
void kv_close(int kv_descriptor);
int kv_write(int kv_descriptor, uint32_t key, char* value, int value_length, int overwrite);
int kv_read(int kv_descriptor, uint32_t key, char* value, int value_length);
int kv_read_optimized(int kv_descriptor, uint32_t key, char** value);
int kv_delete(int kv_descriptor, uint32_t key);