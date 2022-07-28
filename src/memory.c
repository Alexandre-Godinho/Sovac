/*
Trabalho realizado por:
Alexandre Godinho, a21804766
Bernardo Roque, a22103831
Francisco Nunes, a22102747
*/

#ifndef MEMORY_H_GUARD
#define MEMORY_H_GUARD

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "memory-private.h"
#include "memory.h"

// Nomes usados na criação de zonas de memoria partilhada
#define STR_SHM_MAIN_CLI_PTR 			"SHM_MAIN_CLI_PTR"
#define STR_SHM_MAIN_CLI_BUFFER 		"SHM_MAIN_CLI_BUFFER"
#define STR_SHM_CLI_PRX_PTR 			"SHM_CLI_PRX_PTR"
#define STR_SHM_CLI_PRX_BUFFER 			"SHM_CLI_PRX_BUFFER"
#define STR_SHM_PRX_SRV_PTR 			"SHM_PRX_SRV_PTR"
#define STR_SHM_PRX_SRV_BUFFER 			"SHM_PRX_SRV_BUFFER"
#define STR_SHM_SRV_CLI_PTR 			"SHM_SRV_CLI_PTR"
#define STR_SHM_SRV_CLI_BUFFER			"SHM_SRV_CLI_BUFFER"
#define STR_SHM_RESULTS					"SHM_RESULTS"
#define STR_SHM_TERMINATE				"SHM_TERMINATE"

struct operation {
	int id; 		//Integer
	char status;	//Char
	int client;		//Integer
	int proxy; 		//Integer
	int server;		//Integer
    struct timespec start_time;     //quando a op foi iniciada
    struct timespec client_time;    //quando o cliente processou a op
    struct timespec proxy_time;     //quando o proxy processou a op
    struct timespec server_time;    //quando o server processou a op
    struct timespec end_time;       //quando a op foi concluída
};

struct communication_buffers {
	struct rnd_access_buffer* main_cli; //buffer para main fazer pedidos a clientes
	struct circular_buffer* cli_prx;	//buffer para clientes enviarem pedidos a proxies
	struct rnd_access_buffer* prx_srv;  //buffer para proxies encaminharem pedidos a servidores
	struct circular_buffer* srv_cli;	//buffer para servidores responderem a pedidos de clientes
};

void* create_shared_memory(char* name, int size){
	int *ptr;
    int ret;
	int fd = shm_open(name, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	if(fd == -1){
		perror("shm");
		exit(1);
	}

    ret = ftruncate(fd, size);
	if(ret == -1){
		perror("shm");
		exit(2);
	}

    ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(ptr == MAP_FAILED){
		exit(3);
	}
    return ptr;
}

void* create_dynamic_memory(int size){
    void* ptr = calloc(size, sizeof(void));
	return ptr;
}

void destroy_shared_memory(char* name, void* ptr, int size){
	int ret = munmap(ptr,size*sizeof(int));
    if (ret == -1){
        perror("/shm");
        exit(7);
    }

	ret = shm_unlink(name);
    if (ret == -1){
        perror("/shm");
        exit(8);
    }
}

void destroy_dynamic_memory(void* ptr){
	free(ptr);
}

void write_rnd_access_buffer(struct rnd_access_buffer* buffer, int buffer_size, struct operation* op){
    int n;
    for(n = 0; n < buffer_size; n++){
        if(buffer->ptr[n] == 0){
            buffer->op[n] = *op;
            buffer->ptr[n] = 1;
            break;
        }
    }
}

void write_circular_buffer(struct circular_buffer* buffer, int buffer_size, struct operation* op){
    while(((buffer->in_out[0] + 1) % buffer_size) == buffer->in_out[1]){
        printf("Buffer cheio\n");
    }
    buffer->op[buffer->in_out[0]] = *op;
    buffer->in_out[0] = (buffer->in_out[0] + 1) % buffer_size;
}

void read_rnd_access_buffer(struct rnd_access_buffer* buffer, int buffer_size, struct operation* op){
    int n = 0;
    while(n < buffer_size){
        if(buffer->ptr[n] == 1){
            *op = buffer->op[n];
            buffer->ptr[n] = 0;
            n = buffer_size;
        }else{
            n++;
        }
    }
}

void read_circular_buffer(struct circular_buffer* buffer, int buffer_size, struct operation* op){
    while(buffer->in_out[0] == buffer->in_out[1]){
        printf("Buffer vazio\n");
    }
    *op = buffer->op[buffer->in_out[1]];
    buffer->in_out[1] = (buffer->in_out[1] + 1) % buffer_size;
}

#endif