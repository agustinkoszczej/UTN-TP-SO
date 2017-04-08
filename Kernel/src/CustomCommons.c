/*
 * CustomCommons.c
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


ResultWithValue RecibirMensaje(int cliente, void AlRecibirMensaje(char*,int)){
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
