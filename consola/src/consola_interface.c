#include "consola_interface.h"

void server_connectionClosed(socket_connection* connection) {
	//log_debug(logger, "KERNEL has disconnected. Socket = %d, IP = %s, Port = %d.\n", connection->socket, connection->ip, connection->port);
	//exit(EXIT_FAILURE);
}

void kernel_stop_process(socket_connection* connection, char** args) {
	int pid = atoi(args[0]);
	int exit_code = atoi(args[1]);

	log_debug(logger, "kernel_stop_process: pid=%d, exit_code=%d", pid, exit_code);

	t_process* process = find_process_by_pid(pid);

	if (process != NULL)
		abort_program(process, exit_code);
}

void kernel_print_message(socket_connection* connection, char** args) {
	char* message = string_new();
	strcpy(message, args[0]);
	int pid = atoi(args[1]);

	log_debug(logger, "kernel_print_message: message=%s, pid=%d", message, pid);

	t_process* process = find_process_by_pid(pid);
	if(process != NULL) {
		process->c_message++;
		new_message(message, pid);
	}
}

void kernel_response_pid(socket_connection* connection, char** args) {
	int pid = atoi(args[0]);
	int list_pos = atoi(args[1]);

	t_process* process = list_get(process_list, list_pos);
	process->pid = pid;

	send_dynamic_message(connection->socket, "");
}

void kernel_response_load_program(socket_connection* connection, char** args) {
	int response = atoi(args[0]);
	int pid = atoi(args[1]);
	int list_pos = atoi(args[2]);

	log_debug(logger, "kernel_response_load_program: response=%d, pid=%d", response, pid);

	t_process* process = list_get(process_list, list_pos);
	if (response == NO_ERRORES) {
		pthread_mutex_lock(&p_counter_mutex);
		p_counter++;
		pthread_mutex_unlock(&p_counter_mutex);

		process->loaded = true;
		process->pid = pid;
		char* time_start = temporal_get_string_time();
		process->time_start = malloc(string_length(time_start));
		process->time_start = time_start;

		new_message("Started program.", pid);
	} else {
		abort_program(process, response);
	}
}
