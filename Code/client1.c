#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "psiskv.h"
#define MAX_COUNT 20

int main(int argc, char const *argv[])
{
	int kv_descriptor = -1;
	int ret_value;
	int port = 9999;
	char dados[100], ip[100];
	char novosdados[100];

	strcpy(ip, "127.0.0.1");

	while(kv_descriptor == -1)
	{
		kv_descriptor = kv_connect(ip, port);
		port++;
	}

	port--;
	
	int i;
	//Escrever todos os inteiros nas posições
	for(i = 0; i < MAX_COUNT; i++)
	{
		sprintf(dados, "%d", i);
		kv_write(kv_descriptor, (uint32_t) i, dados, 100, 0);
	}

	//Re-escrever os inteiros nas posições MAX_COUNT/2 até MAX_COUNT para serem o simétrico
	for(i = MAX_COUNT/2; i < MAX_COUNT; i++)
	{
		sprintf(dados, "%d", MAX_COUNT - i);
		kv_write(kv_descriptor, (uint32_t) i, dados, 100, 1);
	}

	//Obter os novos inteiros
	for(i = 0; i < MAX_COUNT; i++)
	{
		ret_value = kv_read(kv_descriptor, (uint32_t) i, novosdados, 100);
		if(ret_value >= 0)
			printf("%s\n", novosdados);
	}

	//Eliminar todas as posições
	for(i = 0; i < MAX_COUNT; i++)
	{
		kv_delete(kv_descriptor, (uint32_t) i);
	}

	//Fechar a conexão
	kv_close(kv_descriptor);

	return 0;
}
