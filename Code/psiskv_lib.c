#include "psiskv.h"

#define ERROR -1
#define DATA_SERVER 2
#define CLIENT 1
#define OK 0
#define MIN(a,b) (((a) < (b))? (a):(b))

int kv_connect(char * kv_server_ip, int kv_server_port)
{
	int sock_fd, other_sock_fd;
	int err, handshake;
	char ds_ip[50];
	int ds_port; 

	struct sockaddr_in server_addr, other_server_addr;

	sock_fd = socket(AF_INET, SOCK_STREAM, 0);

	if(sock_fd == -1)
	{
		perror("Error creating socket for client.");
		return -1;
	}

	server_addr.sin_family = AF_INET;

    server_addr.sin_port = htons(kv_server_port);
    inet_aton(kv_server_ip, &server_addr.sin_addr);
	
	err = connect(sock_fd, (struct sockaddr*) &server_addr, sizeof(server_addr));

	// Se a conexão com o front server falhou, não vale a pena receber o data server
	if(err == -1)
		return -1;

	handshake = CLIENT;
	send(sock_fd, &handshake, sizeof(handshake), 0);
	recv(sock_fd, &handshake, sizeof(handshake), 0);
	
	if(handshake == ERROR) //Alguma coisa correu mal
		return -1;

	recv(sock_fd, ds_ip, 50, 0);
	recv(sock_fd, &ds_port, sizeof(ds_port), 0);

	//Recebemos todos os dados do data server, fechar ligação com o front server
	close(sock_fd);

	//Tentar conectar ao data server
	other_sock_fd = socket(AF_INET, SOCK_STREAM, 0);

	if(other_sock_fd == -1)
	{
		perror("Error creating socket for client.");
		return -1;
	}

	other_server_addr.sin_family = AF_INET;
	
	inet_aton(ds_ip, &other_server_addr.sin_addr);
	other_server_addr.sin_port = htons(ds_port);

	err = connect(other_sock_fd, (struct sockaddr*) &other_server_addr, sizeof(other_server_addr));

	if(err == -1)
		return -1;
	else
		return other_sock_fd;
}

void kv_close(int kv_descriptor)
{
	close(kv_descriptor);	
}

int kv_write(int kv_descriptor, uint32_t key, char* value, int value_length, int overwrite)
{
	message mensagem;
	int ret_value;

	if(overwrite == 0)
		mensagem.operation = WRITE;
	else
		mensagem.operation = OVERWRITE;
	mensagem.key = key;
	mensagem.value_length = value_length;
	
	send(kv_descriptor, &mensagem, sizeof(mensagem), 0);

	send(kv_descriptor, value, value_length, 0);

	recv(kv_descriptor, &ret_value, sizeof(ret_value), 0);

	return ret_value;
}

int kv_read(int kv_descriptor, uint32_t key, char* value, int value_length)
{
	message mensagem;

	mensagem.operation = READ;
	mensagem.key = key;
	mensagem.value_length = value_length;
	
	send(kv_descriptor, &mensagem, sizeof(mensagem), 0);

	if(value_length > 0) 
	{

		recv(kv_descriptor, &mensagem, sizeof(mensagem), 0);

		if(mensagem.value_length == -1)
		{
			return -2;
		} 
		else
		{
			char * buffer = (char * ) malloc(mensagem.value_length*sizeof(char));
			recv(kv_descriptor, buffer, mensagem.value_length, 0);
			// memcopy deve ser minimo entre value<-length e message.value_length
			memcpy(value, buffer, MIN(mensagem.value_length,value_length));
			return mensagem.value_length;
		}
	} 
	else 
	{
		return -1;
	}
}

int kv_read_optimized(int kv_descriptor, uint32_t key, char** value)
{
	message mensagem;

	mensagem.operation = READ;
	mensagem.key = key;
	mensagem.value_length = 0;
	
	send(kv_descriptor, &mensagem, sizeof(mensagem), 0);

	recv(kv_descriptor, &mensagem, sizeof(mensagem), 0);

	if(mensagem.value_length == -1)
	{
		return -2;
	} 
	else
	{
		*value = (char *) malloc((mensagem.value_length+1)*sizeof(char));
		recv(kv_descriptor, *value, mensagem.value_length, 0);
		return mensagem.value_length;
	}
}

int kv_delete(int kv_descriptor, uint32_t key)
{
	message mensagem;

	mensagem.operation = DELETE;
	mensagem.key = key;
	mensagem.value_length = 0;
	
	send(kv_descriptor, &mensagem, sizeof(mensagem), 0);

	return 0;
}