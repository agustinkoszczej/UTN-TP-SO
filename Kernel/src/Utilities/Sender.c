/*
 * Sender.c
 *
 *  Created on: 13/4/2017
 *      Author: utnso
 */

#include "Handshake.h"
#include "Headers.h"

void enviarMensaje(int socketCliente, char* msg, int tamanio) {
	send(socketCliente, msg, tamanio, 0);
}

void enviarHeader(int socket,int header) {
	char* head = intToString(header);

	send(socket, head, 4, 0);
}

void devolverHandshake(int socketCliente, t_handshake QuienDevuelveElHandshake) {
	 enviarHeader(socketCliente, HEADER_HANDSHAKE);

	 enviarMensaje(socketCliente, intToString(QuienDevuelveElHandshake), 1);
}

void enviarPasamanos(int socketCliente, char* msg){
	enviarHeader(socketCliente, HEADER_PASAMANOS);

	enviarMensaje(socketCliente, intToString(msg), 1);
}
