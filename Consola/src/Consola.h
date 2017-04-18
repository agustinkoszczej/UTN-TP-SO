/*
 * Consola.h
 *
 *
 *      Author: utnso
 */

#ifndef CONSOLA_H_
#define CONSOLA_H_

#include <commons/config.h>

#include <SharedLibrary/Handshake.h>
#include <SharedLibrary/Headers.h>
#include <SharedLibrary/logger.h>
typedef struct {
	char* IP_KERNEL;	//Después usar esta funcion para transformarlo inet_addr(IP_KERNEL)
	int PUERTO_KERNEL;
}Config_Consola;

Config_Consola consola_config;
char* path = "Consola.cfg";

#endif /* CONSOLA_H_ */
