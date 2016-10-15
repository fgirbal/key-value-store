#include "data-aux.h"

static void sig_ignore(int signo)
{
	return;
}

void sigquit()
{
	exit(0);
}

void alarmhandler(int a)
{
	save_structure();
	alarm(SAVE_TIME);
}

int main(int argc, char const *argv[])
{
	signal(SIGHUP, sig_ignore);
	signal(SIGCHLD, sig_ignore);

	struct sockaddr_in server_addr;
	int j;
	int err = -1;
	int port = ds_port;

	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	
	if (sock_fd == -1)
	{
		perror("socket: ");
		exit(-1);
	}

	if(argc == 2)
		send_to_front = 1;
	
	server_addr.sin_family = AF_INET;
	inet_aton("127.0.0.1", &server_addr.sin_addr);

	//Save own address to relay to the front-server
	ds_ip = inet_ntoa(server_addr.sin_addr);

	while(err == -1)
	{
		server_addr.sin_port = htons(port);
		
		err = bind(sock_fd, (struct sockaddr *) &server_addr, sizeof(server_addr));
		port--;
	}

	port++;
	ds_port = port;

	hashtable = (node **) malloc(NUMBER_BUCKETS*sizeof(node*));
	for(j = 0; j < NUMBER_BUCKETS; j++){
		hashtable[j] = createList();
	}

	structure_recovery();
	log_recovery();

	listen(sock_fd, 20);
	int iret;
	n_empty = 0;


	for(j = 0; j < NUMBER_BUCKETS; j++){
		if (pthread_mutex_init(&lock[j], NULL) != 0)
	    {
	        return -1;
	    }
    }

    //Create thread for backups
    pthread_t back_thread;
    iret = pthread_create(&back_thread, NULL, thread_backup, NULL);
    if(iret)
    	fprintf(stderr, "No thread creating backups, pthread_create() return code: %d\n", iret);

    pthread_t front_server;
    iret = pthread_create(&front_server, NULL, front_server_communication, NULL);
    if(iret)
    {
    	fprintf(stderr, "No thread creating backups, pthread_create() return code: %d\n", iret);
    }

	//Create first thread pool
	pthread_t first_pool[SIZE_OF_POOL];

	for(j = 0; j < SIZE_OF_POOL; j++)
	{
		iret = pthread_create(&first_pool[j], NULL, thread_dealing, NULL);
		n_empty++;
		if(iret)
		{
			fprintf(stderr,"Error - pthread_create() return code: %d\n",iret);
			exit(EXIT_FAILURE);
		}
	}

	while(1);

	for(j = 0; j < NUMBER_BUCKETS; j++)
		deleteList(hashtable[j]);

	close(sock_fd);
	exit(0);
}