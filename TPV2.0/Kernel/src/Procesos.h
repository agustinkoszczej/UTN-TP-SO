/*
 * Procesos.h
 *
 *  Created on: 26/4/2017
 *      Author: utnso
 */

#ifndef PROCESOS_H_
#define PROCESOS_H_

#include <SharedLibrary/EstructurasComunes.h>

typedef enum t_proceso_estado {
	NEW = 0, //0
	READY = 1, //1
	EXEC = 2, //2
	BLOCK = 3, //3
	EXIT = 4 //4
} t_proceso_estado;

typedef struct t_proceso {
	t_proceso_estado estado;
	t_PCB* PCB;
	int socketConsola;
	int socketCPU;
} t_proceso;

t_proceso* crearProceso(int socketConsola);
void rechazarProceso(t_proceso* proceso);
char* exitCode(int EC);

#endif /* PROCESOS_H_ */
