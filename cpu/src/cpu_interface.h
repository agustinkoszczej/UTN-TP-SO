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

#include "cpu.h"
#include "cpu_primitivas.h"


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
void memory_response_read_bytes_from_page(socket_connection* connection, char** args);
//char* conseguirDatosDeLaMemoria(char *programa, t_puntero_instruccion inicioDeLaInstruccion, t_size tamanio);

/*
 * CPU
 */

void kernel_receive_pcb(socket_connection* connection, char** args);

#endif /* CPU_INTERFACE_H_ */
