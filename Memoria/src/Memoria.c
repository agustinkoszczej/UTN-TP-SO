/*
 * Memoria.c
 *
 *
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>
#include<stdbool.h>
#include<sys/types.h>
#include<time.h>
#include<string.h>
#include<commons/log.h>
#include<commons/string.h>

#include "Memoria.h"


#include <arpa/inet.h>
#include<sys/socket.h>

int socketMemoria;

//-----------------------------------------------------------------------------------------------------

struct sockaddr_in* direccionServidor(){ //Kernel
	struct sockaddr_in* retorno = malloc(sizeof(struct sockaddr_in));

	retorno->sin_family = AF_INET;
	retorno->sin_addr.s_addr = inet_addr("127.0.0.1");	//Hardcodeado nomas en este check
	retorno->sin_port = htons(8080);

	return retorno;
}


void conectarAKernel(){
	socketMemoria = getSocket();
	struct sockaddr_in *VdireccionServidor = direccionServidor();
	conectar(socketMemoria, VdireccionServidor);
	iniciarHandshake(MEMORIA, KERNEL,socketMemoria);
}


//-----------------------------------------------------------------------------------------------------

int main(void) {
	printf("Iniciando Memoria...\n\n");
	logger("Iniciando Memoria", "INFO", NOMBRE_PROCESO);

	cargarConfigMemoria();
	mostrarConfigMemoria();
	conectarAKernel();
	printf("\n");
	esperarMensaje(socketMemoria);


	return EXIT_SUCCESS;
}
