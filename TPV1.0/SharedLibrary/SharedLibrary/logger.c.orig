/*
 * logger.c
 *
 *  Created on: 13/4/2017
 *      Author: utnso
 */


#include <stdio.h>
#include <stdlib.h>
#include<stdbool.h>
#include<sys/types.h>
#include<time.h>
#include<string.h>
#include<commons/log.h>
#include<commons/string.h>
#include"logger.h"

t_log* 	archivo_logger;

void logger (char * accion, char * tipo, char * archivo_nombre){

	t_log_level nivel;
	nivel = log_level_from_string(tipo);

	time_t tiempo = time(0);
		struct tm * timeinfo = localtime(&tiempo);
		char fecha [128];
		strftime(fecha, 128, "%d/%m/%y", timeinfo);
	char * archivo_fecha = string_new();
	string_append_with_format(&archivo_fecha, "Logger %s %c%c-%c%c-%c%c.txt", archivo_nombre, fecha[0], fecha[1], fecha[3], fecha[4], fecha[6], fecha[7]);
	printf("%s", archivo_fecha);

	archivo_logger = log_create(archivo_fecha, archivo_nombre, 0, nivel);
	switch(tipo[0]){
	case 'T':
			log_trace(archivo_logger, accion);
			break;
	case 'D':
			log_debug(archivo_logger, accion);
			break;
	case 'I':
			log_info(archivo_logger, accion);
			break;
	case 'W':
			log_warning(archivo_logger, accion);
			break;
	case 'E':
			log_error(archivo_logger, accion);
				break;
	}
	}


