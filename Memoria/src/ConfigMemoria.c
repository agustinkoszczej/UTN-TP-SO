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
	memoria_config.PUERTO = config_get_int_value(configMemoria, "PUERTO");
	logger("Configurado Puerto", "INFO", NOMBRE_PROCESO);
	memoria_config.MARCOS = config_get_int_value(configMemoria, "MARCOS");
	logger("Configurado Marcos", "INFO", NOMBRE_PROCESO);
	memoria_config.MARCO_SIZE = config_get_int_value(configMemoria,
			"MARCO_SIZE");
	logger("Configurado Marco Size", "INFO", NOMBRE_PROCESO);
	memoria_config.ENTRADAS_CACHE = config_get_int_value(configMemoria,
			"ENTRADAS_CACHE");
	logger("Configurado Entradas Cache", "INFO", NOMBRE_PROCESO);
	memoria_config.CACHE_X_PROC = config_get_int_value(configMemoria,
			"CACHE_X_PROC");
	logger("Configurado Cache por Proc.", "INFO", NOMBRE_PROCESO);
	memoria_config.RETARDO_MEMORIA = config_get_int_value(configMemoria,
			"RETARDO_MEMORIA");
	logger("Configurado Retardo Memoria", "INFO", NOMBRE_PROCESO);

	printf("Archivo de configuracion de Memoria cargado exitosamente!\n");
	logger("Archivo de configuracion cargado exitosamente", "INFO", NOMBRE_PROCESO);
}

void mostrarConfigMemoria() {
	printf("PUERTO=%d\n", memoria_config.PUERTO);
	printf("MARCOS=%d\n", memoria_config.MARCOS);
	printf("MARCO_SIZE=%d\n", memoria_config.MARCO_SIZE);
	printf("ENTRADAS_CACHE=%d\n", memoria_config.ENTRADAS_CACHE);
	printf("CACHE_X_PROC=%d\n", memoria_config.CACHE_X_PROC);
	printf("RETARDO_MEMORIA=%d\n", memoria_config.RETARDO_MEMORIA);
}
