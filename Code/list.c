#include "list.h"

node * createList() 
{
	//Creates a list, the first node never contains information, acting merely as a pointer to node.
	node * first;
	first = (node *) malloc(sizeof(node));

	first->key = -1;
	first->value = NULL;
	first->value_length = 0;
	first->next = NULL;

	return first;
}

int addToList(node * first, uint32_t key, char * value, int value_length, int overwrite)
{
	node * aux;
	aux = first;

	//Checks whether the key is in the list
	while(aux->next != NULL) 
	{
		if(aux->key == key) 
		{
			break;
		}
		aux = aux->next;
	}

	//Adds if the key isn't already present
	if((aux->next == NULL) && (aux->key != key))
	{
		node * added;

		added = (node *) malloc(sizeof(node));
		added->key = key;
		added->value = (char *) malloc(value_length*sizeof(char));
		memcpy(added->value, value, value_length);
		added->value_length = value_length;
		added->next = NULL;
		aux->next = added;
		return 0;
	}

	//Found a node with the same key, checks for overwrite value
	if(overwrite == 1){
		free(aux->value);
		aux->value = (char *) malloc(value_length*sizeof(char));
		memcpy(aux->value, value, value_length);
		aux->value_length = value_length;
		return 0;
	} 
	else
	{
		return -2;
	}
}

int getFromList(node * first, uint32_t key, char ** value) 
{
	node * aux;
	//Checks if the list is populated
	if(first->next == NULL)
	{
		return -1;
	}
	aux = first->next;

	//Travels until key is found
	while(aux->next != NULL) 
	{
		if(aux->key == key) 
		{
			break;
		}
		aux = aux->next;
	}

	//If it reached the last one and it wasn't the wanted value, returns error
	if(aux->key != key) 
	{
		return -1;
	} 
	else //gets the data
	{
		*value = (char *) malloc(aux->value_length*sizeof(char));
		memcpy(*value, aux->value, aux->value_length);
		return aux->value_length;
	}
}

int deleteFromList(node * first, uint32_t key) 
{
	node * aux, * aux1;

	//Checks if the list is populated
	if(first->next != NULL)
		aux = first->next;
	else
		return -1;

	aux1 = first;

	//Travels through the list in search of key
	while(aux->next != NULL) 
	{
		if(aux->key == key) 
		{
			break;
		}
		aux = aux->next;
		aux1 = aux1->next;
	}

	//If it reached the last one and it's not key, returns error
	//Otherwise, frees it.
	if(aux->next == NULL)
	{
		if(aux->key != key)
			return -1;
		else
		{
			aux1->next = NULL;
			free(aux->value);
			free(aux);
			return 0;
		}
	} 
	else 
	{
		aux1->next = aux->next;
		free(aux->value);
		free(aux);
		return 0;
	}
}

void deleteList(node * first)
{
	node * aux, * aux1;
	aux = first;

	//Travels through the list, freeing every node.
	while(aux->next != NULL) 
	{
		aux1 = aux->next;
		free(aux->value);
		free(aux);
		aux = aux1;
	}

	free(aux);
}

void printList(node * first) 
{
	node * aux;
	aux = first;
	if(aux->next == NULL)
	{
		printf("Empty list\n");
		return;
	}

	aux = first->next;

	while(aux != NULL) 
	{
		printf("[%d: %s] ", aux->key, aux->value);
		aux = aux->next;
	}
	printf("\n");
}

void snapList(node * first, int fd)
{
	node * aux;
	aux = first;

	//Checks if the list is populated
	if(aux->next == NULL)
		return;

	aux = first->next;

	//Travels through the list, writing all fields in aux to fd.
	while(aux != NULL)
	{
		write(fd, &aux->key, sizeof(aux->key));
		write(fd, &aux->value_length, sizeof(aux->value_length));
		write(fd, aux->value, aux->value_length);

		aux = aux->next;
	}
}
