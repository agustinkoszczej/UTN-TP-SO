/*
 * CPU.c
 *
 *
 *      Author: utnso
 */

#include "CPU.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<stdbool.h>
#include<sys/types.h>
#include<time.h>
#include<commons/log.h>
#include<commons/string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MSJ_DINAMICO 50

int socketCPU;

struct sockaddr_in *direccionServidor() {
	struct sockaddr_in *retorno = malloc(sizeof(struct sockaddr_in));

	retorno->sin_family = AF_INET;
	retorno->sin_addr.s_addr = inet_addr(cpu_config.IP_KERNEL);
	retorno->sin_port = htons(cpu_config.PUERTO_KERNEL);

	return retorno;
}

void conectarAKernel() {
	socketCPU = getSocket();
	struct sockaddr_in *VdireccionServidor = direccionServidor();
	conectar(socketCPU, VdireccionServidor);
	iniciarHandshake(CPU, KERNEL, socketCPU);
}

int main(void) {
	printf("Iniciando CPU...\n\n");
	logger("Iniciando CPU", "INFO", NOMBRE_PROCESO);
	cargarConfigCPU();
	mostrarConfigCPU();
	conectarAKernel();
	printf("\n");
	esperarMensaje(socketCPU);

	return EXIT_SUCCESS;
}
