/*
 * Memoria.h
 *
 *  Created on: 2/4/2017
 *      Author: utnso
 */

#ifndef MEMORIA_H_
#define MEMORIA_H_

#include <commons/config.h>

#include <SharedLibrary/Handshake.h>
#include <SharedLibrary/Headers.h>
#include <SharedLibrary/logger.h>
#include <SharedLibrary/Results.h>
#include <SharedLibrary/ServerManager.h>

typedef struct {
	int PUERTO;
	int MARCOS;
	int MARCO_SIZE;
	int ENTRADAS_CACHE;
	int CACHE_X_PROC;
	int RETARDO_MEMORIA;
}Config_Memoria;

Config_Memoria memoria_config;
char* path = "Memoria.cfg";

#endif /* MEMORIA_H_ */
