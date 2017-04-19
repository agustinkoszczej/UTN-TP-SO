/*
 * Sender.c
 *
 *  Created on: 13/4/2017
 *      Author: utnso
 */

#include "Handshake.h"
#include "Headers.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

//*******************************************************************************
//*									PARTE ENVIO 								*
//*******************************************************************************

void enviarMensaje(int socketCliente, char* msg, int tamanio) {
	send(socketCliente, msg, tamanio, 0);
}

void enviarHeader(int socket, int header) {
	char* head = intToString(header);

	enviarMensaje(socket, head, 4);
}

void enviarPasamanos(int socketCliente, char* msg){
	enviarHeader(socketCliente, HEADER_PASAMANOS);

	enviarMensaje(socketCliente, msg, 50);
}

//NUEVAS
void enviarMensajeConTamanioDinamico(int socketCliente, char* msj) {
	char* longitudMensaje = string_new();
	longitudMensaje = intToString(strlen(msj));

	//FIXEAR ESTO
	//enviarMensaje(socketCliente, longitudMensaje, HEADER_LENGTH); //LE ENVIO PRIMERO EL TAMAÑO DEL MENSAJE A ENVIAR
	//enviarMensaje(socketCliente, msj, strlen(msj)); //LE ENVIO EL MENSAJE DE ESE TAMAÑO
	enviarHeader(socketCliente, 1);
	enviarMensaje(socketCliente, msj, 50);
}

//*******************************************************************************
//*									PARTE RECIBIR								*
//*******************************************************************************

char* recibirMensaje(int socket,int tamanioMsj,int criterio) {
	char* buffer = malloc(tamanioMsj);

	int bytesRecibidos = recv(socket, buffer, tamanioMsj, criterio);

	if(bytesRecibidos <= 0) {
		printf("El Cliente %d se desconecto\n",socket);
	}
	buffer[tamanioMsj] = '\0';
	return buffer;
}

char* recibirMensajeSinEspera(int socketCliente, int tamanioMsj) {
	return recibirMensaje(socketCliente, tamanioMsj, 0); //NO BLOQUEANTE
}

char* recibirMensajeConEspera(int socketCliente, int tamanioMsj) {
	return recibirMensaje(socketCliente, tamanioMsj, MSG_WAITALL); //BLOQUEANTE
}

int recibirTamanioMensaje(int socketCliente) {
	return stringToInt(recibirMensajeConEspera(socketCliente, HEADER_LENGTH));
}

char* recibirMensajeConTamanioDinamico(int socket) {
	int tamanioProxMsj = recibirTamanioMensaje(socket);
	if(tamanioProxMsj != 0){
		return recibirMensajeConEspera(socket, tamanioProxMsj);
	}
		return "ERROR";
}

//VER PARA CORREGIR ESTA FUNCION, ESTA DEMASIADA HARDCODEADA
void esperarMensaje(int socket){
	while(1){

		char* buffer = malloc(4);
			while(1){
				if(recv(socket, buffer, 4, MSG_WAITALL)<=0){
					close(socket);
					printf("Servidor desconectado\n");
					return;
				}
				else{
					if(stringToInt(buffer) == HEADER_PASAMANOS){
						char* msg = malloc(50);
						recv(socket, msg, 50, 0);
						puts(msg);
						free(msg);
					}
				}
			}
	}
}

//*******************************************************************************
//*									PARTE HANDSHAKE								*
//*******************************************************************************

void iniciarHandshake(t_handshake QuienIniciaElHandshake, t_handshake QuienDevuelveHandshake, int socket) {

	enviarHeader(socket, HEADER_HANDSHAKE);  //ENVIO EL HEADER DEL HANDSHAKE PARA QUE EL PROCESO QUE LO RECIBE VERIFIQUE SI ES UN HANDSHAKE LO QUE LE ENVIO O NO

	char* handshake = intToString(QuienIniciaElHandshake);
	enviarMensaje(socket, handshake, 1);		//ENVIO AL SOCKET QUE ME CONECTO EL VALOR DE QUIEN INICIA EL HANDSHAKE (EN ESTE CASO ES UN 4 QUE REPRESENTA A LA CONSOLA)

	if(obtenerHandshake(socket) == QuienDevuelveHandshake) {   //OBTENGO EL HANDSHAKE DEL PROCESO AL QUE LE ENVIE ANTERIORMENTE EL HANDSHAKE (KERNEL) Y LO COMPARO
		printf("\nSe conecto satisfactoriamente al Kernel\n");
	}
}

void devolverHandshake(int socketCliente, t_handshake QuienDevuelveElHandshake) {
	 enviarHeader(socketCliente, HEADER_HANDSHAKE);

	 enviarMensaje(socketCliente, intToString(QuienDevuelveElHandshake), 1);
}

int obtenerHandshake(int socketAlQueMeconecte) {
	char* headerHandshake = recibirMensaje(socketAlQueMeconecte, 4, MSG_WAITALL);

	if(stringToInt(headerHandshake) == HEADER_HANDSHAKE) {
		return stringToInt(recibirMensaje(socketAlQueMeconecte, 1, MSG_WAITALL));
	}
	else {
		return -1; //Error
	}
}
