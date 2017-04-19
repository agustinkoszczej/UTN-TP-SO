/*
 * Sender.h
 *
 *  Created on: 13/4/2017
 *      Author: utnso
 */

#ifndef SENDER_H_
#define SENDER_H_

#include <./Headers.h>

//PARTE ENVIAR
void enviarMensaje(int socketCliente, char* msg, int tamanio);
void enviarHeader(int socket,int header);
void enviarPasamnos();
void enviarMensajeConTamanioDinamico(int socketCliente, char* msj);
//PARTE RECIBIR
char* recibirMensaje(int socket,int tamanioMsj,int criterio);
char* recibirMensajeSinEspera(int socketCliente, int tamanioMsj);
char* recibirMensajeConEspera(int socketCliente, int tamanioMsj);
int recibirTamanioMensaje(int socketCliente);
char* recibirMensajeConTamanioDinamico(int socket);
void esperarMensaje(int socket);
//PARTE HANDSHAKE
void iniciarHandshake(t_handshake QuienIniciaElHandshake, t_handshake QuienDevuelveHandshake, int socket);
void devolverHandshake(int socketCliente, t_handshake QuienDevuelveElHandshake);
int obtenerHandshake(int socketAlQueMeconecte);

#endif /* SENDER_H_ */
