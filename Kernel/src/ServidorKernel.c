/*
 * ServidorKernel.c
 *
 *  Created on: 2/4/2017
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

//El puerto lo recibe del archivo de configuracion

//CREACIONES
//-----------------------------------------------

struct sockaddr_in crearDireccionServidor(unsigned short PUERTO) {
	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = INADDR_ANY;
	direccionServidor.sin_port = htons(PUERTO);
	return direccionServidor;
}

int crearSocket() {
	return socket(AF_INET, SOCK_STREAM, 0);
}

struct sockaddr_in crearDireccionCliente(unsigned short PUERTO, char* IP) {
	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = inet_addr(IP);
	direccionServidor.sin_port = htons(PUERTO);
	return direccionServidor;
}

void conectar(int socketCliente, struct sockaddr_in* direccionServidor) {
	if (connect(socketCliente, (void*) direccionServidor,
			sizeof(*direccionServidor)) != 0) {
		perror("No se pudo conectar");
		exit(1);
	}
}

void enviarMensaje(int socketCliente, char* msg, int tamanio) {
	send(socketCliente, msg, tamanio, 0);
}

//VER VIDEO SOCKETS DE UTN.SO PARTE 2 PARA ENTENDER ESTA FUNCION
void permitirReutilizar(int servidor, int activado) {
	activado = 1;
	setsockopt(servidor, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));
}

char* recibirMensaje(int cliente, int tamanioMsj, int criterio) {
	char* buffer = malloc(tamanioMsj);
	int bytesRecibidos = recv(cliente, buffer, tamanioMsj, criterio);
	if (bytesRecibidos <= 0) {
		perror("El Cliente se desconectó");
		exit(1);
	}
	buffer[tamanioMsj] = '\0';
	return buffer;
}

char* recibirMensajeSinEspera(int socketCliente, int tamanio) {
	return recibirMensaje(socketCliente, tamanio, 0);
}

char* recibirMensajeConEspera(int socketCliente, int tamanio) {
	return recibirMensaje(socketCliente, tamanio, MSG_WAITALL);
}
