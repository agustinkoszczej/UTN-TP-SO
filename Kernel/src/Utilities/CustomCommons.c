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
#include <commons/collections/list.h>


ResultWithValue RecibirMensaje(int cliente, void AlRecibirMensaje(int,char*,int)){
	int tamanio = 4;

	char* buffer = malloc(tamanio);

	int bytesRecibidos = recv(cliente, buffer, tamanio, MSG_WAITALL);

	if (bytesRecibidos <= 0) {
		free(buffer);
		printf("El cliente %d se desconecto!\n",cliente);
		close(cliente);
		return OkWithValue(-1);
	}else{
		AlRecibirMensaje(cliente, buffer, bytesRecibidos);

		free(buffer);
	}

	return OkWithValue(NULL);
}

void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

bool contains(t_list* list, void* value){

	bool equals(void* item) {
		return item == value;
	}

	return list_any_satisfy(list, equals);
}

void PrintClientData(struct sockaddr_storage remoteaddr,int newfd,char remoteIP[INET6_ADDRSTRLEN]){
	printf("selectserver: new connection from %s on "
			"socket %d\n",
			inet_ntop(remoteaddr.ss_family,
				get_in_addr((struct sockaddr*)&remoteaddr),
				remoteIP, INET6_ADDRSTRLEN),
			newfd);
}

