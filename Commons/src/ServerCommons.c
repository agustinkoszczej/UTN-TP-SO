/*
 * ServerCommons.c
 *
 *  Created on: 5/4/2017
 *      Author: utnso
 */
#include<stdio.h>

int main(void) {
	printf("Iniciando Kernel...\n\n");
	logger("Iniciando Kernel", "INFO");

	//CARGAR ARCHIVO DE CONFIGURACIÓN
	cargarConfigKernel();
	//MOSTRAR ARCHIVO DE CONFIGURACIÓN
	mostrarConfigKernel();

	SetupServer();

	return 0;
}
