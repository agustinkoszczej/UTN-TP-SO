#include "kernel_console.h"

/*
void* kernel_create_pcb(void* arg) {
	pcb* new_pcb = malloc(sizeof(pcb));

	pthread_mutex_lock(&p_counter_mutex);
	new_pcb->pid = ++p_counter;
	pthread_mutex_unlock(&p_counter_mutex);

	new_pcb->page_c = 0;
	new_pcb->state = NEW_LIST;
	new_pcb->i_code = list_create();

	pthread_mutex_lock(&pcb_list_mutex);
	queue_push(new_queue, new_pcb);
	pthread_mutex_unlock(&pcb_list_mutex);
	program_struct.pcb = new_pcb;

	runFunction(k_sockets.mem_socket, "i_start_program", 2, string_itoa(new_pcb->pid), program_struct.program);
	return EXIT_SUCCESS;
}

void kernel_send_response_to_console(bool response) {
	runFunction(last_socket, "kernel_response_load_program", 2, string_itoa(response), string_itoa(p_counter));
	pthread_mutex_unlock(&console_mutex);
}
*/
