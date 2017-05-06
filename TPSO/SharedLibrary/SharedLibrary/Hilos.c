/*
 * Hilos.c
 *
 *  Created on: 7/4/2017
 *      Author: utnso
 */

#include <pthread.h>
#include "Hilos.h"

int crearHilo(pthread_t * hilo, void *(*funcion)(void *)) {
	return pthread_create(hilo, &detachedAttr, (void*) funcion, NULL);
}
int crearHiloConParametro(pthread_t * hilo, void *(*funcion)(void *),
		void * parametro) {
	return pthread_create(hilo, &detachedAttr, (void*) funcion, parametro);
}
