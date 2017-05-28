#include "consola_interface.h"

void server_connectionClosed(socket_connection* connection) {
	//log_debug(logger, "KERNEL has disconnected. Socket = %d, IP = %s, Port = %d.\n", connection->socket, connection->ip, connection->port);
	//exit(EXIT_FAILURE);
}

void kernel_stop_process(socket_connection* connection, char** args) {
	int pid = atoi(args[0]);
	int exit_code = atoi(args[1]);

	t_process* process = find_process_by_pid(pid);

	abort_program(process, exit_code);
}

void kernel_print_message(socket_connection* connection, char** args) {
	char* sender = args[0];
	char* message = args[1];
	int pid = atoi(args[2]);

	if (!strcmp(sender, KERNEL))
		new_message(message, pid);
}

void kernel_response_load_program(socket_connection* connection, char** args) {
	int response = atoi(args[0]);
	int pid = atoi(args[1]);

	t_process* process = find_process_by_socket(connection->socket);

	if(response == NO_ERRORES) {
		pthread_mutex_lock(&p_counter_mutex);
		p_counter++;
		pthread_mutex_unlock(&p_counter_mutex);

		process->pid = pid;
		//log_debug(logger, "Program loaded successfully.");
		new_message("Started program.", pid);
	} else {
		//log_debug(logger, "Program failed at loading.");
		remove_from_process_list(process);
		abort_program(process, response);
	}
}
