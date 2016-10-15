#include "front-aux.h"

static void sig_ignore(int signo)
{
	return;
}

static void sig_handler(int signo)
{
	status = 1;
	sleep(2);
	shutdown(sock_fd, SHUT_RDWR);
	close(sock_fd);
	exit(0);
}

int main(int argc, char const *argv[]) {
	//set program to handle SIGHUP, SIGCHLD and SIGINT
	signal(SIGHUP, sig_ignore);
	signal(SIGCHLD, sig_ignore);
	signal(SIGINT, sig_handler);

	struct sockaddr_in server_addr;
	int iret;
	int err = -1;
	int port = 9999;

	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	
	if (sock_fd == -1)
	{
		perror("socket: ");
		exit(-1);
	}
	
	server_addr.sin_family = AF_INET;
	inet_aton("127.0.0.1", &server_addr.sin_addr);

	//Cycles ports starting at 9999 for an available one.
	while(err == -1)
	{
		server_addr.sin_port = htons(port);
		err = bind(sock_fd, (struct sockaddr *) &server_addr, sizeof(server_addr));
		port++;
	}
	
	port--;

	//This if signals whether the instance of front-server is product of a fork or was initialized by the user.
	//It then informs 
	if(argc == 2)
	{
		int aux_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
		struct sockaddr_in aux_server_addr;
		if(aux_sock_fd == -1)
		{
			perror("Error creating socket for front server communication.");
			exit(0);
		}
		
		aux_server_addr.sin_family = AF_INET;
		inet_aton("127.0.0.1", &aux_server_addr.sin_addr);
		int data_server_port;
		sscanf(argv[1], "%d", &data_server_port);

    	aux_server_addr.sin_port = htons(data_server_port);
    	
		err = connect(aux_sock_fd, (struct sockaddr*) &aux_server_addr, sizeof(aux_server_addr));
		if(err == -1)
		{
			perror("Died while connecting to data server.\n");
			exit(-1);
		}
		
		send(aux_sock_fd, &port, sizeof(port), 0);
		
		close(aux_sock_fd);
	}

	listen(sock_fd, 20);

	pthread_t front_server_thread[2];
	iret = pthread_create(&front_server_thread[0], NULL, front_server_threads, NULL);

	if(iret != 0)
	{
		fprintf(stderr,"Error - pthread_create() return code: %d\n",iret);
		exit(EXIT_FAILURE);
	}

	iret = pthread_create(&front_server_thread[1], NULL, front_server_threads, NULL);
	if(iret != 0)
	{
		fprintf(stderr,"Error - pthread_create() return code: %d\n",iret);
		exit(EXIT_FAILURE);
	}

	char buffer[100];

	//Reads keyboard if the program is attached to a terminal, otherwise acts as an active wait.
	while(1)
	{
		if(fgets(buffer, 100, stdin) != NULL)
		{
			if(strcmp(buffer, "quit\n") == 0)
			{
				status = 1;
				sleep(2);
				break;
			}
		}
	}
	close(sock_fd);
	return 0;
}