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
void memory_retard(socket_connection* connection, char** args) {
	log_debug(logger, "memory_retard: void");

	mem_delay = atoi(args[0]);
}
void memory_response_read_bytes_from_page(socket_connection* connection, char** args) {
	mem_buffer = string_new();
	string_append(&mem_buffer, args[0]);
	if(mem_buffer[string_length(mem_buffer)-1] == '#'){
		mem_buffer[string_length(mem_buffer)-1] = '\0';
	}
	char* aux = string_new();
	aux = args[0];
	int i;
	for (i=0 ; i<sizeof(int); i++){
		if (aux[i] == '#'){
			aux[i] = '\0';
		}
	}
	mem_value = char4ToInt(aux);

	//log_debug(logger, "memory_response_read_bytes_from_page: buffer=%s", mem_buffer);
	signal_response(&planning_mutex);
}
void memory_response_store_bytes_in_page(socket_connection* connection, char** args) {

	signal_response(&planning_mutex);
}
void memory_response_get_frame_from_pid_and_page(socket_connection* connection, char** args) {
	frame_from_pid_and_page = atoi(args[0]);
	signal_response(&planning_mutex);
}

void memory_response_get_page_from_pointer(socket_connection* connection, char** args) {
	log_debug(logger, "memory_response_get_page_from_pointer: page_from_pointer=%s", args[0]);

	page_from_pointer = atoi(args[0]);
	signal_response(&planning_mutex);
}

/*
 * KERNEL
 */
void kernel_update_quantum_sleep(socket_connection* connection, char** args){
	quantum_sleep = atoi(args[0]);
	log_debug(logger, "kernel_update_quantum_sleep: quantum_sleep: '%d'", quantum_sleep);
}
void kernel_response_set_shared_var(socket_connection* connection, char** args) {
	log_debug(logger, "kernel_response_set_shared_var: void");

	signal_response(&planning_mutex);
}
void kernel_response_get_shared_var(socket_connection* connection, char** args) {
	log_debug(logger, "kernel_response_get_shared_var: buffer=%s", args[0]);

	kernel_shared_var = atoi(args[0]);
	signal_response(&planning_mutex);
}
void kernel_page_stack_size(socket_connection* connection, char** args) {
	int mem_page_size = atoi(args[0]);
	int stack_s = atoi(args[1]);

	log_debug(logger, "kernel_quantum_page_stack_size: mem_page_size=%d, stack_size=%d",mem_page_size, stack_s);

	frame_size = mem_page_size;
	stack_size = stack_s;
	runFunction(connection->socket, "cpu_received_page_stack_size", 0);
}
void kernel_receive_pcb(socket_connection* connection, char** args) {
	int planning_alg = atoi(args[0]);
	int quantum = atoi(args[1]);
	pcb_actual = string_to_pcb(args[2]);
	quantum_sleep = atoi(args[3]);
	finished = false;
	is_locked = false;
	log_debug(logger, "kernel_receive_pcb: planning_alg=%d, quantum=%d, pcb*", planning_alg, quantum);

	printf("> EXECUTING_PID: %d\n", pcb_actual->pid);
	while ((planning_alg == FIFO || quantum-- >= 0) && !finished) {
		runFunction(kernel_socket, "cpu_has_aborted", 1, string_itoa(pcb_actual->pid));
		aborted_status = atoi(receive_dynamic_message(kernel_socket));

		if (aborted_status<0) {
			if (aborted_status == FINALIZADO_CONSOLA) break;
		}

		t_intructions* i_code = list_get(pcb_actual->i_code, pcb_actual->pc);
		int start = i_code->start;
		int offset = i_code->offset;
		int pid = pcb_actual->pid;

		int n_page = start / frame_size;
		int n_offset = start - frame_size * n_page;
		int n_size = offset;

		log_debug(logger, "\ni_read_bytes_from_page: pid: '%d', n_page: '%d', n_offset: '%d', n_size: '%d'", pid, n_page, n_offset, n_size);
		runFunction(mem_socket, "i_read_bytes_from_page", 4, string_itoa(pid), string_itoa(n_page), string_itoa(n_offset), string_itoa(n_size));
		wait_response(&planning_mutex);

		string_trim(&mem_buffer);
		log_debug(logger, "INSTRUCCION LEIDA: %s", mem_buffer);
		printf("Executing: %s\n", mem_buffer);
		analizadorLinea(mem_buffer, &functions, &kernel_functions);
		pcb_actual->pc++;
		pcb_actual->statistics.cycles++;

		runFunction(kernel_socket, "cpu_has_quantum_changed", 0);
		quantum_sleep = atoi(receive_dynamic_message(kernel_socket));

		sleep(quantum_sleep / 1000);
		free(mem_buffer);
	}

	if(aborted_status < 0){
		pcb_actual->exit_code = aborted_status;
		finished = true;
	}

	printf("Finished executing.\n\n");
	log_debug(logger, "cpu_task_finished");
	runFunction(kernel_socket, "cpu_task_finished", 3, pcb_to_string(pcb_actual), string_itoa(finished), string_itoa(is_locked));
}
void kernel_response_malloc_pointer(socket_connection* connection, char** args) {
	log_debug(logger, "kernel_response_malloc_pointer: malloc_pointer=%s", args[0]);

	//*malloc_pointer = atoi(args[0]);
	signal_response(&planning_mutex);
}
void kernel_response_file(socket_connection* connection, char** args) {
	log_debug(logger, "kernel_response_file: kernel_file_descriptor=%s", args[0]);

	//kernel_file_descriptor = atoi(args[0]);
	signal_response(&planning_mutex);
}
void kernel_response(socket_connection* connection, char** args) {
	signal_response(&planning_mutex);
}
void kernel_response_validate_file(socket_connection* connection, char** args) {
	log_debug(logger, "kernel_response_file: kernel_file_descriptor=%s", args[0]);

	//validate_file = atoi(args[0]);
	signal_response(&planning_mutex);
}
void kernel_response_read_file(socket_connection* connection, char** args) {
	read_info = args[0];
	//kernel_file_descriptor = atoi(args[1]);
	signal_response(&planning_mutex);
}
