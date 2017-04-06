/*
 * ServerManager.c
 *
 *  Created on: 2/4/2017
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


//TODOS ESTOS VAN AL ARCHIVO DE CONFIGURACION
#define PORT "9034"   // port we're listening on
#define IP


// ----

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

struct addrinfo getAddrInfo(struct addrinfo** addrReference){
	struct addrinfo hints;
	int errorCode = 0;

	memset(&hints, 0, sizeof hints);

	hints.ai_family 	= AF_UNSPEC;
	hints.ai_socktype 	= SOCK_STREAM;
	hints.ai_flags 		= AI_PASSIVE;

	if ((errorCode = getaddrinfo(NULL, PORT, &hints, addrReference)) != 0) {
		fprintf(stderr, "selectserver: %s\n", gai_strerror(errorCode));
		exit(1);
	}

	return hints;
}

void ClearSet(fd_set set){
	FD_ZERO(&set);
}


int CreateSocketsAndBind(struct addrinfo *ai,struct addrinfo *p){
	bool yes = true; // for setsockopt() SO_REUSEADDR, below
	int listener;

	for(p = ai; p != NULL; p = p->ai_next) {
		listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (listener < 0) {
			continue;
		}

		// lose the pesky "address already in use" error message
		setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

		if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
			close(listener);
			continue;
		}

		break;
	}

	return listener;
}

void OnNewConnection(int listener, int fdmax, fd_set master){
	socklen_t addrlen;
	int newfd;        // newly accept()ed socket descriptor
	struct sockaddr_storage remoteaddr; // client address
	char remoteIP[INET6_ADDRSTRLEN];


	addrlen = sizeof remoteaddr;
	newfd = accept(listener,
			(struct sockaddr *)&remoteaddr,
			&addrlen);

	if (newfd == -1) {
		perror("accept");
	} else {
		FD_SET(newfd, &master); // add to master set
		if (newfd > fdmax) {    // keep track of the max
			fdmax = newfd;
		}
		printf("selectserver: new connection from %s on "
				"socket %d\n",
				inet_ntop(remoteaddr.ss_family,
						get_in_addr((struct sockaddr*)&remoteaddr),
						remoteIP, INET6_ADDRSTRLEN),
						newfd);
	}
}

void OnRecievingClosed(int socketfd){
	printf("selectserver: socket %d hung up\n", socketfd);
}

void OnRecievingError(){
	perror("recv");
}

void OnRecieved(int listener, int fdmax, int i, int nbytes, char buf[256], fd_set master){
	int j;

	for(j = 0; j <= fdmax; j++) {
		// send to everyone!
		if (FD_ISSET(j, &master)) {
			// except the listener and ourselves
			if (j != listener && j != i) {
				if (send(j, buf, nbytes, 0) == -1) {
					perror("send");
				}
			}
		}
	}
}

void OnRecieving(int listener, int fdmax, int i, fd_set master){
	int nbytes;
	char buf[256];    // buffer for client data

	// handle data from a client
	if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {
		// got error or connection closed by client
		if (nbytes == 0) {
			// connection closed
			OnRecievingClosed(i);
		} else {
			OnRecievingError();
		}
		close(i); // bye!
		FD_CLR(i, &master); // remove from master set
	} else {
		// we got some data from a client
		OnRecieved(listener, fdmax, i, nbytes,buf, master);
	}
}

void ReadDataInSockets(int listener, int fdmax, fd_set read_fds,fd_set master){
	int i;

	// run through the existing connections looking for data to read
	for(i = 0; i <= fdmax; i++) {
		if (FD_ISSET(i, &read_fds)) { // we got one!!
			if (i == listener)
				OnNewConnection(listener,fdmax,master); // handle new connections
			else {
				OnRecieving(listener,fdmax,i,master);
			} // END handle data from client
		} // END got new incoming connection
	} // END looping through file descriptors
}

void SetupServer(){
	fd_set master;    // master file descriptor list
	fd_set read_fds;  // temp file descriptor list for select()
	int fdmax;        // maximum file descriptor number

	struct addrinfo *ai, *p;

	ClearSet(master);    // clear the master and temp sets
	ClearSet(read_fds);

	// get us a socket and bind it
	getAddrInfo(&ai);

	//CreateSocketsAndBind(struct addrinfo *ai,struct addrinfo *p, int listener)

	int listener = CreateSocketsAndBind(ai,p);


	// if we got here, it means we didn't get bound
	if (p == NULL) {
		fprintf(stderr, "selectserver: failed to bind\n");
		exit(2);
	}

	freeaddrinfo(ai); // all done with this

	// listen
	if (listen(listener, 10) == -1) {
		perror("listen");
		exit(3);
	}

	// add the listener to the master set
	FD_SET(listener, &master);

	// keep track of the biggest file descriptor
	fdmax = listener; // so far, it's this one

	// main loop
	for(;;) {
		read_fds = master; // copy it
		if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
			perror("select");
			exit(4);
		}

		ReadDataInSockets(listener, fdmax, read_fds,master);
	}
}
