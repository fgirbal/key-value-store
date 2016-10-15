#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/un.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

/******************************************************************************
*
* File Name: list.h
*
* Author:    Miguel Cardoso e Francisco Eiras
*
* DESCRIPTION
*
*     Function prototypes for handling lists in a semi-abstract manner,
*		defined in list.c
*
*****************************************************************************/

/***************************************************************

	Basic struct for the list, containing a key, a data 
		"string", its size and a pointer to the next node.

***************************************************************/

typedef struct node
{
	uint32_t key;
	char * value;
	int value_length;
	struct node * next;
} node;


/***************************************************************

	This function creates a new list.

	Input: Nothing.
	Output: A pointer to a node struct.

***************************************************************/
node * createList();


/***************************************************************

	This function adds data to a certain key, taking into
		account overwrite value.

	Input: Pointer to start of list, key, data, data size,
				overwrite.
	Output: 0 for successful write, -2 if overwrite was 0 and
				the node was populated.

***************************************************************/
int addToList(node * first, uint32_t key, char * value, int value_length, int overwrite);


/***************************************************************

	This function searches for key, in the list,
		puts saved data into value.

	Input: Pointer to start of list, key, pointer to data
			buffer.
	Output: Size of data stored for a successful read
				-1 for unsuccessful read.

***************************************************************/
int getFromList(node * first, uint32_t key, char ** value);


/***************************************************************

	This function finds the key in the list, and deletes it.

	Input: Pointer to start of list, key.
	Output: 0 for successful delete, -1 for unsuccessful 
				delete.

***************************************************************/
int deleteFromList(node * first, uint32_t key);


/***************************************************************

	This function travels through the list, freeing it from
		memory.

	Input: Pointer to start of list.
	Output: Nothing.

***************************************************************/
void deleteList(node * first);

/***************************************************************

	This function travels through the list, printing all
		information in each node, for debug purposes.

	Input: A numeric key.
	Output: Nothing, (information in stdout).

***************************************************************/
void printList(node * first);


/***************************************************************

	This function travels through the list, writing down all
		information in each node to the file described by fd.

	Input: Pointer to start of list, file descriptor obtained
				through open.
	Output: Nothing.

***************************************************************/
void snapList(node * first, int fd);