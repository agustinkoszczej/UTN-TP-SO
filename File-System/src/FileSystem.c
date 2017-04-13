/*
 * FileSystem.c
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

#include "FileSystem.h"
#include "ServerManager.h"
#include "Results.h"

void cargarConfigFileSystem() {

	t_config* configFileSystem;
	configFileSystem = config_create(path);

	file_system_config.PUERTO = config_get_int_value(configFileSystem,
			"PUERTO");
	file_system_config.PUNTO_MONTAJE = config_get_string_value(configFileSystem,
			"PUNTO_MONTAJE");

	printf("Archivo de configuracion de File System cargado exitosamente!\n");
	logger("Archivo de configuracion cargado exitosamente", "INFO");
}

void mostrarConfigFileSystem() {
	printf("PUERTO=%d\n", file_system_config.PUERTO);
	printf("PUNTO_MONTAJE=%s\n", file_system_config.PUNTO_MONTAJE);
}

int main(void) {
	printf("Iniciando File System...\n\n");
	logger("Inicinado File System", "INFO");

	//CARGAR ARCHIVO DE CONFIGURACIÓN
	cargarConfigFileSystem();
	//MOSTRAR ARCHIVO DE CONFIGURACIÓN
	mostrarConfigFileSystem();

	Result r = SetupServer();

	printf("\n");

	puts(r.msg);

	return EXIT_SUCCESS;
}
