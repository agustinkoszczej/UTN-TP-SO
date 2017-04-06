/*
 * ServerCommons.c
 *
 *  Created on: 5/4/2017
 *      Author: utnso
 */
#include<stdio.h>

#define CANT_CONEXIONES 50 //Esto es del listen
#define MAX_HEADER 10 //Esto es el maximo del header

#include "ServerCommons.h"

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

struct sockaddr_in crearDireccionServidor(unsigned short PUERTO) {
	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = INADDR_ANY;
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
void permitirReutilizar(int servidor) {
	int activado = 1;
	setsockopt(servidor, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));
}

char* recibirMensaje(int cliente, int tamanioMsj, int criterio) {
	char* buffer = malloc(tamanioMsj);
	//Criterio = 0 -> No-Bloqueante
	//Criterio = MSG_WAITALL -> Bloqueante
	int bytesRecibidos = recv(cliente, buffer, tamanioMsj, criterio);
	if (bytesRecibidos <= 0) {
		printf("El Cliente %d se desconectó", cliente);
		exit(1);
	}
	buffer[tamanioMsj] = '\0';
	return buffer;
}

char* recibirMensajeSinEspera(t_cliente* Cliente, int tamanioMsj) {
	return recibirMensaje(Cliente->socket, tamanioMsj, 0);
}

char* recibirMensajeConEspera(int socketCliente, int tamanioMsj) {
	return recibirMensaje(socketCliente, tamanioMsj, MSG_WAITALL);
}

void hacerListen(int servidor) {
	listen(servidor, CANT_CONEXIONES);
}

void hacerBindeo(int* servidor, struct sockaddr_in* direccionServidor) {
	if (bind(servidor, (void*) direccionServidor, sizeof(*direccionServidor))
			!= 0) {
		perror("Fallo el bind");
		exit(1);
	}
}

void configurarServidor(struct sockaddr_in* direccion, int* socketServidor,
		unsigned short PUERTO) {
	(*direccion) = crearDireccionServidor(PUERTO);
	(*socketServidor) = crearSocket();
	permitirReutilizar(*socketServidor);
	//Recordar regla memotecnica
	//BLAB: Bind-Listen-Accept-Begin to Communicate
	hacerBindeo((*socketServidor), direccion);
	hacerListen((*socketServidor));
}

void agregarCliente(t_cliente* Cliente) {
	list_add(ListaClientes, Cliente);
	printf("El Cliente = %d fue agregado satisfactoriamente!\n",
			Cliente->socket);
}

void quitarCliente(int indice) {
	t_cliente* Cliente = list_get(ListaClientes, indice);
	printf(
			"El Cliente = %d, IP = %s, Puerto = %d fue quitado satisfactoriamente!\n",
			Cliente->socket, inet_ntoa(Cliente->direccion.sin_addr),
			ntohs(Cliente->direccion.sin_port));
	close(Cliente->socket);
	list_remove_and_destroy_element(Cliente, indice, free);
}

void hacerAccept(t_cliente* Cliente, int* socketServidor) {
	int socketNuevoCliente;
	unsigned int tamanioDireccion;
	Cliente->socket = accept((*socketServidor),
			(void*) &(Cliente->direccion), &tamanioDireccion);
	agregarCliente(Cliente);
}

void hacerHandshake(t_cliente* Cliente) {
	pthread_t hiloMensajes;

	enviarMensaje(Cliente->socket, (int) (Cliente->tipo), 1);

	pthread_create(&hiloMensajes, NULL, recibirMensajeConEspera, Cliente);
	Cliente->hilo = hiloMensajes;
}

void recibirClientes(int* socketServidor) {
	t_cliente* Cliente = malloc(sizeof(Cliente));

	hacerAccept(Cliente, socketServidor);
	hacerHandshake(Cliente);
}

int charToInt(char *c){
	return ((int)(*c));
}
char* intToChar(int i){
	return string_from_format("%c",i);
}
