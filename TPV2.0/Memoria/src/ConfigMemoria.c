/*
 * ConfigMemoria.c
 *
 *
 *      Author: utnso
 */
#include "ConfigMemoria.h"
#include "Memoria.h"

void cargarConfigMemoria() {

	t_config* configMemoria;
	configMemoria = config_create(RUTA_CFG);
	memoria_config.PUERTO_MEMORIA = config_get_int_value(configMemoria, "PUERTO");
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
	printf("PUERTO=%d\n", memoria_config.PUERTO_MEMORIA);
	printf("MARCOS=%d\n", memoria_config.MARCOS);
	printf("MARCO_SIZE=%d\n", memoria_config.MARCO_SIZE);
	printf("ENTRADAS_CACHE=%d\n", memoria_config.ENTRADAS_CACHE);
	printf("CACHE_X_PROC=%d\n", memoria_config.CACHE_X_PROC);
	printf("RETARDO_MEMORIA=%d\n", memoria_config.RETARDO_MEMORIA);
}
