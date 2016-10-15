#include "data-aux.h"

uint32_t interval = SAVE_TIME;
int killbackup = 0;
int send_to_front = 0;
int port = 9999;
int ds_port = 9998;

int hash_function(uint32_t key)
{
	return key*(key+3)%NUMBER_BUCKETS;
}

void save_structure()
{
	int i = 0;
	FILE * file = fopen("struct_log.txt", "w");

	fclose(file);

	int fd = open("struct_log.txt", O_WRONLY | O_CREAT, S_IWRITE | S_IREAD);
	if(fd == -1)
		return;

	//Lock all mutexes so that no client can add/remove during this time
	for(i = 0; i < NUMBER_BUCKETS; i++)
		pthread_mutex_lock(&lock[i]);

	//Saves every struct into the file
	for(i = 0; i < NUMBER_BUCKETS; ++i)
	{
		snapList(hashtable[i], fd);
	}

	//Unlock all mutexes
	for(i = 0; i < NUMBER_BUCKETS; i++)
		pthread_mutex_unlock(&lock[i]);

	close(fd);
	//Truncates file to size 0
	file = fopen("logs.txt", "w");

	fclose(file);
}

void structure_recovery()
{
	uint32_t key;
	int value_length;
	int hashkey;
	char * buffer;

	int fd = open("struct_log.txt", O_RDONLY, S_IWRITE | S_IREAD);
	//Reads something the size of key.
	while(read(fd, &key, sizeof(key)) == sizeof(key))
	{
		//Reads something the size of value_length
		read(fd, &value_length, sizeof(value_length));

		hashkey = hash_function(key);
		buffer = (char *)malloc((value_length)*sizeof(char));
		//Reads something with value_length for size.
		read(fd, buffer, value_length);
		//Saves it to the struct
		addToList(hashtable[hashkey], key, buffer, value_length, 1);
	}
}

void log_recovery()
{
	char * buffer;
	int hashkey, fd;
	message * mensagem = malloc(sizeof(mensagem));

	fd = open("logs.txt", O_RDONLY, S_IWRITE | S_IREAD);
	if(fd == -1)
		return;

	while(read(fd, mensagem, sizeof(*mensagem)) == sizeof(*mensagem))
	{
		//Allocates memory for non delete operations
		if(mensagem->operation != DELETE)
		{
			buffer = (char *) malloc(mensagem->value_length);

			if(read(fd, buffer, mensagem->value_length) == 0)
				continue;
		}

		hashkey = hash_function(mensagem->key);

		//Checks what operation is read from the log and executes it.
		switch(mensagem->operation)
		{
			case WRITE:
				addToList(hashtable[hashkey], mensagem->key, buffer, mensagem->value_length, 0);
				break;
			case OVERWRITE:
				addToList(hashtable[hashkey], mensagem->key, buffer, mensagem->value_length, 1);
				break;
			case DELETE:
				deleteFromList(hashtable[hashkey], mensagem->key);
				break;
			default:
				break;
		}
		if(mensagem->operation != DELETE)
			free(buffer);
	}

	close(fd);
}

void write_to_log(message * mensagem, char * value)
{
	int fd = open("logs.txt", O_WRONLY | O_APPEND | O_CREAT, S_IWRITE | S_IREAD);
	if(fd == -1)
		return;

	write(fd, mensagem, sizeof(*mensagem));

	if(mensagem->operation != DELETE)
		write(fd, value, mensagem->value_length);

	close(fd);
}

void *thread_dealing (void *ptr) {
	struct sockaddr_in client_addr;
	socklen_t size_addr;
	char * dados;
	message mensagem;
	int nbytes, ret_val;

	while(1) {
	    size_addr = sizeof(client_addr);

		int new_fd = accept(sock_fd,(struct sockaddr *) &client_addr, &size_addr);
		int hashkey;
		
		//Thread management
		n_empty--;

		if (n_empty < SIZE_OF_POOL)
		{
			// Few active threads, create a new one.
			pthread_t temp_thread;
			int iret = pthread_create( &temp_thread, NULL, thread_dealing, NULL);
			n_empty++;
			if(iret)
			{
				fprintf(stderr,"Error - pthread_create() return code: %d\n",iret);
				exit(EXIT_FAILURE);
			}
		}

		if(new_fd == -1)
		{
			perror("client addr: ");
			break;
		}

		//Client request cycle
		while(1) 
		{
			nbytes = recv(new_fd, &mensagem, sizeof(mensagem), 0);
			//If the client close the socket (receive 0 bytes), then the thread should quit
			if(nbytes == 0)
				break;
			
			//Evaluate the hashkey of the key passed in the message, as it will be used in any operation
			hashkey = hash_function(mensagem.key);
			
			switch(mensagem.operation)
			{
				case READ:
					//Read the value in the key position and return the value of the message
					
					if(mensagem.value_length < 1)
						break;

					char * newdados;

					//Lock the mutex connected to the specific bucket
					pthread_mutex_lock(&lock[hashkey]);
					
					//Get the value from the list, and verify its value_length
					mensagem.value_length = getFromList(hashtable[hashkey], mensagem.key, &newdados);
					
					//Unlock the mutex connected to the specific bucket
					pthread_mutex_unlock(&lock[hashkey]);
					
					//Send the message with the value_length of the value in the position 
					send(new_fd, &mensagem, sizeof(mensagem), 0);

					//If there is anything to send, send it to the client
					if(mensagem.value_length != -1)
						send(new_fd, newdados, mensagem.value_length, 0);

					break;
				case OVERWRITE:
					//Receive the value and write it in the key position (overwritting what was there), returning whether it was successful or not
					
					dados = (char *) malloc(mensagem.value_length);
					nbytes = recv(new_fd, dados, mensagem.value_length, 0);

					pthread_mutex_lock(&lock[hashkey]);
					
					//Write the value into the hahstable
					ret_val = addToList(hashtable[hashkey], mensagem.key, dados, mensagem.value_length, 1);
					
					pthread_mutex_unlock(&lock[hashkey]);

					write_to_log(&mensagem, dados);

					//Return the outcome of the writting
					send(new_fd, &ret_val, sizeof(ret_val), 0);
					
					free(dados);
					
					break;
				case WRITE:
					//Receive the value and write it in the key position (not overwritting what was there), returning whether it was successful or not

					dados = (char *) malloc(mensagem.value_length);
					nbytes = recv(new_fd, dados, mensagem.value_length, 0);

					pthread_mutex_lock(&lock[hashkey]);

					//Write the value into the hahstable
					ret_val = addToList(hashtable[hashkey], mensagem.key, dados, mensagem.value_length, 0);
					
					pthread_mutex_unlock(&lock[hashkey]);

					write_to_log(&mensagem, dados);
					
					//Return the outcome of the writting
					send(new_fd, &ret_val, sizeof(ret_val), 0);

					free(dados);
	
					break;
				case DELETE:
					//Delete the value in the key position of the hashtable
					pthread_mutex_lock(&lock[hashkey]);

					deleteFromList(hashtable[hashkey], mensagem.key);

					pthread_mutex_unlock(&lock[hashkey]);

					write_to_log(&mensagem, NULL);
					//This function is always successful, so no need to return anything to the client
					break;
				default:
					break;
			}
		}

		close(new_fd);
		n_empty++;

		if (n_empty > SIZE_OF_POOL)
		{
			// Plenty of empty threads, deactivate this one
			n_empty--;
			break;
		}
    }

    return NULL;
}

void * thread_backup(void * ptr) 
{
	while(!killbackup)
	{
		sleep(SAVE_TIME);
		save_structure();
	}
	return NULL;
}

int relaunch_front_server() {
	struct sockaddr_in other_client_addr, other_server_addr;
	socklen_t other_size_addr;
	int other_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	int nbytes1, fs_port;
	int port_com = 9990;
	int err = -1;

	if(other_sock_fd == -1){
		perror("socket: ");
		exit(-1);
	}
	
	other_server_addr.sin_family = AF_INET;
	inet_aton("127.0.0.1", &other_server_addr.sin_addr);

	while(err == -1){
		other_server_addr.sin_port = htons(port_com);
		
		err = bind(other_sock_fd, (struct sockaddr *) &other_server_addr, sizeof(other_server_addr));
		port_com--;
		if(port_com < 9980)
			port_com = 9990;
	}

	port_com++;

	other_size_addr = sizeof(other_client_addr);

	listen(other_sock_fd, 20);

	if(!fork()) 
	{
		//Set up data transfer into the newly run front-server
		char buffer2[100];
		sprintf(buffer2, "%d", port_com);
		char * buffer[] = {"./front-server", buffer2, 0};
		execve(buffer[0], buffer, NULL);
	}

	int other_fd = accept(other_sock_fd,(struct sockaddr *) &other_client_addr, &other_size_addr);
	if(other_fd == -1) {
		perror("client addr: ");
		exit(-23);
	}
	
	//Receive the new front server port
	nbytes1 = recv(other_fd, &fs_port, sizeof(fs_port), 0);
	if(nbytes1 == 0)
		exit(-1);

	close(other_fd);
	
	return fs_port;
}

void * front_server_communication(void * ptr) 
{
	int use_sock_fd, handshake, nbytes, i;
	int err = -1;

	if(send_to_front) 
	{
		port = relaunch_front_server();
	}

	struct sockaddr_in server_addr;

	use_sock_fd = socket(AF_INET, SOCK_STREAM, 0);

	if(use_sock_fd == -1)
	{
		perror("Error creating socket for front server communication.");
		exit(0);
	}

	server_addr.sin_family = AF_INET;
	inet_aton("127.0.0.1", &server_addr.sin_addr);

	i = 0;
	while(err == -1)
	{
		close(use_sock_fd);

		use_sock_fd = socket(AF_INET, SOCK_STREAM, 0);

		if(use_sock_fd == -1)
		{
			perror("Error creating socket for front server communication.");
			exit(0);
		}
		server_addr.sin_family = AF_INET;
		inet_aton("127.0.0.1", &server_addr.sin_addr);
    	server_addr.sin_port = htons(port);

		err = connect(use_sock_fd, (struct sockaddr*) &server_addr, sizeof(server_addr));

		i++;		
		
		if(i > 10)
			i = 0;
	}

	if(err == -1)
	{
		exit(-1);
	}

	handshake = DATA_SERVER;
	send(use_sock_fd, &handshake, sizeof(handshake), 0);
	
	nbytes = recv(use_sock_fd, &handshake, sizeof(handshake), 0);

	if(handshake == ERROR)
		exit(0);

	send(use_sock_fd, ds_ip, 50, 0);
	send(use_sock_fd, &ds_port, sizeof(ds_port), 0);

	while(1) 
	{
		sleep(1);
		nbytes = recv(use_sock_fd, &handshake, sizeof(handshake), 0);
		if(handshake == 1)
		{
			killbackup =1;
			save_structure();
			int j;
			for(j = 0; j < NUMBER_BUCKETS; j++)
				deleteList(hashtable[j]);

			exit(-1);
		}
		if(nbytes == 0)
		{
			send_to_front = 1;
			pthread_t front_server;
    		int iret = pthread_create(&front_server, NULL, front_server_communication, NULL);
    		if(iret)
    		{
    			fprintf(stderr, "No thread creating backups, pthread_create() return code: %d\n", iret);
    		}
			break;
		}

	}
	close(use_sock_fd);
	return NULL;
}
