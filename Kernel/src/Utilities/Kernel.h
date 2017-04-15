/*
 * Kernel.h
 *
 *
 *      Author: utnso
 */

#ifndef KERNEL_H_
#define KERNEL_H_

#include <commons/config.h>
#include<commons/log.h>
#include<commons/string.h>

#include "../Handshake.h"
#include "../Headers.h"

typedef struct {
	int PUERTO_PROG;
	int PUERTO_CPU;
	char* IP_MEMORIA; //Después usar esta funcion para transformarlo inet_addr(IP_MEMORIA)
	int PUERTO_MEMORIA;
	char* IP_FS;
	int PUERTO_FS;
	int QUANTUM;
	int QUANTUM_SLEEP;
	char* ALGORITMO;
	int GRADO_MULTIPROG;
	char** SEM_IDS;
	char** SEM_INIT;
	char** SHARED_VARS;
	int STACK_SIZE;
}Config_Kernel;

Config_Kernel kernel_config;
char* path = "Kernel.cfg";

struct sockaddr_in direccionServidorKernel;
int socketServidorKernel;

#endif /* KERNEL_H_ */
