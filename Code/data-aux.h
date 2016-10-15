/******************************************************************************
*
* File Name: data-aux.h
*
* Author:    Miguel Cardoso e Francisco Eiras
*
*
* SYNOPSIS
*
*		#include "servers.h"
*		#include "list.h"
*
* DESCRIPTION
*
*      Functions protoypes of functions declared in data-aux.c, as well
*			as global variables used by all functions.
*
*****************************************************************************/


#ifndef DATAAUX
#define DATAAUX

#include "servers.h"
#include "list.h"

#define SIZE_OF_POOL 10
#define NUMBER_BUCKETS 199
#define SAVE_TIME 120

int sock_fd;
int n_empty;
node ** hashtable;
pthread_mutex_t lock[NUMBER_BUCKETS];
uint32_t interval;
int killbackup;
int send_to_front;
int port;
int ds_port;
char * ds_ip;

/***************************************************************

	This function turns a key into a bucket for storage.

	Input: A numeric key.
	Output: A bucket.

***************************************************************/
int hash_function(uint32_t key);
/***************************************************************

	This function saves all data in the struct to a file.

	Input: Nothing (Reads from global variable)
	Output: Nothing (Writes to file)

***************************************************************/
void save_structure();
/***************************************************************

	This function recovers the last saved structure.

	Input: Nothing (Reads from file)
	Output: Nothing (Write to a global variable)

***************************************************************/
void structure_recovery();
/***************************************************************

	This function recovers from the last saved log.

	Input: Nothing (Reads from file)
	Output: Nothing (Writes to a global variable)

***************************************************************/
void log_recovery();
/***************************************************************

	This function saves all operations (except delete) 
		to the log.

	Input: Pointer to message struct, data related to this
			message.
	Output: Nothing (Writes to file)

***************************************************************/
void write_to_log(message * mensagem, char * value);
/***************************************************************

	This function creates the control flow for a thread 
		working on client connections.

	Input: Nothing
	Output: Nothing

***************************************************************/
void * thread_dealing (void *ptr);
/***************************************************************

	This function creates the control flow for a thread
		that will backup the structure at regular intervals.

	Input: Nothing
	Output: Nothing

***************************************************************/
void * thread_backup(void * ptr);
/***************************************************************

	This functions relaunches the front server after an
		unexpected break

	Input: Nothing
	Output: The new front server port

***************************************************************/
int relaunch_front_server();
/***************************************************************

	This function creates the control flow for a thread
		that will handle communiation with the front server.

	Input: Nothing
	Output: Nothing

***************************************************************/
void * front_server_communication(void * ptr);

#endif
