/*
Trabalho realizado por:
Alexandre Godinho, a21804766
Bernardo Roque, a22103831
Francisco Nunes, a22102747
*/

#ifndef MAIN_H_GUARD
#define MAIN_H_GUARD


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include "main.h"
#include "memory.h"
#include "synchronization.h"
#include "sosignal.h"
#include "log.h"
#include "sotime.h"

struct main_data *data;
struct communication_buffers *buffers;
struct semaphores *sems;

struct main_data {
	char *log_filename; //Nome do ficheiro de logs

	int max_ops;		//Integer
	int buffers_size;	//Integer
	
	int n_clients;		//Integer
	int n_proxies;		//Integer
	int n_servers;		//Integer
	
	int *client_pids;	//process ids de clientes
	int *proxy_pids;	//process ids de proxies
	int *server_pids;	//process ids de servidores
	
	int *client_stats;	//nº de operações recebidas por cada cliente
	int *proxy_stats;	//nº de operações encaminhadas por cada proxy
	int *server_stats;	//nº de operações respondidas por cada servidor
	
	struct operation* results;	//array com histórico de ops executadas
	
	int *terminate; //flag booleana, valor 1 indica que sovac deve terminar a sua execução
};

void main_args(int argc, char* argv[], struct main_data* data){
	if(argc	==	2){
		config(argv[1], data);
	}else{
		printf("Wrong program Usage.\n"
		"Usage format: sovac config_filename\n"
		"Usage example: ./bin/sovac configuration.txt\n");
		exit(0);
	}
}

void create_dynamic_memory_buffers(struct main_data* data){
	data->client_pids	= create_dynamic_memory(data->buffers_size);
	data->proxy_pids	= create_dynamic_memory(data->buffers_size);
	data->server_pids	= create_dynamic_memory(data->buffers_size);

	data->client_stats	= create_dynamic_memory(data->buffers_size);
	data->proxy_stats	= create_dynamic_memory(data->buffers_size);
	data->server_stats	= create_dynamic_memory(data->buffers_size);

	printf("		[Dynamic memory buffers created...]\n");
}

void create_shared_memory_buffers(struct main_data* data, struct communication_buffers* buffers){
	data->results	= create_shared_memory(STR_SHM_RESULTS, data->buffers_size);
	data->terminate	= create_shared_memory(STR_SHM_TERMINATE, data->buffers_size);

	buffers->main_cli->op	= create_shared_memory(STR_SHM_MAIN_CLI_BUFFER, data->buffers_size);
	buffers->cli_prx->op	= create_shared_memory(STR_SHM_CLI_PRX_BUFFER, data->buffers_size);
	buffers->prx_srv->op	= create_shared_memory(STR_SHM_PRX_SRV_BUFFER, data->buffers_size);
	buffers->srv_cli->op	= create_shared_memory(STR_SHM_SRV_CLI_BUFFER, data->buffers_size);

	buffers->main_cli->ptr		= create_shared_memory(STR_SHM_MAIN_CLI_PTR, data->buffers_size);
	buffers->cli_prx->in_out	= create_shared_memory(STR_SHM_CLI_PRX_PTR, 2);
	buffers->prx_srv->ptr		= create_shared_memory(STR_SHM_PRX_SRV_PTR, data->buffers_size);
	buffers->srv_cli->in_out	= create_shared_memory(STR_SHM_SRV_CLI_PTR, 2);

	printf("		[Shared memory buffers created...]\n");
}

void create_semaphores(struct main_data* data, struct semaphores* sems){
	sems->main_cli->full	= semaphore_create(STR_SEM_MAIN_CLI_FULL, 0);
	sems->main_cli->empty	= semaphore_create(STR_SEM_MAIN_CLI_EMPTY, sizeof(data->terminate));
	sems->main_cli->mutex	= semaphore_create(STR_SEM_MAIN_CLI_MUTEX, 1);

	sems->cli_prx->full		= semaphore_create(STR_SEM_CLI_PRX_FULL, 0);
	sems->cli_prx->empty	= semaphore_create(STR_SEM_CLI_PRX_EMPTY, sizeof(data->terminate));
	sems->cli_prx->mutex	= semaphore_create(STR_SEM_CLI_PRX_MUTEX, 1);

	sems->prx_srv->full		= semaphore_create(STR_SEM_PRX_SRV_FULL, 0);
	sems->prx_srv->empty	= semaphore_create(STR_SEM_PRX_SRV_EMPTY, sizeof(data->terminate));
	sems->prx_srv->mutex	= semaphore_create(STR_SEM_PRX_SRV_MUTEX, 1);

	sems->srv_cli->full		= semaphore_create(STR_SEM_SRV_CLI_FULL, 0);
	sems->srv_cli->empty	= semaphore_create(STR_SEM_SRV_CLI_EMPTY, sizeof(data->terminate));
	sems->srv_cli->mutex	= semaphore_create(STR_SEM_SRV_CLI_MUTEX, 1);

	sems->results_mutex		= semaphore_create(STR_SEM_RESULTS_MUTEX, 1);

	printf("		[Semaphores created...]\n");
}

void launch_processes(struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems){
	int i;
	for(i=0; i<data->n_clients; i++){
		data->client_pids[i] = launch_process(i, 0, buffers,data, sems);
	}
	for(i=0; i<data->n_proxies; i++){
		data->proxy_pids[i] = launch_process(i, 1, buffers, data, sems);
	}
	for(i=0; i<data->n_servers; i++){
		data->server_pids[i] = launch_process(i, 2, buffers, data, sems);
	}

	printf("		[Processes launched...]\n");
}

void user_interaction(struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems){
	int counter = 0;
	while(*data->terminate == 0){
		char option[20];
		printf("Please insert a command:\n");
		scanf("%s",option);
		if(strcmp(option,"op") == 0 && *data->terminate == 0){
			create_request(&counter, buffers, data, sems);
		}else if(strcmp(option,"read") == 0 && *data->terminate == 0){
			read_answer(data, sems);
		}else if(strcmp(option,"stop") == 0 && *data->terminate == 0){
			stop_execution(data, buffers, sems);
		}else if(strcmp(option,"help") == 0 && *data->terminate == 0){
			printf("The available commands are:\n"
			"		  op - Creates a new request;\n"
			"		read - Attempts to read the response to a request;\n"
			"		stop - Stops the execution of this program;\n"
			"		help - Prints the available commands\n");
		}else if(*data->terminate == 0){
			printf("Command not recognized, type 'help' for assistance.\n");
		}
	}
}

void create_request(int* op_counter, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems){
	user_interaction_log(data->log_filename, "op", -1);
	produce_begin(sems->main_cli);
	if(*op_counter < data->max_ops){
		struct operation *op;
		op = create_dynamic_memory(sizeof(struct operation));
		get_timespec(op->start_time);
		op->id = *op_counter;
		printf("The operation %d was created.\n", *op_counter);
		write_rnd_access_buffer(buffers->main_cli, data->buffers_size, op);
		printf("		[Operation sent from main to client...]\n");
		(*op_counter)++;
	}else{
		printf("The maximum limit for operations has been reached!\n");
	}
	produce_end(sems->main_cli);
}

void read_answer(struct main_data* data, struct semaphores* sems){
	int answer;
	int i = 0;
	printf("Please insert the operation ID:\n");
	scanf("%d", &answer);
	user_interaction_log(data->log_filename, "read", answer);
	while(i < data->max_ops){
		semaphore_mutex_unlock(sems->results_mutex);
		if(data->results[i].id == answer && data->results[i].status == 'S'){
			printf("The operation %d with status %c was received client %d, forwarded by proxy %d, and served by server %d!\n",
			answer,data->results[i].status,data->results[i].client,data->results[i].proxy,data->results[i].server);
			i = data->max_ops;
		}else{
			i++;
			if(i == data->max_ops){
				printf("The asked operation doesn't exist!\n");
			}
		}
		semaphore_mutex_lock(sems->results_mutex);
	}
}

void stop_execution(struct main_data* data, struct communication_buffers* buffers, struct semaphores* sems){
	user_interaction_log(data->log_filename, "stop", -1);
	*data->terminate = 1;
	wakeup_processes(data, sems);
	wait_processes(data);
	write_statistics(data);
	destroy_dynamic_memory_buffers(data);
	destroy_shared_memory_buffers(data, buffers);
	destroy_semaphores(sems);
}

void wakeup_processes(struct main_data* data, struct semaphores* sems){
	produce_end(sems->main_cli);
	produce_end(sems->cli_prx);
	produce_end(sems->prx_srv);
}

void wait_processes(struct main_data* data){
	int i;
	for(i = 0; i < data->n_clients; i++){
		data->client_stats[i] = wait_process(data->client_pids[i]);
	}
	for(i = 0; i < data->n_proxies; i++){
		data->proxy_stats[i] = wait_process(data->proxy_pids[i]);
	}
	for(i = 0; i < data->n_servers; i++){
		data->server_stats[i] = wait_process(data->server_pids[i]);
	}
}

void write_statistics(struct main_data* data){
	int counter;
	for(counter = 0; counter < data->n_clients; counter++){
		printf("The client %d processed %d operations.\n", counter, data->client_stats[counter]);
	}
	for(counter = 0; counter < data->n_proxies; counter++){
		printf("The proxy %d processed %d operations.\n", counter, data->proxy_stats[counter]);
	}
	for(counter = 0; counter < data->n_servers; counter++){
		printf("The server %d processed %d operations.\n", counter, data->server_stats[counter]);
	}
}

void destroy_dynamic_memory_buffers(struct main_data* data){
	destroy_dynamic_memory(data->client_pids);
	destroy_dynamic_memory(data->proxy_pids);
	destroy_dynamic_memory(data->server_pids);

	destroy_dynamic_memory(data->client_stats);
	destroy_dynamic_memory(data->proxy_stats);
	destroy_dynamic_memory(data->server_stats);

	printf("		[Dynamic memory buffers destroyed...]\n");
}

void destroy_shared_memory_buffers(struct main_data* data, struct communication_buffers* buffers){
	destroy_shared_memory(STR_SHM_RESULTS, data->results, data->buffers_size);
	destroy_shared_memory(STR_SHM_TERMINATE, data->terminate, data->buffers_size);
	
	destroy_shared_memory(STR_SHM_MAIN_CLI_BUFFER, buffers->main_cli->op, data->buffers_size);
	destroy_shared_memory(STR_SHM_CLI_PRX_BUFFER, buffers->cli_prx->op, data->buffers_size);
	destroy_shared_memory(STR_SHM_PRX_SRV_BUFFER, buffers->prx_srv->op, data->buffers_size);
	destroy_shared_memory(STR_SHM_SRV_CLI_BUFFER, buffers->srv_cli->op, data->buffers_size);

	destroy_shared_memory(STR_SHM_MAIN_CLI_PTR, buffers->main_cli->ptr, data->buffers_size);
  	destroy_shared_memory(STR_SHM_CLI_PRX_PTR, buffers->cli_prx->in_out, 2);
	destroy_shared_memory(STR_SHM_PRX_SRV_PTR, buffers->prx_srv->ptr, data->buffers_size);
	destroy_shared_memory(STR_SHM_SRV_CLI_PTR, buffers->srv_cli->in_out, 2);

	printf("		[Shared memory buffers destroyed...]\n");
}

void destroy_semaphores(struct semaphores* sems){
	semaphore_destroy(STR_SEM_MAIN_CLI_FULL, sems->main_cli->full);
	semaphore_destroy(STR_SEM_MAIN_CLI_EMPTY, sems->main_cli->empty);
	semaphore_destroy(STR_SEM_MAIN_CLI_MUTEX, sems->main_cli->mutex);

	semaphore_destroy(STR_SEM_CLI_PRX_FULL, sems->cli_prx->full);
	semaphore_destroy(STR_SEM_CLI_PRX_EMPTY, sems->cli_prx->empty);
	semaphore_destroy(STR_SEM_CLI_PRX_MUTEX, sems->cli_prx->mutex);

	semaphore_destroy(STR_SEM_PRX_SRV_FULL, sems->prx_srv->full);
	semaphore_destroy(STR_SEM_PRX_SRV_EMPTY, sems->prx_srv->empty);
	semaphore_destroy(STR_SEM_PRX_SRV_MUTEX, sems->prx_srv->mutex);
	
	semaphore_destroy(STR_SEM_SRV_CLI_FULL, sems->srv_cli->full);
	semaphore_destroy(STR_SEM_SRV_CLI_EMPTY, sems->srv_cli->empty);
	semaphore_destroy(STR_SEM_SRV_CLI_MUTEX, sems->srv_cli->mutex);

	semaphore_destroy(STR_SEM_RESULTS_MUTEX, sems->results_mutex);

	printf("		[Semaphores destroyed...]\n");
}

void sigint_handler(int signal){
	stop_execution(data, buffers, sems);
	exit(0);
}

int main(int argc,char *argv[]){
	/* Variáveis utilizadas ao longo do SOVAC */
	static int data_created 	= 0;	//flag booleana que irá indicar se o programa criou a struct 'data'
	static int buffers_created 	= 0;	//flag booleana que irá indicar se o programa criou a struct 'buffers'
	static int sems_created 	= 0;	//flag booleana que irá indicar se o programa criou a struct 'semaphores'

	/* Criação de memória necessária à execução do programa */
	if(data_created == 0){
		data 				= create_dynamic_memory(sizeof(struct main_data));
		data->log_filename	= create_dynamic_memory(sizeof(char *));
		data_created = 1;
	}
	if(buffers_created == 0){
		buffers 			= create_dynamic_memory(sizeof(struct communication_buffers));
		buffers->main_cli 	= create_dynamic_memory(sizeof(struct rnd_access_buffer));
 		buffers->cli_prx 	= create_dynamic_memory(sizeof(struct circular_buffer));
 		buffers->prx_srv 	= create_dynamic_memory(sizeof(struct rnd_access_buffer));
		buffers->srv_cli 	= create_dynamic_memory(sizeof(struct circular_buffer));
		buffers_created 	= 1;
	}
	if(sems_created == 0){
		sems			= create_dynamic_memory(sizeof(struct semaphores));
		sems->main_cli	= create_dynamic_memory(sizeof(struct prodcons));
		sems->cli_prx	= create_dynamic_memory(sizeof(struct prodcons));
		sems->prx_srv	= create_dynamic_memory(sizeof(struct prodcons));
		sems->srv_cli	= create_dynamic_memory(sizeof(struct prodcons));
		sems_created	= 1;
	}

	/* Execução do código principal */
	main_args(argc, argv, data);
	create_dynamic_memory_buffers(data);
	create_shared_memory_buffers(data, buffers);
	create_semaphores(data, sems);
	launch_processes(buffers, data, sems);
	signal(SIGINT, sigint_handler);
	user_interaction(buffers, data, sems);
	
	/* Libertação final da memória*/
	if(sems_created == 1){
		destroy_dynamic_memory(sems->main_cli);
 		destroy_dynamic_memory(sems->cli_prx);
 		destroy_dynamic_memory(sems->prx_srv);
 		destroy_dynamic_memory(sems->srv_cli);
		destroy_dynamic_memory(sems);
	}
	if(buffers_created == 1){
 		destroy_dynamic_memory(buffers->main_cli);
 		destroy_dynamic_memory(buffers->cli_prx);
 		destroy_dynamic_memory(buffers->prx_srv);
 		destroy_dynamic_memory(buffers->srv_cli);
 		destroy_dynamic_memory(buffers);
	}
	if(data_created == 1){
 		destroy_dynamic_memory(data);
	}
	return 0;
} 

#endif