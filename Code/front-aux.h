/******************************************************************************
*
* File Name: front-aux.h
*
* Author:    Miguel Cardoso e Francisco Eiras
*
*
* SYNOPSIS
*
*		#include "servers.h"
*
* DESCRIPTION
*
*      Functions protoypes of functions declared in front-aux.c, as well
*			as global variables used by all functions.
*
*****************************************************************************/


#ifndef FRONTAUX
#define FRONTAUX

#include "servers.h"

int sock_fd;
int ds_data_in;
char ds_ip[50];
int ds_port;
int status, stop;

/***************************************************************

	This function creates the control flow for the front
		server to connect to clients and the data server.

	Input: Nothing (Works on global variable)
	Output: Nothing (Writes to file)

***************************************************************/
void *front_server_threads (void *ptr);

#endif