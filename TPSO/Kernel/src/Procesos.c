/*
 * Procesos.c
 *
 *  Created on: 26/4/2017
 *      Author: utnso
 */

#include "Kernel.h"

#include <SharedLibrary/EstructurasComunes.h>
#include <SharedLibrary/ServerCommons.h>

#include <math.h>

t_proceso* crearProceso(int socketConsola){
	t_proceso* proceso = malloc(sizeof(t_proceso));
	proceso->PCB = crearPCB();
	proceso->PCB->PID = procesos++;
	proceso->estado = NEW;
	proceso->socketConsola = socketConsola;
	Conexiones.ListaClientes[buscarCliente(socketConsola)].proceso = proceso;

		char* codigo = getScript(socketConsola);

		printf("\n%s\n", codigo);

		proceso->PCB->cantidad_paginas = ceil(
				((double) strlen(codigo)) / ((double) tamanio_pagina));

		if (!pedirPaginas(proceso, codigo)) {
			rechazarProceso(proceso);
			printf("El Proceso fue rechazado!\n");
		} else {
			cambiarEstado(proceso, READY);
			printf("Creado PID:%d para la Consola %d\n\n", proceso->PCB->PID, socketConsola);
		}
		free(codigo);
	return proceso;
}

void rechazarProceso(t_proceso* proceso) {
	eliminarPCB(proceso->PCB);
	free(proceso);
}

char* exitCode(int EC){
	switch (EC){

	case 0:
		return "El programa finalizó correctamente.";
	break;

	case -1:
		return "No se pudieron reservar recursos para ejecutar el programa.";
	break;

	case -2:
		return "El programa intentó acceder a un archivo que no existe.";
	break;

	case -3:
		return "El programa intentó leer un archivo sin permisos.";
	break;

	case -4:
		return "El programa intentó escribir un archivo sin permisos.";
	break;

	case -5:
		return "Excepción de memoria.";
	break;

	case -6:
		return "Finalizado a través de desconexión de consola.";
	break;

	case -7:
		return "Finalizado a través del comando Finalizar Programa de la consola.";
	break;

	case -8:
		return "Se intentó reservar más memoria que el tamaño de una página.";
	break;

	case -9:
		return "No se pueden asignar más páginas al proceso.";
	break;

	case -20:
		return "Error sin definición.";
	break;
	}
}
