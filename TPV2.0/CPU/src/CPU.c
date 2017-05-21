/*
 * cpu.c
 *
 *  Created on: 2/4/2017
 *      Author: utnso
 */


#include "CPU.h"


void conectarKernel(){
	struct sockaddr_in direccionParaConectarseServidor = crearDireccionCliente(cpu_config.PUERTO_KERNEL_CPU, cpu_config.IP_KERNEL_CPU);
	socketCPU = crearSocket();
	conectar(socketCPU, &direccionParaConectarseServidor, KERNEL);

	iniciarHandshake(CPU, KERNEL, socketCPU);

	quedarEsperaRecibirMensajes(socketCPU, KERNEL, CPU);
}


int main (void){
	printf("Iniciando CPU \n");

	//CARGAR ARCHIVO DE CONFIGURACIÓN
	cargarConfigCPU();
	//MOSTRAR ARCHIVO DE CONFIGURACIÓN
	mostrarConfigCPU();

	conectarKernel();

	return EXIT_SUCCESS;
}
