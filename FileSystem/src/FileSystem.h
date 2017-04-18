/*
 * FileSystem.h
 *
 *
 *      Author: utnso
 */

#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include <commons/config.h>

#include <SharedLibrary/Handshake.h>
#include <SharedLibrary/Headers.h>
#include <SharedLibrary/logger.h>
#include <SharedLibrary/ServerManager.h>
#include <SharedLibrary/Results.h>

typedef struct {
	int PUERTO;
	char* PUNTO_MONTAJE;
}Config_FileSystem;

Config_FileSystem file_system_config;
char* path = "FileSystem.cfg";


#endif /* FILESYSTEM_H_ */
