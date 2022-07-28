/*
Trabalho realizado por:
Alexandre Godinho, a21804766
Bernardo Roque, a22103831
Francisco Nunes, a22102747
*/

#include <time.h>
#include <stdlib.h>
#include "sotime.h"

void get_timespec(struct timespec spec){
    clock_gettime(CLOCK_REALTIME, &spec);
}