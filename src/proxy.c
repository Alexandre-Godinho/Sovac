/*
Trabalho realizado por:
Alexandre Godinho, a21804766
Bernardo Roque, a22103831
Francisco Nunes, a22102747
*/

#ifndef PROXY_H_GUARD
#define PROXY_H_GUARD

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory.h"
#include "main.h"
#include "proxy.h"
#include "synchronization.h"

int execute_proxy(int proxy_id, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems){
    while(1){
        struct operation* op;
        op = create_dynamic_memory(sizeof(struct operation));
        proxy_receive_operation(op, buffers, data, sems);
        if(op->id != -1 && *data->terminate == 0){
            proxy_process_operation(op, proxy_id, &data->proxy_stats[proxy_id]);
            proxy_forward_operation(op, buffers, data, sems);
        }else if(*data->terminate == 1){
            return data->proxy_stats[proxy_id];
        }
        destroy_dynamic_memory(op);
    }
}

void proxy_receive_operation(struct operation* op, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems){
    consume_begin(sems->cli_prx);
    if(*data->terminate == 1){
        return;
    }else{
        read_circular_buffer(buffers->cli_prx, data->buffers_size, op);
        printf("		[Proxy recieved operation...]\n");
    }
    consume_end(sems->cli_prx);
}

void proxy_process_operation(struct operation* op, int proxy_id, int* counter){
    op->proxy   = proxy_id;
    op->status  = 'P';
    (*counter)++;
    get_timespec(op->proxy_time);
    printf("		[Proxy processed operation...]\n");
}

void proxy_forward_operation(struct operation* op, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems){
    produce_begin(sems->prx_srv);
    write_rnd_access_buffer(buffers->prx_srv, data->buffers_size, op);
    printf("		[Operation sent from proxy to server...]\n");
    produce_end(sems->prx_srv);
}

#endif