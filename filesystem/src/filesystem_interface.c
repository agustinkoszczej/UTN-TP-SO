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

void validateFile(socket_connection* connection, char** args) {
	pthread_mutex_lock(&request_mutex);
	char* path = args[0];
	FILE* file = fopen(path, "r");
	bool result = false;

	if(file) {
		result = true;
		fclose(file);
	}

	runFunction(connection->socket, "fs_validate_file", 1, string_itoa(result));
	pthread_mutex_unlock(&request_mutex);
}
void createFile(socket_connection* connection, char** args) {
	pthread_mutex_lock(&request_mutex);

	pthread_mutex_unlock(&request_mutex);
}
void deleteFile(socket_connection* connection, char** args) {
	pthread_mutex_lock(&request_mutex);

	pthread_mutex_unlock(&request_mutex);
}
void getData(socket_connection* connection, char** args) {
	pthread_mutex_lock(&request_mutex);

	pthread_mutex_unlock(&request_mutex);
}
void saveData(socket_connection* connection, char** args) {
	pthread_mutex_lock(&request_mutex);

	pthread_mutex_unlock(&request_mutex);
}
