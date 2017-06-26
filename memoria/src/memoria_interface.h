#ifndef MEMORIA_INTERFACE_H_
#define MEMORIA_INTERFACE_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <socket.h>
#include <math.h>
#include "memoria.h"

typedef struct {
	t_list* cpu_sockets;
	int k_socket;
} memory_sockets;
memory_sockets m_sockets;

pthread_mutex_t cpu_sockets_mutex;

void kernel_stack_size(socket_connection* connection, char** args);
void i_get_page_from_pointer(socket_connection* connection, char** args);
void i_get_frame_from_pid_and_page(socket_connection* connection, char** args);
unsigned int hash(int pid, int page);

/*
 * INTERFACE
 */
void i_start_program(socket_connection* connection, char** args);
void i_read_bytes_from_page(socket_connection* connection, char** args);
void i_store_bytes_in_page(socket_connection* connection, char** args);
void i_add_pages_to_program(socket_connection* connection, char** args);
void i_finish_program(socket_connection* connection, char** args);
void i_free_page(socket_connection* connection, char** args);
/*
 * CLIENT
 */
void newClient(socket_connection* connection);
void connectionClosed(socket_connection* connection);
/*
 * SERVER
 */
void client_identify(socket_connection* connection, char** args);

#endif /* MEMORIA_INTERFACE_H_ */
