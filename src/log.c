/*
Trabalho realizado por:
Alexandre Godinho, a21804766
Bernardo Roque, a22103831
Francisco Nunes, a22102747
*/

#include <stdio.h>
#include <time.h>

char* get_time(){
	struct timespec spec;
	clock_gettime(CLOCK_REALTIME, &spec);
	time_t mytime = time(NULL);
    struct tm tm = *localtime(&mytime);
	
	int year = tm.tm_year + 1900;		// Ano
	int mon = tm.tm_mon + 1;			// Mês
	int day = tm.tm_mday;				// Dia
	int hour = tm.tm_hour;				// Hora
	int min = tm.tm_min;				// Minuto
	int sec = tm.tm_sec;  				// Segundo
	int msec = spec.tv_nsec / 1.0e6;	// Milisegundo

	char* string;

	asprintf(&string, "%04d-%02d-%02d %02d:%02d:%02d.%03d", year, mon, day, hour, min, sec, msec);

	return string;
}

void user_interaction_log(char log_filename[100], char instruction[4], int argument){
	FILE *file = fopen(log_filename, "a");

	if(instruction == "read") {
		fprintf(file, "%s %s %d\n",get_time(), instruction, argument);
	}else{
		fprintf(file, "%s %s\n",get_time(), instruction);
	}
    
	fclose(file);
}