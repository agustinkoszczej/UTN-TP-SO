#ifndef FILESYSTEM_INTERFACE_H_
#define FILESYSTEM_INTERFACE_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <socket.h>
#include "filesystem.h"

int k_socket;

void newClient(socket_connection* connection);
void connectionClosed(socket_connection* connection);

/*
 * KERNEL
 */
void kernel_validate_file(socket_connection* connection, char** args);
void kernel_create_file(socket_connection* connection, char** args);
void kernel_delete_file(socket_connection* connection, char** args);
void kernel_get_data(socket_connection* connection, char** args);
void kernel_save_data(socket_connection* connection, char** args);



#endif /* FILESYSTEM_INTERFACE_H_ */
