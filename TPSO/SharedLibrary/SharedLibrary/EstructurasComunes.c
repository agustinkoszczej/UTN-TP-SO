/*
 * EstructurasComunes.c
 *
 *  Created on: 26/4/2017
 *      Author: utnso
 */

#include "EstructurasComunes.h"

t_PCB* crearPCB() {
	t_PCB* pcb = malloc(sizeof(t_PCB));
	pcb->PC=0;
	pcb->PID=0;
	pcb->cantidad_paginas=0;
	//pcb->SP = stack_create();
	pcb->indice_codigo = list_create();
	pcb->indice_etiquetas = dictionary_create();
	return pcb;
}

void eliminarPCB(t_PCB* pcb){
	list_destroy(pcb->indice_codigo);
	//stack_destroy(pcb->SP);
	dictionary_destroy(pcb->indice_etiquetas);
	free(pcb);
}
