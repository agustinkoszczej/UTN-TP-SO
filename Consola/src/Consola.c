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

struct sockaddr_in *direccionServidor(){
	struct sockaddr_in *retorno = malloc(sizeof(struct sockaddr_in));

	retorno->sin_family = AF_INET;
	retorno->sin_addr.s_addr = inet_addr(consola_config.IP_KERNEL);
	retorno->sin_port = htons(consola_config.PUERTO_KERNEL);

	return retorno;
}

char* recibirMensaje(int socket,int tamanioMsj,int criterio) {
	char* buffer = malloc(tamanioMsj);

	int bytesRecibidos = recv(socket, buffer, tamanioMsj, criterio);

	if(bytesRecibidos <= 0) {
		printf("El Cliente %d se desconecto\n",socket);
	}
	buffer[tamanioMsj] = '\0';
	return buffer;

}

int obtenerHandshake(int socketAlQueMeconecte) {
	char* headerHandshake = recibirMensaje(socketAlQueMeconecte, 4, MSG_WAITALL);

	if(stringToInt(headerHandshake) == HEADER_HANDSHAKE) {
		return stringToInt(recibirMensaje(socketAlQueMeconecte, 1, MSG_WAITALL));
	}
	else {
		return 1; //Error
	}
}

void iniciarHandshake(t_handshake QuienIniciaElHandshake, t_handshake QuienDevuelveHandshake, int socket) {

	enviarHeader(socket, HEADER_HANDSHAKE);  //ENVIO EL HEADER DEL HANDSHAKE PARA QUE EL PROCESO QUE LO RECIBE VERIFIQUE SI ES UN HANDSHAKE LO QUE LE ENVIO O NO
	logger("Enviado mensaje de prueba de conexion al Kernel", "TRACE", NOMBRE_PROCESO);
	char* handshake = intToString(QuienIniciaElHandshake);
	enviarMensaje(socket, handshake, 1);		//ENVIO AL SOCKET QUE ME CONECTO EL VALOR DE QUIEN INICIA EL HANDSHAKE (EN ESTE CASO ES UN 4 QUE REPRESENTA A LA CONSOLA)

	if(obtenerHandshake(socket) == QuienDevuelveHandshake) {   //OBTENGO EL HANDSHAKE DEL PROCESO AL QUE LE ENVIE ANTERIORMENTE EL HANDSHAKE (KERNEL) Y LO COMPARO
		printf("\nSe conecto satisfactoriamente al Kernel\n");
	}

}

void enviarMensajeConTamanioDinamico(int socketCliente, char* msj) {
 char* longitudMensaje = string_new();
 longitudMensaje = intToString(strlen(msj));

 //enviarMensaje(socketCliente, longitudMensaje, MSJ_DINAMICO); //LE ENVIO PRIMERO EL TAMAÑO DEL MENSAJE A ENVIAR
 enviarHeader(socketCliente, 1);
 enviarMensaje(socketCliente, msj, 50); //LE ENVIO EL MENSAJE DE ESE TAMAÑO
}

void conectarAKernel(){

	socketConsola = getSocket();

	struct sockaddr_in *VdireccionServidor = direccionServidor();
	if(connect(socketConsola,(struct sockaddr*) VdireccionServidor, sizeof(*VdireccionServidor)) != 0){
		perror("Error en el connect");																		//ERROR
		logger("Error en el connect", "ERROR", NOMBRE_PROCESO);
	}

	iniciarHandshake(CONSOLA,KERNEL,socketConsola); //INICIO EL HANDSHAKE

		 while(1){
		  char* mensaje;
		  puts("Consola: Ingrese mensaje a enviar Kernel:");
		  scanf("%s", mensaje);
		  enviarMensajeConTamanioDinamico(socketConsola, mensaje);
		 }
}

int main() {
	printf("Iniciando Consola...\n\n");
	logger("Iniciando Consola", "INFO", NOMBRE_PROCESO);

	//CARGAR ARCHIVO DE CONFIGURACIÓN
	cargarConfigConsola();

	//MOSTRAR ARCHIVO DE CONFIGURACIÓN
	mostrarConfigConsola();

	conectarAKernel();



	return EXIT_SUCCESS;
}
