#include "cpu_interface.h"

void server_connectionClosed(socket_connection * connection) {
	log_debug(logger, "server_connectionClosed: socket = %d, ip = %s, port = %d.\n", connection->socket, connection->ip, connection->port);

	printf("Server has disconnected. Socket = %d, IP = %s, Port = %d.\n", connection->socket, connection->ip, connection->port);
	exit(EXIT_FAILURE);
}

/*
 * MEMORY
 */
void memory_identify(socket_connection* connection, char** args) {
	log_debug(logger, "memory_identify: void");

	runFunction(connection->socket, "client_identify", 1, CPU);
}
void memory_response_read_bytes_from_page(socket_connection* connection, char** args) {
	log_debug(logger, "memory_response_read_bytes_from_page: buffer=%s", args[0]);

	mem_buffer = string_new();
	string_append(&mem_buffer, args[0]);
	signal_response();
}
void memory_response_store_bytes_in_page(socket_connection* connection, char** args) {
	log_debug(logger, "memory_response_store_bytes_in_page: void");

	signal_response();
}

/*
 * KERNEL
 */
void kernel_response_set_shared_var(socket_connection* connection, char** args) {
	log_debug(logger, "kernel_response_set_shared_var: void");

	signal_response();
}
void kernel_response_get_shared_var(socket_connection* connection, char** args) {
	log_debug(logger, "kernel_response_get_shared_var: buffer=%s", args[0]);

	kernel_shared_var = atoi(args[0]);
	signal_response();
}
void kernel_quantum_page_stack_size(socket_connection* connection, char** args) {
	int mem_page_size = atoi(args[0]);
	int stack_s = atoi(args[1]);
	int quantum_s = atoi(args[2]);

	log_debug(logger, "kernel_quantum_page_stack_size: quantum_sleep=%d, mem_page_size=%d, stack_size=%d", quantum_s, mem_page_size, stack_s);

	frame_size = mem_page_size;
	stack_size = stack_s;
	quantum_sleep = quantum_s;
	runFunction(connection->socket, "cpu_received_page_stack_size", 0);
}
void kernel_receive_pcb(socket_connection* connection, char** args) {
	int planning_alg = atoi(args[0]);
	int quantum = atoi(args[1]);
	pcb_actual = string_to_pcb(args[2]);
	quantum_sleep = atoi(args[3]);
	finished = false;
	log_debug(logger, "kernel_receive_pcb: planning_alg=%d, quantum=%d, pcb*", planning_alg, quantum);

	double percent_per_instruction = 100.0f / (double) (list_size(pcb_actual->i_code) - pcb_actual->pc);
	double acum_percent = 0;

	pthread_mutex_init(&planning_mutex, NULL);
	pthread_mutex_lock(&planning_mutex);

	printf("> EXECUTING_PID: %d\n", pcb_actual->pid);
	while ((planning_alg == FIFO || quantum-- >= 0) && !finished) {
		printf("[ %%%.2f ]\n", acum_percent);
		t_intructions* i_code = list_get(pcb_actual->i_code, pcb_actual->pc);
		int start = i_code->start;
		int offset = i_code->offset;
		int pid = pcb_actual->pid;

		int n_page = start / frame_size;
		int n_offset = start - frame_size * n_page;
		int n_size = offset;

		runFunction(mem_socket, "i_read_bytes_from_page", 4, string_itoa(pid), string_itoa(n_page), string_itoa(n_offset), string_itoa(n_size));
		wait_response();

		analizadorLinea(mem_buffer, &functions, &kernel_functions);
		pcb_actual->pc++;
		acum_percent += percent_per_instruction;
		sleep(quantum_sleep / 1000);
	}

	if (acum_percent == 100)
		printf("[ %%%.2f ]\n\n", acum_percent);

	log_debug(logger, "cpu_task_finished");

	runFunction(kernel_socket, "cpu_task_finished", 2, pcb_to_string(pcb_actual), string_itoa(finished));
}
void kernel_response_malloc_pointer(socket_connection* connection, char** args) {
	log_debug(logger, "kernel_response_malloc_pointer: malloc_pointer=%s", args[0]);

	*malloc_pointer = atoi(args[0]);
	signal_response();
}
void kernel_response_file(socket_connection* connection, char** args) {
	log_debug(logger, "kernel_response_file: kernel_file_descriptor=%s", args[0]);

	kernel_file_descriptor = atoi(args[0]);
	signal_response();
}
void kernel_response(socket_connection* connection, char** args) {
	signal_response();
}
void kernel_response_validate_file(socket_connection* connection, char** args) {
	log_debug(logger, "kernel_response_file: kernel_file_descriptor=%s", args[0]);

	validate_file = atoi(args[0]);
	signal_response();
}
void kernel_response_read_file(socket_connection* connection, char** args) {
	read_info = args[0];
	kernel_file_descriptor = atoi(args[1]);
	signal_response();
}
