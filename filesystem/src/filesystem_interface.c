#include "filesystem_interface.h"

/*
 * FS AS SERVER
 */
void newClient(socket_connection * connection) {
	log_debug(logger, "newClient: socket = %d, ip = %s, port = %d.\n", connection->socket, connection->ip, connection->port);
	k_socket = connection->socket;
}

void connectionClosed(socket_connection * connection) {
	log_debug(logger, "connectionClosed: socket = %d, ip = %s, port = %d.\n", connection->socket, connection->ip, connection->port);
}

void kernel_validate_file(socket_connection* connection, char** args) {
	char* path = string_substring_from(args[0], 1);
	log_debug(logger, "validateFile: path=%s", path);

	pthread_mutex_lock(&request_mutex);

	bool result = validate_file(path);
	printf("VALIDAR: %s --> %s\n", path, result ? "true" : "false");
	runFunction(connection->socket, "fs_response_file", 1, string_itoa(result));

	pthread_mutex_unlock(&request_mutex);
}

void kernel_create_file(socket_connection* connection, char** args) {
	char* path = string_substring_from(args[0], 1);
	log_debug(logger, "kernel_create_file=path: %s", path);

	pthread_mutex_lock(&request_mutex);

	bool result = create_file(path);
	printf("CREAR: %s --> %s\n", path, result ? "true" : "false");
	runFunction(connection->socket, "fs_response_file", 1, string_itoa(result));

	pthread_mutex_unlock(&request_mutex);
}

void kernel_delete_file(socket_connection* connection, char** args) {
	char* path = string_substring_from(args[0], 1);
	log_debug(logger, "kernel_delete_file=path: %s", path);

	pthread_mutex_lock(&request_mutex);

	bool result = delete_file(path);
	printf("BORRAR: %s --> %s\n", path, result ? "true" : "false");
	runFunction(connection->socket, "fs_response_file", 1, string_itoa(result));

	pthread_mutex_unlock(&request_mutex);
}

void kernel_get_data(socket_connection* connection, char** args) {
	char* path = string_substring_from(args[0], 1);
	int offset = atoi(args[1]);
	int size = atoi(args[2]);
	log_debug(logger, "kernel_get_data: path=%s, offset=%d, size=%d", path, offset, size);

	pthread_mutex_lock(&request_mutex);

	char* result = get_data(path, offset, size);
	printf("LEER: %s\n", path);
	runFunction(connection->socket, "fs_response_get_data", 1, result);

	pthread_mutex_unlock(&request_mutex);
}

void kernel_save_data(socket_connection* connection, char** args) {
	char* path = string_substring_from(args[0], 1);
	int offset = atoi(args[1]);
	int size = atoi(args[2]);
	char* buffer = args[3];
	log_debug(logger, "kernel_get_data: path=%s, offset=%d, size=%d, buffer=%s", path, offset, size, buffer);

	pthread_mutex_lock(&request_mutex);

	bool result = save_data(path, offset, size, buffer);
	printf("ESCRIBIR: %s --> %s\n", path, result ? "true" : "false");
	runFunction(connection->socket, "fs_response_file", 1, string_itoa(result));

	pthread_mutex_unlock(&request_mutex);
}
