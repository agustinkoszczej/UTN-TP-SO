/*
 * Memoria.c
 *
 *
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>


#include "Memoria.h"

void cargarPrograma(int socketKernel) {
	char* pidScript = malloc(sizeof(int));
	char* cantidadDePaginasScript = malloc(sizeof(int));
	char* tamanioCodigoScript = malloc(sizeof(int));

	//PID
	pidScript = recibirMensajeConEspera(socketKernel,sizeof(int));
	//PAGINAS
	cantidadDePaginasScript = recibirMensajeConEspera(socketKernel, sizeof(int));
	//TAMAÑO CODIGO
	tamanioCodigoScript = recibirMensajeConEspera(socketKernel, sizeof(int));

	//CODIGO
	char* codigoScript = malloc(stringToInt(tamanioCodigoScript));

	codigoScript = recibirMensajeConEspera(socketKernel, stringToInt(tamanioCodigoScript));

	printf("Codigo recibido = \n%s\n", codigoScript);

	enviarMensaje(socketKernel,"1", 1);

	//string_append(MEMORIA_PRINCIPAL, codigoScript);

	free(pidScript);
	free(cantidadDePaginasScript);
	free(tamanioCodigoScript);

}

void atenderKernel(int socketKernel){
	while(1){

		int head = recibirHeader(socketKernel);
		switch(head){
		case HEADER_PROGRAMA:
			cargarPrograma(socketKernel);
		break;
		}
	}
}

void enviarTamanioDePagina(int socketKernel) {
	char* serialTamanio = intToString(memoria_config.MARCO_SIZE);
	enviarMensaje(socketKernel, serialTamanio, sizeof(int));
	free(serialTamanio);
}

void recibirKernel() {

	configurarServidor(&direccionServidorMemoria, &socketServidorMemoria,
			memoria_config.PUERTO_MEMORIA, &activadoKernel);

	printf("Esperando Kernel...\n");

	while (1) {
		int socketKernel = recibirCliente(socketServidorMemoria);
		devolverHandshake(socketKernel, KERNEL);
		enviarTamanioDePagina(socketKernel);
		atenderKernel(socketKernel);
		//quedarEsperaRecibirMensajes(socketKernel, KERNEL, MEMORIA);
	}

}

void reservarMemoriaPrincipal(){
	MEMORIA_PRINCIPAL = malloc(memoria_config.MARCO_SIZE * memoria_config.MARCOS);
	memset(MEMORIA_PRINCIPAL, '\0', sizeof(MEMORIA_PRINCIPAL));
}

int main(void) {
	//system("clear");
	printf("Iniciando Memoria...\n\n");

	cargarConfigMemoria();
	mostrarConfigMemoria();

	reservarMemoriaPrincipal();
	recibirKernel();

	return EXIT_SUCCESS;
}
