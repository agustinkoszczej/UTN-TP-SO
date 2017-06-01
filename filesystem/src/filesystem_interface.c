#include "filesystem_interface.h"

/*
 * FS AS SERVER
 */
void newClient(socket_connection * connection) {
	log_debug(logger, "KERNEL has connected. Socket = %d, IP = %s, Port = %d.\n", connection->socket, connection->ip, connection->port);
	k_socket = connection->socket;
}
void connectionClosed(socket_connection * connection) {
	log_debug(logger, "KERNEL has disconnected. Socket = %d, IP = %s, Port = %d.\n", connection->socket, connection->ip, connection->port);
}

void kernel_validate_file(socket_connection* connection, char** args) {
	pthread_mutex_lock(&request_mutex);
	char* path = args[0];

	bool result = validate_file(path);
	runFunction(connection->socket, "fs_response_file", 1, string_itoa(result));
	pthread_mutex_unlock(&request_mutex);
}
void kernel_create_file(socket_connection* connection, char** args) {
	pthread_mutex_lock(&request_mutex);
	char* path = args[0];

	bool result = create_file(path);
	runFunction(connection->socket, "fs_response_file", 1, string_itoa(result));
	pthread_mutex_unlock(&request_mutex);
}
void kernel_delete_file(socket_connection* connection, char** args) {
	pthread_mutex_lock(&request_mutex);
	char* path = args[0];

	bool result = delete_file(path);
	runFunction(connection->socket, "fs_response_file", 1, string_itoa(result));
	pthread_mutex_unlock(&request_mutex);
}
void kernel_get_data(socket_connection* connection, char** args) {
	pthread_mutex_lock(&request_mutex);
	char* path = args[0];
	int offset = atoi(args[1]);
	int size = atoi(args[2]);

	char* result = get_data(path, offset, size);
	runFunction(connection->socket, "fs_get_data", 1, result);
	pthread_mutex_unlock(&request_mutex);
}
void kernel_save_data(socket_connection* connection, char** args) {
	pthread_mutex_lock(&request_mutex);
	char* path = args[0];
	int offset = atoi(args[1]);
	int size = atoi(args[2]);
	char* buffer = args[3];

	bool result = save_data(path, offset, size, buffer);
	runFunction(connection->socket, "fs_save_data", 1, string_itoa(result));
	pthread_mutex_unlock(&request_mutex);
}


/*
 * KERNEL
 */
