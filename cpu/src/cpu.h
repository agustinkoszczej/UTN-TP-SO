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
#include "cpu_interface.h"
#include <pcb.h>

#include <parser/metadata_program.h>

#define CPU "CPU"

#define IP_KERNEL "IP_KERNEL"
#define PUERTO_KERNEL "PUERTO_KERNEL"
#define IP_MEMORIA "IP_MEMORIA"
#define PUERTO_MEMORIA "PUERTO_MEMORIA"
#define CONFIG_FIELDS_N 4

int					socket_server;			/* Socket que apunta a servidor */
char *				ip = "127.0.0.1";		/* IP de server */
int					port = 5000;			/* Puerto de server */
t_dictionary * 		fns;					/* Funciones de socket */
pthread_mutex_t 	mx_main;				/* Semaforo de main */
t_log* 				logger;

pcb* 				pcbActual;
bool 				running;

#endif /* CPU_H_ */
