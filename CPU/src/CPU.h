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

#define CPU "CPU"

int					socket_server;			/* Socket que apunta a servidor */
char *				ip = "127.0.0.1";		/* IP de server */
int					port = 5000;			/* Puerto de server */
t_dictionary * 		fns;					/* Funciones de socket */
pthread_mutex_t 	mx_main;				/* Semaforo de main */

#endif /* CPU_H_ */
