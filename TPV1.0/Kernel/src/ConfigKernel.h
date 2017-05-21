/*
 * ConfigKernel.h
 *
 *  Created on: 19/4/2017
 *      Author: utnso
 */

#ifndef CONFIGKERNEL_H_
#define CONFIGKERNEL_H_


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

#define RUTA_CFG "Kernel.cfg"

void cargarConfigKernel();
void mostrarConfigKernel();


#endif /* CONFIGKERNEL_H_ */
