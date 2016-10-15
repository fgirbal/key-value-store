#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "psiskv.h"
#define MAX_COUNT 100

int main(int argc, char const *argv[])
{
	int ret_value, i;
	int port = 9999;
	char dados[100];
	char novosdados[100];

	//Dois processos competem pelas escritas no mesmo espaço (como clientes separados), sempre com kv_overwrite = 1
	if(!fork()){
		int kv_descriptor = -1;

		while(kv_descriptor == -1)
		{
			kv_descriptor = kv_connect("127.0.0.1", port);
			port++;
			if(port > 10009)
				port = 9999;
		}

		port--;

		//O processo filho escreve valores i + MAX_COUNT
		for(i = 0; i < MAX_COUNT; i++)
		{
			sprintf(dados, "%d", MAX_COUNT + i);
			kv_write(kv_descriptor, (uint32_t) i, dados, strlen(dados)+1, 1);
		}

		kv_close(kv_descriptor);

		return 0;
	}

	int kv_descriptor = -1;

	while(kv_descriptor == -1)
	{
		kv_descriptor = kv_connect("127.0.0.1", port);
		port++;
		if(port > 10009)
				port = 9999;
	}

	port--;
	
	//O processo pai escreve valores i
	for(i = 0; i < MAX_COUNT; i++)
	{
		sprintf(dados, "%d", i);
		kv_write(kv_descriptor, (uint32_t) i, dados, strlen(dados)+1, 1);
	}

	//O processo pai espera que o filho acabe (para o caso de acabar primeiro)
	wait(NULL);

	//E agora lê-se os valores escritos por ambos
	for(i = 0; i < MAX_COUNT; i++)
	{
		ret_value = kv_read(kv_descriptor, (uint32_t) i, novosdados, 100);
		if(ret_value >= 0)
			printf("%s\n", novosdados);
	}
	
	kv_close(kv_descriptor);

	return 0;
}
