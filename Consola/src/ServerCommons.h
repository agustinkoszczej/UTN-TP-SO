/*
 * ServerCommons.h
 *
 *
 *      Author: utnso
 */

#ifndef SERVERCOMMONS_H_
#define SERVERCOMMONS_H_

#include <commons/collections/list.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdbool.h>

#include <pthread.h>

#include "Handshake.h"
#include "Headers.h"

//struct sockaddr_in direccion;
//int socketServidor, tamanioDireccion;

int activado;

typedef struct {
	int socket;
	struct sockaddr_in direccion;
	pthread_t hilo;
	t_handshake tipo;
} t_cliente;

t_list* ListaClientes;


int crearSocket();
struct sockaddr_in crearDireccionCliente(unsigned short PUERTO, char* IP);
struct sockaddr_in crearDireccionServidor(unsigned short PUERTO);
void conectar(int socketCliente, struct sockaddr_in* direccionServidor);
void enviarMensaje(int socketCliente, char* msg, int tamanio);
void permitirReutilizar(int servidor);
char* recibirMensaje(int cliente, int tamanioMsj, int criterio);
char* recibirMensajeSinEspera(t_cliente* Cliente, int tamanioMsj);
char* recibirMensajeConEspera(int socketCliente, int tamanioMsj);
void hacerListen(int servidor);
void hacerBindeo(int* servidor, struct sockaddr_in* direccionServidor);
void configurarServidor(struct sockaddr_in* direccion, int* socketServidor, unsigned short PUERTO);
void agregarCliente(t_cliente* Cliente);
void quitarCliente(int indice);
void hacerAccept(t_cliente* Cliente, int* socketServidor);
void hacerHandshake(t_cliente* Cliente);
void recibirClientes(int* socketServidor);


#endif /* SERVERCOMMONS_H_ */
