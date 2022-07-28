/*
Trabalho realizado por:
Alexandre Godinho, a21804766
Bernardo Roque, a22103831
Francisco Nunes, a22102747
*/

/* Função que começa por abrir/criar um ficheiro log, cujo nome foi definido pelo utilizador
* no ficheiro de configuração, escreve a data, a hora e a instrução que o utilizador
* mandou o SOVAC realizar e, por fim, fecha o ficheiro log.
*/
void user_interaction_log(char log_filename[100], char instruction[4], int argument);