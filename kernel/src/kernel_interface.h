#ifndef KERNEL_INTERFACE_H_
#define KERNEL_INTERFACE_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/string.h>
#include <socket.h>
#include <pcb.h>
#include <cJSON.h>
#include "kernel.h"
#include "kernel_console.h"
#include "kernel_memory.h"

int port_cpu;
int port_con;
int mem_socket;
int fs_socket;

/*
 * CONSOLE
 */
void console_load_program(socket_connection* connection, char** args);

/*
 * MEMORY
 */
void memory_identify(socket_connection* connection, char** args);
void memory_response_start_program(socket_connection* connection, char** args);
void memory_page_size(socket_connection* connection, char** args);

/*
 * SERVER
 */
void server_connectionClosed(socket_connection* connection);

/*
 * CLIENT
 */
void newClient(socket_connection* connection);
void connectionClosed(socket_connection* connection);

#endif /* KERNEL_INTERFACE_H_*/
