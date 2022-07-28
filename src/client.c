/*
Trabalho realizado por:
Alexandre Godinho, a21804766
Bernardo Roque, a22103831
Francisco Nunes, a22102747
*/

#ifndef CLIENT_H_GUARD
#define CLIENT_H_GUARD

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory-private.h"
#include "memory.h"
#include "main.h"
#include "client.h"
#include "synchronization.h"
#include "sotime.h"

int execute_client(int client_id, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems){
    while(1){
        struct operation* op;
        op = create_dynamic_memory(sizeof(struct operation));
        client_get_operation(op, buffers, data, sems);
        if(op->id != -1 && *data->terminate == 0){
            client_process_operation(op, client_id, &data->client_stats[client_id]);
            client_send_operation(op, buffers, data, sems);
        }else if(*data->terminate == 1){
            return data->client_stats[client_id];
        }
        client_receive_answer(op, buffers, data, sems);
        if(op->id != -1 && *data->terminate == 0){
            client_process_answer(op, data, sems);
        }else if(*data->terminate == 1){
            return data->client_stats[client_id];
        }
        destroy_dynamic_memory(op);
    }
}

void client_get_operation(struct operation* op, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems){
    consume_begin(sems->main_cli);
    if(*data->terminate == 1){
        return;
    }else{
        read_rnd_access_buffer(buffers->main_cli, data->buffers_size, op);
        printf("		[Client recieved operation...]\n");
    }
    consume_end(sems->main_cli);
}

void client_process_operation(struct operation* op, int client_id, int* counter){
    op->client  = client_id;
    op->status  = 'C';
    (*counter)++;
    get_timespec(op->client_time);
    printf("		[Client processed operation...]\n");
}

void client_send_operation(struct operation* op, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems){
    produce_begin(sems->cli_prx);
    write_circular_buffer(buffers->cli_prx, data->buffers_size, op);
    printf("		[Operation sent from client to proxy...]\n");
    produce_end(sems->cli_prx);
}

void client_receive_answer(struct operation* op, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems){
    consume_begin(sems->srv_cli);
    if(*data->terminate == 1){
        return;
    }else{
        read_circular_buffer(buffers->srv_cli, data->buffers_size, op);
        printf("		[Client recieved answer...]\n");
    }
    consume_end(sems->srv_cli);
}

void client_process_answer(struct operation* op, struct main_data* data, struct semaphores* sems){
    semaphore_mutex_unlock(sems->results_mutex);
    data->results[op->id] = *op;
    get_timespec(op->end_time);
    printf("		[Client processed answer...]\n");
    semaphore_mutex_lock(sems->results_mutex);
}

#endif