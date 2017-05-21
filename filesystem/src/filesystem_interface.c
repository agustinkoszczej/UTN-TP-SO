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
