#ifndef SOCKET_H_
#define SOCKET_H_

#include <commons/log.h>
#include <commons/collections/dictionary.h>
#define KERNEL "KERNEL"
#define CONSOLE "CONSOLE"
#define CPU "CPU"
#define FILESYSTEM "FILESYSTEM"
#define MEMORY "MEMORY"

typedef struct {
	int socket;
	char * ip;
	int port;
	void * data;
	bool run_fn_connectionClosed;
} socket_connection;

typedef struct {
	socket_connection * connection;
	t_dictionary * fns_receipts;
	void (*fn_connectionClosed)();
} args_receiptMessage;

char* receive_dynamic_message(int socket);
void send_dynamic_message(int socket, char* msj);

void receiptMessage(void * arguments);

//envia un mensaje a un socket
bool sendMessage(int socket, char * message);

//Corre una funcion de otro proceso
bool runFunction(int socket, char * name, int n, ...);

void sigchld_handler(int s);

int connectServer(char * ip, int port, t_dictionary * fns, void (*fn_connectionClosed)(), void * data);

typedef struct {
	int port;
	int socket_client;
	void (*fn_newClient)();
	t_dictionary * fns_receipts;
	void (*fn_connectionClosed)();
	void * data;
} args_listenClients;

void listenClients(void * arguments);

int createListen(int port, void (*fn_newClient)(), t_dictionary * fns, void (*fn_connectionClosed)(), void * data);

#endif
