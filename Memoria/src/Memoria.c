/*
 * Memoria.c
 *
 *
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

int activar_logger = 0; //Cambiar a 1 cuando no lo estemos probando. Esto es para que no nos llenemos de archivos cada vez que lo probamos.

#include "Memoria.h"

t_log* 	archivo_logger;

void logger (char * accion, char * tipo){

	if(activar_logger == 1){

	t_log_level nivel;
	nivel = log_level_from_string(tipo);

	time_t tiempo = time(0);
		struct tm * timeinfo = localtime(&tiempo);
		char fecha [128];
		strftime(fecha, 128, "%d/%m/%y", timeinfo);
	char * archivo_nombre = "Memoria";
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
}


void cargarConfigMemoria() {

	t_config* configMemoria;
	configMemoria = config_create(path);

	memoria_config.PUERTO = config_get_int_value(configMemoria, "PUERTO");
	memoria_config.MARCOS = config_get_int_value(configMemoria, "MARCOS");
	memoria_config.MARCO_SIZE = config_get_int_value(configMemoria,
			"MARCO_SIZE");
	memoria_config.ENTRADAS_CACHE = config_get_int_value(configMemoria,
			"ENTRADAS_CACHE");
	memoria_config.CACHE_X_PROC = config_get_int_value(configMemoria,
			"CACHE_X_PROC");
	memoria_config.RETARDO_MEMORIA = config_get_int_value(configMemoria,
			"RETARDO_MEMORIA");

	printf("Archivo de configuracion de Memoria cargado exitosamente!\n");
	logger("Archivo de configuracion cargado exitosamente", "INFO");
}

void mostrarConfigMemoria() {
	printf("PUERTO=%d\n", memoria_config.PUERTO);
	printf("MARCOS=%d\n", memoria_config.MARCOS);
	printf("MARCO_SIZE=%d\n", memoria_config.MARCO_SIZE);
	printf("ENTRADAS_CACHE=%d\n", memoria_config.ENTRADAS_CACHE);
	printf("CACHE_X_PROC=%d\n", memoria_config.CACHE_X_PROC);
	printf("RETARDO_MEMORIA=%d\n", memoria_config.RETARDO_MEMORIA);
}

int main(void) {
	printf("Iniciando Memoria...\n\n");
	logger("Iniciando Memoria", "INFO");

	//CARGAR ARCHIVO DE CONFIGURACIÓN
	cargarConfigMemoria();
	//MOSTRAR ARCHIVO DE CONFIGURACIÓN
	mostrarConfigMemoria();

	return EXIT_SUCCESS;
}
