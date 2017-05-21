/*
 * Hilos.h
 *
 *  Created on: 7/4/2017
 *      Author: utnso
 */

#ifndef HILOS_H_
#define HILOS_H_

pthread_attr_t detachedAttr;

int crearHilo(pthread_t * hilo, void *(*funcion)(void *)) ;
int crearHiloConParametro(pthread_t * hilo, void *(*funcion)(void *),
		void * parametro);

#endif /* HILOS_H_ */
