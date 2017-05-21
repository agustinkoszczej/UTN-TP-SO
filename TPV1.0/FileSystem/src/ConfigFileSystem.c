/*
 * ConfigFileSystem.c
 *
 *  Created on: 19/4/2017
 *      Author: utnso
 */

#include "ConfigFileSystem.h"
#include "FileSystem.h"

void cargarConfigFileSystem() {

	t_config* configFileSystem;
	configFileSystem = config_create(RUTA_CFG);

	file_system_config.PUERTO = config_get_int_value(configFileSystem,
			"PUERTO");
	file_system_config.PUNTO_MONTAJE = config_get_string_value(configFileSystem,
			"PUNTO_MONTAJE");

	printf("Archivo de configuracion de File System cargado exitosamente!\n");
	logger("Archivo de configuracion cargado exitosamente", "INFO", NOMBRE_PROCESO);
}

void mostrarConfigFileSystem() {
	printf("PUERTO=%d\n", file_system_config.PUERTO);
	printf("PUNTO_MONTAJE=%s\n", file_system_config.PUNTO_MONTAJE);
}
