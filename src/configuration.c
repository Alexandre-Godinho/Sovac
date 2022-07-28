/*
Trabalho realizado por:
Alexandre Godinho, a21804766
Bernardo Roque, a22103831
Francisco Nunes, a22102747
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"

void config(char *config_filename, struct main_data* data){
    FILE *file = fopen(config_filename, "r");
    char str[20];
    int line = 0;

    if(file == NULL){
        perror("Error opening the file!\n");
        exit(0);
    }

    while(line < 6){
        fgets(str, 20, file);
        switch(line){
            case 0:{
                if(atoi(str) == 0){
                    printf("max_ops has to be an integer bigger than 0!\n");
                    exit(0);
                }else{
                    data->max_ops = atoi(str);
                    break;
                }
            }
            case 1:{
                if(atoi(str) == 0){
                    printf("buffers_size has to be an integer bigger than 0!\n");
                    exit(0);
                }else{
                    data->buffers_size = atoi(str);
                    break;
                }
            }
            case 2:{
                if(atoi(str) == 0){
                    printf("n_clients has to be an integer bigger than 0!\n");
                    exit(0);
                }else{
                    data->n_clients = atoi(str);
                    break;
                }
            }
            case 3:{
                if(atoi(str) == 0){
                    printf("n_proxies has to be an integer bigger than 0!\n");
                    exit(0);
                }else{
                    data->n_proxies = atoi(str);
                    break;
                }
            }
            case 4:{
                if(atoi(str) == 0){
                    printf("n_servers has to be an integer bigger than 0!\n");
                    exit(0);
                }else{
                    data->n_servers = atoi(str);
                    break;
                }
            }
            default:{
                strcpy(data->log_filename, str);
                break;
            }
        }
        line++;
    }

    fclose(file);
}