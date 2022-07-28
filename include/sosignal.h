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

/*Função que liberta todos os recursos do SOVAC de forma correta
* quando um sinal de interrupeção do programa é capturado,
* através da execução da função stop_execution.
*/
void sigint_handler(int signal);