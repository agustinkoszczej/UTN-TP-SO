#ifndef CPU_INTERFACE_H_
#define CPU_INTERFACE_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <socket.h>

void server_identify(socket_connection * connection, char ** args);
void server_connectionClosed(socket_connection * connection);
void server_handshake(socket_connection * connection, char ** args);
void server_print_message(socket_connection * connection, char ** args);

#endif /* CPU_INTERFACE_H_ */
