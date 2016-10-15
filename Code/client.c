#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "psiskv.h"

int main(int argc, char const *argv[])
{
	int kv_descriptor, ret_value;
	char dados[100];
	char novosdados[100];

	if(argc < 2){
		printf("Usage: ./client [port]\n");
		return 0;
	}

	kv_descriptor = kv_connect("127.0.0.1", atoi(argv[1]));

	strcpy(dados, "100");
	kv_write(kv_descriptor, (uint32_t) 3002, dados, strlen(dados), 0);

	//Ler de uma chave não inicializada
	ret_value = kv_read(kv_descriptor, (uint32_t) 3001, novosdados, 100);
	if(ret_value < 0)
		printf("problem with reading\n");
	else
		printf("%d, %s\n", ret_value, novosdados);

	//Ler de uma chave inicializada
	ret_value = kv_read(kv_descriptor, (uint32_t) 3002, novosdados, 100);
	if(ret_value < 0)
		printf("problem with reading\n");
	else
		printf("%d, %s\n", ret_value, novosdados);

	kv_close(kv_descriptor);
	return 0;
}