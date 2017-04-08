/*
 * ServerManager2.c
 *
 *  Created on: 6/4/2017
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
#include "Multiplexor.h"

//TODOS ESTOS VAN AL ARCHIVO DE CONFIGURACION
#define PORT "9034"   // port we're listening on
#define IP
//

struct sockaddr_in CrearDireccionServer(){
	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = INADDR_ANY;
	direccionServidor.sin_port = htons(8080);
	return direccionServidor;
}

void permitirReutilizarPuerto(int servidor) {
	int activado = 1;
	setsockopt(servidor, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));
}

void AlAceptarConexion(int cliente){
	printf("Recibí una conexión en %d!!\n", cliente);
	send(cliente, "Hola NetCat!", 13, 0);
	send(cliente, ":)\n", 4, 0);
}

ResultWithValue AceptarConexion(int servidor){
	struct sockaddr_in direccionCliente;
	unsigned int tamanioDireccion = sizeof(struct sockaddr_in);

	int cliente = accept(servidor, (void*) &direccionCliente, &tamanioDireccion);

	if(cliente == -1)
		return ErrorWithValue(strerror("accept"), NULL);

	AlAceptarConexion(cliente);

	return OkWithValue(cliente);
}

/*ResultWithValue RecibirMensaje(int cliente){
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
}*/

Result SetupServer(){
	int servidor = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in direccionServer = CrearDireccionServer();
	permitirReutilizarPuerto(servidor);

	if (bind(servidor, (void*) &direccionServer, sizeof(direccionServer)) != 0) {
		return Error("Fallo el bind");
	}

	printf("Estoy escuchando\n");
	 	listen(servidor, 100);

	Multiplexar(servidor);

	return Ok();

}





