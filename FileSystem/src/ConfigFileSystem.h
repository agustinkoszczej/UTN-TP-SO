/*
 * ConfigFileSystem.h
 *
 *  Created on: 19/4/2017
 *      Author: utnso
 */

#ifndef CONFIGFILESYSTEM_H_
#define CONFIGFILESYSTEM_H_


typedef struct {
	int PUERTO;
	char* PUNTO_MONTAJE;
}Config_FileSystem;

Config_FileSystem file_system_config;

#define RUTA_CFG "FileSystem.cfg"

void cargarConfigFileSystem();
void mostrarConfigFileSystem();

#endif /* CONFIGFILESYSTEM_H_ */
