/*
 * ServerManager.c
 *
 *  Created on: 8/4/2017
 *      Author: utnso
 */


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
#include "Results.h"

//TODOS ESTOS VAN AL ARCHIVO DE CONFIGURACION
#define PORT "9034"   // port we're listening on
#define IP
//

struct sockaddr_in CrearDireccionServer(){
	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = INADDR_ANY;
	direccionServidor.sin_port = htons(5002);		//5002, lo saque del archivo Kernel/Kernel.cfg
	return direccionServidor;
}

void permitirReutilizarPuerto(int servidor) {
	int activado = 1;
	setsockopt(servidor, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));
}



ResultWithValue AceptarConexion(int servidor){
	struct sockaddr_in direccionCliente;
	unsigned int tamanioDireccion = sizeof(struct sockaddr_in);

	int cliente = accept(servidor, (void*) &direccionCliente, &tamanioDireccion);

	if(cliente == -1)
		return ErrorWithValue(strerror("accept"), NULL);

	return OkWithValue(cliente);
}

void AlRecibirMensaje(char* buffer, int bytesRecibidos){
	buffer[bytesRecibidos] = '\0';
	printf("Me llegaron %d bytes con %s", bytesRecibidos, buffer);
}

ResultWithValue RecibirMensaje(int cliente){
	uint32_t tamanio;
	recv(cliente, &tamanio, 4, 0);

	char* buffer = malloc(tamanio);

	int bytesRecibidos = recv(cliente, buffer, tamanio, MSG_WAITALL);

	if (bytesRecibidos < 0) {
		free(buffer);
		return ErrorWithValue(strerror("recv"), NULL);
	}

	AlRecibirMensaje(buffer, bytesRecibidos);

	free(buffer);

	return OkWithValue(NULL);
}

int getSocket(){
	return socket(AF_INET, SOCK_STREAM,0);
}

Result SetupServer(){
	int servidorFS = getSocket();

	struct sockaddr_in direccionServer = CrearDireccionServer();
	permitirReutilizarPuerto(servidorFS);

	if (bind(servidorFS, (void*) &direccionServer, sizeof(direccionServer)) != 0) {
		return Error("Fallo el bind");
	}

	printf("Estoy escuchando\n");
	 	listen(servidorFS, 100);

	//------------------------------

	 ResultWithValue r = AceptarConexion(servidorFS);

	//------------------------------

	 if(r.value != NULL)
		 RecibirMensaje(r.value);
	 else
		 return r.result;

	return Ok();

}
