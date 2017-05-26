#include "cpu_interface.h"

void server_identify(socket_connection * connection, char ** args) {
	runFunction(connection->socket, "client_handshake", 1, CPU);
}

void server_connectionClosed(socket_connection * connection) {
	printf("Server has disconnected. Socket = %d, IP = %s, Port = %d.\n", connection->socket, connection->ip, connection->port);
	exit(EXIT_FAILURE);
}

void server_handshake(socket_connection * connection, char ** args) {
	printf("Server (Socket = %d, IP = %s, Port = %d) allows connection.\n", connection->socket, connection->ip, connection->port);
}

void server_print_message(socket_connection * connection, char ** args) {
	printf("Message sent by %s says: %s\n", args[0], args[1]);
}
