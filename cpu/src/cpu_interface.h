#ifndef CPU_INTERFACE_H_
#define CPU_INTERFACE_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <socket.h>
#include <pcb.h>
#include <parser/parser.h>

#include "cpu.h"
#include "cpu_primitivas.h"

int mem_socket;
int kernel_socket;

void server_connectionClosed(socket_connection * connection);

/*
 * MEMORY
 */
void memory_identify(socket_connection* connection, char** args);
void memory_response_read_bytes_from_page(socket_connection* connection, char** args);
void memory_response_store_bytes_in_page(socket_connection* connection, char** args);

/*
 * KERNEL
 */
void kernel_receive_pcb(socket_connection* connection, char** args);
void kernel_page_stack_size(socket_connection* connection, char** args);
void kernel_response_get_shared_var(socket_connection* connection, char** args);
void kernel_response_set_shared_var(socket_connection* connection, char** args);
void kernel_response_file(socket_connection* connection, char** args);

#endif /* CPU_INTERFACE_H_ */
