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

#include "Memoria.h"
#include "Results.h"
#include "ServerManager.h"


void cargarConfigMemoria() {

	t_config* configMemoria;
	configMemoria = config_create(path);

	memoria_config.PUERTO = config_get_int_value(configMemoria, "PUERTO");
	logger("Configurado Puerto", "INFO");
	memoria_config.MARCOS = config_get_int_value(configMemoria, "MARCOS");
	logger("Configurado Marcos", "INFO");
	memoria_config.MARCO_SIZE = config_get_int_value(configMemoria,
			"MARCO_SIZE");
	logger("Configurado Marco Size", "INFO");
	memoria_config.ENTRADAS_CACHE = config_get_int_value(configMemoria,
			"ENTRADAS_CACHE");
	logger("Configurado Entradas Cache", "INFO");
	memoria_config.CACHE_X_PROC = config_get_int_value(configMemoria,
			"CACHE_X_PROC");
	logger("Configurado Cache por Proc.", "INFO");
	memoria_config.RETARDO_MEMORIA = config_get_int_value(configMemoria,
			"RETARDO_MEMORIA");
	logger("Configurado Retardo Memoria", "INFO");

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

	Result r = SetupServer();

	printf("\n");

	puts(r.msg);


	return EXIT_SUCCESS;
}
