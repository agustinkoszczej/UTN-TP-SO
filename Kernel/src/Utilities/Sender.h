/*
 * Sender.h
 *
 *  Created on: 13/4/2017
 *      Author: utnso
 */

#ifndef SENDER_H_
#define SENDER_H_

void enviarMensaje(int socketCliente, char* msg, int tamanio);
void enviarHeader(int socket,int header);
void devolverHandshake(int socketCliente, t_handshake QuienDevuelveElHandshake);
void enviarPasamnos();

#endif /* SENDER_H_ */
