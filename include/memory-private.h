/*
Trabalho realizado por:
Alexandre Godinho, a21804766
Bernardo Roque, a22103831
Francisco Nunes, a22102747
*/

#ifndef MEMORY_PRIVATE_H_GUARD
#define MEMORY_PRIVATE_H_GUARD

//estrutura que representa um buffer circular
struct circular_buffer { 	
    struct operation *op;
    int *in_out; //array de tamanho 2 em que a posição 0 = in e a posição 1 = out
};

//estrutura que representa um buffer de acesso aleatório
struct rnd_access_buffer { 		
    struct operation *op;
    int *ptr;
};

#endif