/*
 * FileSystem.c
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

#include "FileSystem.h"

#include <arpa/inet.h>
#include<sys/socket.h>

int socketFileSystem;

//-----------------------------------------------------------------------------------

struct sockaddr_in *direccionServidor() { //Kernel
	struct sockaddr_in *retorno = malloc(sizeof(struct sockaddr_in));

	retorno->sin_family = AF_INET;
	retorno->sin_addr.s_addr = inet_addr("127.0.0.1");//Hardcodeado nomas en este check
	retorno->sin_port = htons(8080);

	return retorno;
}
void conectarAKernel() {
	socketFileSystem = getSocket();
	struct sockaddr_in *VdireccionServidor = direccionServidor();
	conectar(socketFileSystem, VdireccionServidor);
	iniciarHandshake(FILE_SYSTEM, KERNEL, socketFileSystem);
}

//-----------------------------------------------------------------------------------

int main(void) {
	printf("Iniciando File System...\n\n");
	logger("Inicinado File System", "INFO", NOMBRE_PROCESO);

	cargarConfigFileSystem();
	mostrarConfigFileSystem();
	conectarAKernel();
	printf("\n");
	esperarMensaje(socketFileSystem);

	return EXIT_SUCCESS;
}
