#ifndef CPU_H_
#define CPU_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <commons/string.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>
#include <commons/error.h>
#include <console.h>
#include <config.h>
#include <socket.h>

#include <pcb.h>

#include "cpu_interface.h"

#include <parser/metadata_program.h>

#define CPU "CPU"

#define IP_KERNEL "IP_KERNEL"
#define PUERTO_KERNEL "PUERTO_KERNEL"
#define IP_MEMORIA "IP_MEMORIA"
#define PUERTO_MEMORIA "PUERTO_MEMORIA"
#define CONFIG_FIELDS_N 4

t_dictionary * 		fns;					/* Funciones de socket */
pthread_mutex_t 	mx_main;				/* Semaforo de main */
t_log* 				logger;

pcb* 				pcbActual;
bool 				running;
//TODO
int FRAME_SIZE; //se lo tengo que pedir a Memoria, despues veo como hacerlo

#endif /* CPU_H_ */
