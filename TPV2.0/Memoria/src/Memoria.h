/*
 * Memoria.h
 *
 *  Created on: 2/4/2017
 *      Author: utnso
 */

#ifndef MEMORIA_H_
#define MEMORIA_H_

#include <SharedLibrary/ServerCommons.h>
#include <SharedLibrary/Hilos.h>

#include "ConfigMemoria.h"

typedef struct{
	int pid;
	int nroPagina;
	int frame;
}tabla_pags_invert_t;

struct sockaddr_in direccionServidorMemoria;
int socketServidorMemoria;

int activadoKernel;

char* MEMORIA_PRINCIPAL;

#endif /* MEMORIA_H_ */
