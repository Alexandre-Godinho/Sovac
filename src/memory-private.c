/*
Trabalho realizado por:
Alexandre Godinho, a21804766
Bernardo Roque, a22103831
Francisco Nunes, a22102747
*/

#ifndef MEMORY_PRIVATE_H_GUARD
#define MEMORY_PRIVATE_H_GUARD

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory-private.h"

//estrutura que representa um buffer circular
struct circular_buffer { 	
    struct operation *op;
    int *in_out;
};

//estrutura que representa um buffer de acesso aleatório
struct rnd_access_buffer { 	
    struct operation *op;
    int *ptr;
};

#endif