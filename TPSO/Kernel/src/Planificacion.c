/*
 * Planificacion.c
 *
 *  Created on: 26/4/2017
 *      Author: utnso
 */

#include "Kernel.h"

void finalizarProceso(int socketCliente) {
	t_proceso* proceso = Conexiones.ListaClientes[buscarCliente(socketCliente)].proceso;
	cambiarEstado(proceso, EXIT);
}

void cambiarEstado(t_proceso* proceso, int estado) {
		if (estado == READY){
			//queue_push(colaListos, proceso);}
		}
		else if (estado == EXIT){
			//queue_push(colaSalida, proceso);
		}
		proceso->estado = estado;
}
