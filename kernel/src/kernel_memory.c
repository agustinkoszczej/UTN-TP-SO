#include "kernel_memory.h"

/*
void kernel_send_program_to_memory() {
	char* buffer;
	char* pid = string_itoa(program_struct.pcb->pid);
	char* program = program_struct.program;
	int page_counter = program_struct.pcb->page_c;
	int offset = mem_page_size * page_counter;

	if (string_length(program) - offset > 0) {
		buffer = string_substring(program, offset, mem_page_size);
		program_struct.pcb->page_c++;
		runFunction(k_sockets.mem_socket, "kernel_ask_store_page", 3, KERNEL, buffer, pid);
	} else {
		pthread_mutex_lock(&pcb_list_mutex);
		pcb* new_pcb = queue_pop(new_queue);
		list_add(ready_list, new_pcb);
		pthread_mutex_unlock(&pcb_list_mutex);

		t_socket_pcb* socket_pcb = malloc(sizeof(t_socket_pcb));
		socket_pcb->pcb = program_struct.pcb;
		socket_pcb->socket = program_struct.socket;
		pthread_mutex_lock(&socket_pcb_mutex);
		list_add(socket_pcb_list, socket_pcb);
		pthread_mutex_unlock(&socket_pcb_mutex);

		kernel_send_response_to_console(true);
	}
}
*/
