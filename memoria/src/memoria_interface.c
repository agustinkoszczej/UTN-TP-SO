#include "memoria_interface.h"

/*
 * INTERFACE
 */
void i_start_program(socket_connection* connection, char** args) {
	int pid = atoi(args[0]);
	char* buffer = args[1];


	int size_buffer = string_length(buffer);
	int n_frames = ceil((float) size_buffer / frame_size);

	if (has_available_frames(n_frames + stack_size)) {
		start_program(pid, n_frames);
		int i;
		for (i = 0; i < n_frames; i++) {
			int size = size_buffer < frame_size ? size_buffer : frame_size;
			char* buffer2 = string_substring(buffer, frame_size * i, size);
			store_bytes(pid, i, 0, size, buffer2);
		}

		runFunction(m_sockets.k_socket, "memory_response_start_program", 1, string_itoa(NO_ERRORES));
	} else
		runFunction(m_sockets.k_socket, "memory_response_start_program", 1, string_itoa(NO_SE_PUEDEN_RESERVAR_RECURSOS));
}
void i_read_bytes_from_page(socket_connection* connection, char** args) {
	int pid = atoi(args[0]);
	int page = atoi(args[1]);
	int offset = atoi(args[2]);
	int size = atoi(args[3]);

	char* buffer = read_bytes(pid, page, offset, size);

	log_debug(logger, "memory_response_read_bytes_from_page: '%s'", buffer);
	runFunction(connection->socket, "memory_response_read_bytes_from_page", 1, buffer);
}
void i_store_bytes_in_page(socket_connection* connection, char** args) {
	int pid = atoi(args[0]);
	int page = atoi(args[1]);
	int offset = atoi(args[2]);
	int size = atoi(args[3]);
	char* buffer = args[4];

	store_bytes(pid, page, offset, size, buffer);

	//TODO hacer que esta funcion me retorne el offset absoluto en memoria
	runFunction(connection->socket, "memory_response_store_bytes_in_page", 1, string_itoa(NO_ERRORES));
}
void i_add_pages_to_program(socket_connection* connection, char** args) {
	int pid = atoi(args[0]);
	int n_frames = atoi(args[1]);

	if (has_available_frames(n_frames)) {
		add_pages(pid, n_frames);

		runFunction(connection->socket, "memory_response_heap", 1, string_itoa(NO_ERRORES));
	} else
		runFunction(connection->socket, "memory_response_heap", 1, string_itoa(NO_SE_PUEDEN_RESERVAR_RECURSOS));
}
void i_finish_program(socket_connection* connection, char** args) {
	int pid = atoi(args[0]);
	finish_program(pid);
}

void i_free_page(socket_connection* connection, char** args) {
	int pid = atoi(args[0]);
	int page = atoi(args[0]);

	free_page(pid, page);
}

void kernel_stack_size(socket_connection* connection, char** args) {
	int stack_s = atoi(args[0]);

	stack_size = stack_s;
}

/*
 * SERVER
 */
void client_identify(socket_connection* connection, char** args) {
	char* sender = args[0];

	log_debug(logger, "%s has connected. Socket = %d, IP = %s, Port = %d.\n", args[0], connection->socket, connection->ip, connection->port);

	if (!strcmp(sender, CPU)) {
		pthread_mutex_lock(&cpu_sockets_mutex);
		list_add(m_sockets.cpu_sockets, &connection->socket);
		pthread_mutex_unlock(&cpu_sockets_mutex);
	} else if (!strcmp(sender, KERNEL)) {
		m_sockets.k_socket = connection->socket;
		runFunction(connection->socket, "memory_page_size", 1, string_itoa(frame_size));
	}
}

/*
 * CLIENT
 */
void newClient(socket_connection* connection) {
	runFunction(connection->socket, "memory_identify", 0);
}
void connectionClosed(socket_connection* connection) {
	char* client = (connection->socket == m_sockets.k_socket) ? KERNEL : CPU;
	log_debug(logger, "%s has disconnected. Socket = %d, IP = %s, Port = %d.\n", client, connection->socket, connection->ip, connection->port);

	if (!strcmp(client, CPU)) {
		pthread_mutex_lock(&cpu_sockets_mutex);
		int i;
		for (i = 0; i < list_size(m_sockets.cpu_sockets); i++) {
			int _socket = (int) list_get(m_sockets.cpu_sockets, i);
			if (_socket == connection->socket)
				list_remove(m_sockets.cpu_sockets, i);
		}
		pthread_mutex_unlock(&cpu_sockets_mutex);
	}
}
