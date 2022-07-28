/*
Trabalho realizado por:
Alexandre Godinho, a21804766
Bernardo Roque, a22103831
Francisco Nunes, a22102747
*/

#ifndef SYNCHRONIZATION_H_GUARD
#define SYNCHRONIZATION_H_GUARD

#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include "synchronization.h"
	
// Nomes usados na criacao dos semaforos
#define STR_SEM_MAIN_CLI_FULL 	"sem_main_cli_full"
#define STR_SEM_MAIN_CLI_EMPTY 	"sem_main_cli_empty"
#define STR_SEM_MAIN_CLI_MUTEX 	"sem_main_cli_mutex"
#define STR_SEM_CLI_PRX_FULL 	"sem_cli_prx_full"
#define STR_SEM_CLI_PRX_EMPTY 	"sem_cli_prx_empty"
#define STR_SEM_CLI_PRX_MUTEX 	"sem_cli_prx_mutex"
#define STR_SEM_PRX_SRV_FULL	"sem_prx_srv_full"
#define STR_SEM_PRX_SRV_EMPTY 	"sem_prx_srv_empty"
#define STR_SEM_PRX_SRV_MUTEX 	"sem_prx_srv_mutex"
#define STR_SEM_SRV_CLI_FULL	"sem_srv_cli_full"
#define STR_SEM_SRV_CLI_EMPTY 	"sem_srv_cli_empty"
#define STR_SEM_SRV_CLI_MUTEX 	"sem_srv_cli_mutex"
#define STR_SEM_RESULTS_MUTEX	"sem_results_mutex"

//estrutura de 3 semáforos utilizada no modelo produtor/consumidor
struct prodcons {
	sem_t *full, *empty, *mutex;
};

//estrutura que agrega informação de todos os semáforos necessários pelo socps
struct semaphores {
	struct prodcons *main_cli;	// semáforos para acesso ao buffer entre a main e clientes
	struct prodcons *cli_prx; 	// semáforos para acesso ao buffer entre clientes e proxies
	struct prodcons *prx_srv; 	// semáforos para acesso ao buffer entre proxies e servidores
	struct prodcons *srv_cli;	// semáforos para acesso ao buffer entre servidores e clientes
	sem_t *results_mutex;		// semáforo para exclusão mútua no acesso ao array de resultados
};

sem_t *semaphore_create(char* name, int value){
    sem_t *semaphore;
    semaphore = sem_open(name, O_CREAT, 0xFFFFFFFF, value);
    if(semaphore == SEM_FAILED){
        perror("full");
        exit(1);
    }
    return semaphore;
}

void semaphore_destroy(char* name, sem_t* semaphore){
    if (sem_close(semaphore) == -1){
        perror("");
    }
    if (sem_unlink(name) == -1){
        perror("");
    }
}

void produce_begin(struct prodcons* pc){
    sem_wait(pc->empty);
    sem_wait(pc->mutex);
}

void produce_end(struct prodcons* pc){
    sem_post(pc->mutex);
    sem_post(pc->full);
}

void consume_begin(struct prodcons* pc){
    sem_wait(pc->full);
    sem_wait(pc->mutex);
}

void consume_end(struct prodcons* pc){
    sem_post(pc->mutex);
    sem_post(pc->empty);
}

void semaphore_mutex_lock(sem_t* sem){
    sem_wait(sem);
}

void semaphore_mutex_unlock(sem_t* sem){
    sem_post(sem);
}

#endif