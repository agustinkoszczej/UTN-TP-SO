/*
 * ConfigConsola.h
 *
 *  Created on: 18/4/2017
 *      Author: utnso
 */

#ifndef CONFIGCONSOLA_H_
#define CONFIGCONSOLA_H_

#include <commons/config.h>
#include <SharedLibrary/logger.h>

typedef struct {
	char* IP_KERNEL;	//Después usar esta funcion para transformarlo inet_addr(IP_KERNEL)
	int PUERTO_KERNEL;
}Config_Consola;

Config_Consola consola_config;

#define RUTA_CFG "Consola.cfg"

void cargarConfigConsola();
void mostrarConfigConsola();



#endif /* CONFIGCONSOLA_H_ */
