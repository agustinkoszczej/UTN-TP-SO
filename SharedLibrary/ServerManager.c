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

struct sockaddr_in CrearDireccionServer(int puerto){ //Direccion local Kernel
	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = INADDR_ANY;
	direccionServidor.sin_port = htons(puerto);
	return direccionServidor;
}

void permitirReutilizarPuerto(int servidor) { //Comun
	int activado = 1;
	setsockopt(servidor, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));
}


Result SetupServer(int puerto, Result AlLevantarServidor(int)){			//Comun
	int servidor = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in direccionServer = CrearDireccionServer(puerto);
	permitirReutilizarPuerto(servidor);

	if (bind(servidor, (void*) &direccionServer, sizeof(direccionServer)) != 0) {
		return Error(strerror("bind"));

	}


	logger("Setup Server OK", "INFO");

	listen(servidor, 100);
	printf("Estoy escuchando\n");

	Result r = AlLevantarServidor(servidor);

	return r;

}





