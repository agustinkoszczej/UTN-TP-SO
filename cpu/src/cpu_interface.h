#ifndef CPU_INTERFACE_H_
#define CPU_INTERFACE_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <socket.h>
#include <pcb.h>

int mem_socket;
int kernel_socket;

/*
void server_identify(socket_connection * connection, char ** args);*/
void server_connectionClosed(socket_connection * connection);
/*
void server_handshake(socket_connection * connection, char ** args);
void server_print_message(socket_connection * connection, char ** args);
*/

/*
 * MEMORY
 */
void memory_identify(socket_connection* connection, char** args);

/*
 * CPU
 */

void kernel_execute_program(pcb* pcb, int quantum);

#endif /* CPU_INTERFACE_H_ */
