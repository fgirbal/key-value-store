#include "front-aux.h"

int ds_data_in = 0;
int status, stop = 0;

void *front_server_threads (void *ptr)
{
	struct sockaddr_in client_addr;
	socklen_t size_addr;
	int nbytes, handshake;

	while(1)
	{
		handshake = -2;
	    size_addr = sizeof(client_addr);

		int new_fd = accept(sock_fd,(struct sockaddr *) &client_addr, &size_addr);
		nbytes = recv(new_fd, &handshake, sizeof(handshake), 0);
		if(nbytes == 0)
			continue;

		if(handshake == DATA_SERVER && ds_data_in == 0)
		{
			handshake = 2;
			send(new_fd, &handshake, sizeof(handshake), 0);
			//Contact with front-server was made by data-server
			ds_data_in = 1;
			
			recv(new_fd, ds_ip, 50, 0);
			recv(new_fd, &ds_port, sizeof(ds_port), 0);

			while(1)
			{
				sleep(1);
				if(stop == 1)
					break;
				if(send(new_fd, &status, sizeof(status), MSG_NOSIGNAL) == -1)
				{
					ds_data_in = 0;
					if(!fork())
					{
						char * buffer[] = {"./data-server", "1", 0};
						execve(buffer[0], buffer, NULL);
					}
					else
						exit(0);
				}
			}
		} 
		else if(handshake == CLIENT && ds_data_in != 0)
		{
			handshake = 2;
			send(new_fd, &handshake, sizeof(handshake), 0);
			send(new_fd, ds_ip, 50, 0);
			send(new_fd, &ds_port, sizeof(ds_port), 0);
		} 
		else 
		{
			handshake = ERROR;
			send(new_fd, &handshake, sizeof(handshake), 0);
		}

		close(new_fd);
	}

	return NULL;
}