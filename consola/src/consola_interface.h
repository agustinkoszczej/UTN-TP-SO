#ifndef CONSOLA_INTERFACE_H_
#define CONSOLA_INTERFACE_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <socket.h>
#include "consola.h"

char* ip;
int port;

void server_connectionClosed(socket_connection * connection);
void kernel_print_message(socket_connection * connection, char ** args);
void kernel_response_load_program(socket_connection * connection, char ** args);
void kernel_stop_process(socket_connection* connection, char** args);

#endif /* CONSOLA_INTERFACE_H_ */
