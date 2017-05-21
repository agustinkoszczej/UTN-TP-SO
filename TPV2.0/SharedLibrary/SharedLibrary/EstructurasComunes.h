/*
 * EstructurasComunes.h
 *
 *  Created on: 26/4/2017
 *      Author: utnso
 */

#ifndef SHAREDLIBRARY_ESTRUCTURASCOMUNES_H_
#define SHAREDLIBRARY_ESTRUCTURASCOMUNES_H_

#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>

typedef struct {
	int PID; // Identificador Único
	int PC;	 // Program Counter
	//t_stack* SP; // Posición del Stack: Pila de t_stack_item
	int cantidad_paginas;
	t_list* indice_codigo; // lista de t_sentencias que indican la posicion absoluta de cada sentencia
	t_dictionary* indice_etiquetas; // diccionario [Etiqueta,Posicion de la sentencia a saltar]
	int exitCode;
} t_PCB;

t_PCB* crearPCB();

#endif /* SHAREDLIBRARY_ESTRUCTURASCOMUNES_H_ */
