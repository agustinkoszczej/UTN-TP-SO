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

#include "../Results.h"
#include <commons/collections/list.h>
#include "../CustomCommons.h"
#include "../Handshake.h"
#include "../Headers.h"
#include "../Conversor.h"

t_list* listeners;
t_list* clients;
int fdmax;
int lastMax = 0;
fd_set rfds;
fd_set rfdsTemp;


void AddFdToMaster(int fd) {
	FD_SET(fd, &rfds);

	if (fd > fdmax) {    // keep track of the max
		lastMax = fdmax;
		fdmax = fd;
	}
}


void RemoveFdFromMaster(int fd) {
	FD_CLR(fd, &rfds);

	if (fd == fdmax) {    // keep track of the max
		fdmax = lastMax;
	}
}

void AddListenerToMaster(int fd) {
	AddFdToMaster(fd);

	list_add(listeners, fd);
}

void AddClientToMaster(int fd) {
	AddFdToMaster(fd);

	list_add(clients, fd);
}

void RemoveClientFromMaster(int fd){
	RemoveFdFromMaster(fd);

	list_remove(clients,fd);
}

void ResetSet(int listener) {
	FD_ZERO(&rfds);
	AddListenerToMaster(listener);
}

ResultWithValue SelectReaders() {
	int retval = select(fdmax + 1, &rfdsTemp, NULL, NULL, NULL);

	if (retval == -1)
		return ErrorWithValue(strerror("select"), NULL);
	else
		return OkWithValue(retval);
}

bool isListener(int fd) {
	return contains(listeners, fd);
}

bool isClient(int fd) {
	return contains(clients, fd);
}

ResultWithValue GetNewConnection(int listener) {
	int newfd;        // newly accept()ed socket descriptor
	struct sockaddr_storage remoteaddr; // client address

	char remoteIP[INET6_ADDRSTRLEN];

	// handle new connections
	socklen_t addrlen;

	addrlen = sizeof remoteaddr;

	newfd = accept(listener, (struct sockaddr *) &remoteaddr, &addrlen);

	if (newfd == -1)
		return ErrorWithValue(strerror("accept"), NULL);

	AddClientToMaster(newfd);
	PrintClientData(remoteaddr, newfd, remoteIP);


	return OkWithValue(NULL);
}

void AlRecibirHandshake(int cliente, char* buffer) {
	int tamanio = sizeof(char);
	char* handshake = malloc(tamanio);

	int bytesRecibidos = recv(cliente, handshake, tamanio, MSG_WAITALL);

	if (bytesRecibidos <= 0) {
		free(buffer);
		printf("El cliente %d se desconecto\n", cliente);
		close(cliente);
	} else {
		printf("Se conecto %s socket %d\n",textonombreProceso(stringToInt(handshake)), cliente);

		devolverHandshake(cliente, KERNEL);

	}
}

void AlRecibirPasamanos(int cliente, char* buffer) {
	char* msg = malloc(50);

	int bytesRecibidos = recv(cliente, msg, 50, 0);

	if (bytesRecibidos <= 0) {
		free(buffer);
		printf("El cliente %d se desconecto\n", cliente);
		close(cliente);
	} else {

		void ReplicarPasamanos(int cliente) {
			enviarPasamanos(cliente, msg);
		};

		list_iterate(clients, ReplicarPasamanos);

		puts(msg);
	}
}

void AlRecibirMensaje(int cliente, char* buffer, int bytesRecibidos) {
	buffer[bytesRecibidos] = '\0';

	char* header = buffer;
	int headerCode = stringToInt(header);

	switch (headerCode) {
	case HEADER_HANDSHAKE:
		AlRecibirHandshake(cliente, buffer);
		break;
	case HEADER_PASAMANOS:
		AlRecibirPasamanos(cliente, buffer);
		break;
	}

}

ResultWithValue CheckForIncomingData() {
	int i;
	ResultWithValue r;

	for (i = 0; i <= fdmax; i++) {
		if (FD_ISSET(i, &rfdsTemp)) {
			if (isListener(i)) {
				r = GetNewConnection(i);

				if (r.result.noError != true)
					return r;
			} else if (isClient(i)) {
				r = RecibirMensaje(i, AlRecibirMensaje);

				if(r.value == -1){
					RemoveClientFromMaster(i);
				}
			}
		}
	}

	return OkWithValue(NULL);
}

Result Multiplexar(int listener) {
	listeners = list_create();
	clients = list_create();

	ResultWithValue r;

	/* Watch stdin (fd 0) to see when it has input. */
	ResetSet(listener);

	fdmax = listener;

	for (;;) {
		rfdsTemp = rfds;

		r = SelectReaders();

		if (r.result.noError != true)
			return r.result;

		r = CheckForIncomingData();

		if (r.result.noError != true)
			return r.result;
	}

	return r.result;
}

