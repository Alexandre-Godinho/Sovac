/*
Trabalho realizado por:
Alexandre Godinho, a21804766
Bernardo Roque, a22103831
Francisco Nunes, a22102747
*/

#include "main.h"

/* Função que irá ler um ficheiro config, cujo nome é passado pelo
* utilizador ao lançar o SOVAC e que irá atribuir os devidos valores
* às variaveis max_ops, buffers_size, n_clients, n_proxies, n_servers
* e log_filename da estrutura data.
*/
void config(char *config_filename, struct main_data* data);