/*
 * Memoria.c
 *
 *
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>

#include "Memoria.h"

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

	//CARGAR ARCHIVO DE CONFIGURACIÓN
	cargarConfigMemoria();
	//MOSTRAR ARCHIVO DE CONFIGURACIÓN
	mostrarConfigMemoria();

	return EXIT_SUCCESS;
}
