/*
Trabalho realizado por:
Alexandre Godinho, a21804766
Bernardo Roque, a22103831
Francisco Nunes, a22102747
*/

#ifndef SERVER_H_GUARD
#define SERVER_H_GUARD

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory.h"
#include "main.h"
#include "server.h"
#include "synchronization.h"

int execute_server(int server_id, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems){
    while(1){
        struct operation* op;
        op = create_dynamic_memory(sizeof(struct operation));
        server_receive_operation(op, buffers, data, sems);
        if(op->id != -1 && *data->terminate == 0){
            server_process_operation(op, server_id, &data->server_stats[server_id]);
            server_send_answer(op, buffers, data, sems);
        }else if(*data->terminate == 1){
            return data->server_stats[server_id];
        }
        destroy_dynamic_memory(op);
    }
}

void server_receive_operation(struct operation* op, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems){
    consume_begin(sems->prx_srv);
    if(*data->terminate == 1){
        return;
    }else{
        read_rnd_access_buffer(buffers->prx_srv, data->buffers_size, op);
        printf("		[Server recieved operation...]\n");
    }
    consume_end(sems->prx_srv);
}

void server_process_operation(struct operation* op, int server_id, int* counter){
    op->server  = server_id;
    op->status  = 'S';
    (*counter)++;
    get_timespec(op->server_time);
    printf("		[Server processed operation...]\n");
}

void server_send_answer(struct operation* op, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems){
    produce_begin(sems->srv_cli);
    write_circular_buffer(buffers->srv_cli, data->buffers_size, op);
    printf("		[Answer sent from server to client...]\n");
    produce_end(sems->srv_cli);
}

#endif