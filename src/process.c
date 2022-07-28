/*
Trabalho realizado por:
Alexandre Godinho, a21804766
Bernardo Roque, a22103831
Francisco Nunes, a22102747
*/

#ifndef PROCESS_H_GUARD
#define PROCESS_H_GUARD

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "memory.h"
#include "main.h"
#include "process.h"
#include "synchronization.h"

int launch_process(int process_id, int process_code, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems){
    int pid = fork();
    if(pid == 0){
        if(process_code == 0){
            exit(execute_client(process_id, buffers, data, sems));
        }else if(process_code == 1){
            exit(execute_proxy(process_id, buffers, data, sems));
        }else if(process_code == 2){
            exit(execute_server(process_id, buffers, data, sems));
        }
    }else{
        return pid;
    }
}

int wait_process(int process_id){
    int status;
    waitpid(process_id, &status, 0);
    return  WEXITSTATUS(status);
}

#endif