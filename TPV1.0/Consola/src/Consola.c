/*
 * Consola.c
 *
 *
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>

#include "Consola.h"

#include<stdbool.h>
#include<sys/types.h>
#include<time.h>
#include<string.h>
#include<commons/string.h>

#include <arpa/inet.h>
#include<sys/socket.h>
#define MSJ_DINAMICO 50

int socketConsola; //Pruebo, no estoy seguro si lo necesito como global

struct sockaddr_in *direccionServidor() {
	struct sockaddr_in *retorno = malloc(sizeof(struct sockaddr_in));

	retorno->sin_family = AF_INET;
	retorno->sin_addr.s_addr = inet_addr(consola_config.IP_KERNEL);
	retorno->sin_port = htons(consola_config.PUERTO_KERNEL);

	return retorno;
}

void conectarAKernel() {
	socketConsola = getSocket();
	struct sockaddr_in *VdireccionServidor = direccionServidor();
	conectar(socketConsola, VdireccionServidor);
	iniciarHandshake(CONSOLA, KERNEL, socketConsola);

	while (1) {
		char* mensaje;
		puts("Consola: Ingrese mensaje a enviar Kernel:");
		scanf("%s", mensaje);
		enviarMensajeConTamanioDinamico(socketConsola, mensaje);
	}
}

int main() {
	printf("Iniciando Consola...\n\n");
	logger("Iniciando Consola", "INFO", NOMBRE_PROCESO);

	cargarConfigConsola();
	mostrarConfigConsola();
	conectarAKernel();

	return EXIT_SUCCESS;
}
