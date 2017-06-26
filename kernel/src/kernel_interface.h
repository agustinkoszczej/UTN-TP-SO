#ifndef KERNEL_INTERFACE_H_
#define KERNEL_INTERFACE_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <math.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/string.h>
#include <socket.h>
#include <pcb.h>
#include "kernel.h"
#include <parser/metadata_program.h>

int port_cpu;
int port_con;
int mem_socket;
int fs_socket;

/*
 * CONSOLE
 */
void console_load_program(socket_connection* connection, char** args);
void console_abort_program(socket_connection* connection, char** args);

/*
 * MEMORY
 */
void memory_identify(socket_connection* connection, char** args);
void memory_response_start_program(socket_connection* connection, char** args);
void memory_page_size(socket_connection* connection, char** args);
void memory_response_heap(socket_connection* connection, char** args);
void memory_response_store_bytes_in_page(socket_connection* connection, char** args);
void memory_response_read_bytes_from_page(socket_connection* connection, char** args);
void memory_response_get_page_from_pointer(socket_connection* connection, char** args);

/*
 * CPU
 */
void cpu_has_quantum_changed(socket_connection* connection, char** args);
void cpu_has_aborted(socket_connection* connection, char** args);
void cpu_received_page_stack_size(socket_connection* connection, char** args);
void cpu_get_shared_var(socket_connection* connection, char** args);
void cpu_set_shared_var(socket_connection* connection, char** args);
void cpu_task_finished(socket_connection* connection, char** args);

void cpu_wait_sem(socket_connection* connection, char** args);
void cpu_signal_sem(socket_connection* connection, char** args);
void cpu_malloc(socket_connection* connection, char** args);
void cpu_free(socket_connection* connection, char** args);
void cpu_open_file(socket_connection* connection, char** args);
void cpu_close_file(socket_connection* connection, char** args);
void cpu_delete_file(socket_connection* connection, char** args);
void cpu_write_file(socket_connection* connection, char** args);
void cpu_read_file(socket_connection* connection, char** args);
void cpu_seek_file(socket_connection* connection, char** args);
void cpu_validate_file(socket_connection* connection, char** args);

/*
 * FILESYSTEM
 */

void fs_response_file(socket_connection* connection, char** args);
void fs_response_get_data(socket_connection* connection, char** args);

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
