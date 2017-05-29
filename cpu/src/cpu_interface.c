#include "cpu_interface.h"

void server_connectionClosed(socket_connection * connection) {
	printf("Server has disconnected. Socket = %d, IP = %s, Port = %d.\n", connection->socket, connection->ip, connection->port);
	exit(EXIT_FAILURE);
}

/*
 * MEMORY
 */
void memory_identify(socket_connection* connection, char** args) {
	runFunction(connection->socket, "client_identify", 1, CPU);
}
void memory_response_read_bytes_from_page(socket_connection* connection, char** args) {
	mem_buffer = string_new();
	string_append(&mem_buffer, args[0]);
	signal_response();
}
void memory_response_store_bytes_in_page(socket_connection* connection, char** args) {
	signal_response();
}

/*
 * KERNEL
 */
void kernel_response_set_shared_var(socket_connection* connection, char** args) {
	signal_response();
}
void kernel_response_get_shared_var(socket_connection* connection, char** args) {
	kernel_shared_var = atoi(args[0]);
	signal_response();
}
void kernel_page_stack_size(socket_connection* connection, char** args) {
	int mem_page_size = atoi(args[0]);
	int stack_s = atoi(args[1]);

	frame_size = mem_page_size;
	stack_size = stack_s;
	runFunction(connection->socket, "cpu_received_page_stack_size", 0);
}
void kernel_receive_pcb(socket_connection* connection, char** args) {
	int planning_alg = atoi(args[0]);
	int quantum = atoi(args[1]);
	pcb_actual = string_to_pcb(args[2]);
	finished = false;

	while ((planning_alg == FIFO || quantum-- >= 0) && !finished) {
		t_intructions* i_code = list_get(pcb_actual->i_code, pcb_actual->pc);
		int start = i_code->start;
		int offset = i_code->offset;
		int pid = pcb_actual->pid;

		int n_page = start / frame_size;
		int n_offset = start - frame_size * n_page;
		int n_size = offset;

		runFunction(mem_socket, "i_read_bytes_from_page", 4, string_itoa(pid), string_itoa(n_page), string_itoa(n_offset), string_itoa(n_size));
		wait_response();
		pcb_actual->pc++;
		analizadorLinea(mem_buffer, &functions, &kernel_functions);
	}

	runFunction(kernel_socket, "cpu_task_finished", 2, pcb_to_string(pcb_actual), string_itoa(finished));
}
