#include "kernel_interface.h"

/*
 * CONSOLE
 */
void console_load_program(socket_connection* connection, char** args) {
	char* program = args[0];

	if (process_in_memory + 1 > multiprog) {
		runFunction(process_struct.socket, "kernel_response_load_program", 2,
				string_itoa(NO_SE_PUEDEN_RESERVAR_RECURSOS), string_itoa(-1));
		return;
	}

	pcb* new_pcb = malloc(sizeof(pcb));

	pthread_mutex_lock(&p_counter_mutex);
	new_pcb->pid = ++p_counter;
	pthread_mutex_unlock(&p_counter_mutex);

	new_pcb->page_c = ceil((double) string_length(program) / mem_page_size);
	new_pcb->state = NEW_LIST;
	new_pcb->i_code = list_create();
	new_pcb->i_label = dictionary_create();
	new_pcb->i_stack = list_create();

	t_stack* stack = malloc(sizeof(t_stack));
	stack->args = NULL;
	stack->vars = NULL;
	stack->retpos = NULL;
	stack->retvar = NULL;
	list_add(new_pcb->i_stack, stack);

	new_pcb->statistics.cycles = 0;
	new_pcb->statistics.op_priviliges = 0;

	t_metadata_program* metadata = metadata_desde_literal(program);

	new_pcb->pc = metadata->instruccion_inicio;
	int i;

	if (metadata->etiquetas_size > 0) {
		char** labels = string_split(metadata->etiquetas, "\0");
		for (i = 0; i < metadata->etiquetas_size; i++) {
			char* label = labels[i];
			t_puntero_instruccion* instruction = malloc(
					sizeof(t_puntero_instruccion));
			*instruction = metadata_buscar_etiqueta(label, metadata->etiquetas,
					metadata->etiquetas_size);
			dictionary_put(new_pcb->i_label, label, instruction);
		}
	}

	if (metadata->instrucciones_size > 0) {
		for (i = 0; i < metadata->instrucciones_size; i++) {
			t_intructions* instruction = malloc(sizeof(t_intructions));
			*instruction = (metadata->instrucciones_serializado)[i];
			list_add(new_pcb->i_code, instruction);
		}
	}

	metadata_destruir(metadata);

	pthread_mutex_lock(&console_mutex);

	pthread_mutex_lock(&pcb_list_mutex);
	queue_push(new_queue, new_pcb);
	pthread_mutex_unlock(&pcb_list_mutex);

	t_socket_pcb* socket_pcb = malloc(sizeof(t_socket_pcb));
	socket_pcb->pid = new_pcb->pid;
	socket_pcb->state = new_pcb->state;
	socket_pcb->socket = connection->socket;
	pthread_mutex_lock(&socket_pcb_mutex);
	list_add(socket_pcb_list, socket_pcb);
	pthread_mutex_unlock(&socket_pcb_mutex);

	process_struct.socket = connection->socket;
	process_struct.pid = new_pcb->pid;
	process_struct.state = new_pcb->state;

	runFunction(mem_socket, "i_start_program", 2, string_itoa(new_pcb->pid),
			program);
}

/*
 * CPU
 */
void cpu_received_page_stack_size(socket_connection* connection, char** args) {
	short_planning();
}
void cpu_get_shared_var(socket_connection* connection, char** args) {
	char* var_name = args[0];
	pthread_mutex_lock(&shared_vars_mutex);
	int value = atoi(dictionary_get(shared_vars, var_name));
	pthread_mutex_unlock(&shared_vars_mutex);

	runFunction(connection->socket, "kernel_response_get_shared_var", 1,
			string_itoa(value));
}
void cpu_set_shared_var(socket_connection* connection, char** args) {
	void free_var(void* v) {
		int* val = v;
		free(val);
	}

	char* var_name = args[0];
	int* var_value = malloc(sizeof(int));
	*var_value = atoi(args[1]);
	pthread_mutex_lock(&shared_vars_mutex);
	dictionary_remove_and_destroy(shared_vars, var_name, &free_var);
	dictionary_put(shared_vars, var_name, var_value);
	pthread_mutex_unlock(&shared_vars_mutex);

	runFunction(connection->socket, "kernel_response_set_shared_var", 0);
}
void set_new_pcb(pcb** o_pcb, pcb* n_pcb) {
	*o_pcb = n_pcb;
}
void cpu_task_finished(socket_connection* connection, char** args) {
	pcb* n_pcb = string_to_pcb(args[0]);
	bool finished = atoi(args[1]);

	pcb* o_pcb = find_pcb_by_pid(n_pcb->pid);
	set_new_pcb(&o_pcb, n_pcb);

	t_cpu* n_cpu = find_cpu_by_socket(connection->socket);
	n_cpu->busy = false;

	if (finished) {
		move_to_list(n_pcb, EXIT_LIST);
		substract_process_in_memory();
		runFunction(mem_socket, "i_finish_program", 1, string_itoa(n_pcb->pid));
		t_socket_pcb* socket_pcb = find_socket_by_pid(n_pcb->pid);
		runFunction(socket_pcb->socket, "kernel_stop_process", 2,
				string_itoa(n_pcb->pid), string_itoa(n_pcb->exit_code));
	} else {
		move_to_list(n_pcb, READY_LIST);
	}

	short_planning();
}

void cpu_error(socket_connection* connection, char** args) {

	switch (atoi(args[0])) {
	//TODO Falta settear el exit code para cada proceso y liberar la CPU
	case NO_EXISTE_ARCHIVO:
		log_debug(logger,
				"Error: No existe archivo. CPU socket: %d, IP = %s, Port = %d.\n",
				connection->socket, connection->ip, connection->port);
		break;

	case LEER_SIN_PERMISOS:
		log_debug(logger,
				"Error: Leer sin permisos. CPU socket: %d, IP = %s, Port = %d.\n",
				connection->socket, connection->ip, connection->port);
		break;

	case ESCRIBIR_SIN_PERMISOS:
		log_debug(logger,
				"Error: Escribir sin permisos. CPU socket: %d, IP = %s, Port = %d.\n",
				connection->socket, connection->ip, connection->port);
		break;

	case STACK_OVERFLOW:
		log_debug(logger,
				"Error: Stack Overflow. CPU socket: %d, IP = %s, Port = %d.\n",
				connection->socket, connection->ip, connection->port);
		break;
	}
}

void cpu_wait_sem(socket_connection* connection, char** args) {
	//TODO
}

void cpu_signal_sem(socket_connection* connection, char** args) {
	//TODO
}

void cpu_malloc(socket_connection* connection, char** args) {
	//TODO
}

void cpu_free(socket_connection* connection, char** args) {
	//TODO
}

void cpu_validate_file(socket_connection* connection, char** args) {
	char* path = args[0];
	bool validate = validate_file_from_fs(path);
	runFunction(connection->socket, "kernel_response_validate_file", 1,
			string_itoa(validate));
}

void cpu_open_file(socket_connection* connection, char** args) {
	char* path = args[0];
	char* flags = args[1];
	int pid = atoi(args[2]);

	if (!strcmp(flags, "c")) {
		runFunction(fs_socket, "kernel_create_file", 1, path);
		wait_response(fs_mutex);
		if (fs_response == 0) {
			//TODO aca llega cuando create_file = false, que no se que significa del lado de FileSystem xd
			return;
		}
	}

	int fd_assigned = open_file_in_process_table(path, flags, pid);

	runFunction(connection->socket, "kernel_response_file", 1,
			string_itoa(fd_assigned));
}

void cpu_delete_file(socket_connection* connection, char** args) {
	int gfd_delete = atoi(args[0]);

	bool result = delete_file_from_global_table(gfd_delete);
	if (!result) {
		runFunction(connection->socket, "kernel_response_file", 1,
				string_itoa(NO_EXISTE_ARCHIVO));
		return;
	}

	char* path = get_path_by_gfd(gfd_delete);

	runFunction(fs_socket, "kernel_delete_file", 1, path);
	wait_response(fs_mutex);
	if (fs_response == 0) {
		//TODO aca llega cuando delete_file = false, que no se que significa del lado de FileSystem xd
		return;
	}

	runFunction(connection->socket, "kernel_response_file", 1,
			string_itoa(gfd_delete));
}

void cpu_close_file(socket_connection* connection, char** args) {
	int fd_close = atoi(args[0]);
	int pid = atoi(args[1]);

	bool result = close_file(fd_close, pid);
	if (result) {
		runFunction(connection->socket, "kernel_response_file", 1,
				string_itoa(fd_close));
		return;
	}

	runFunction(connection->socket, "kernel_response_file", 1,
			string_itoa(ERROR_SIN_DEFINIR)); //TODO ARCHIVO_SIN_ABRIR_PREVIAMENTE deberia ser pero no se me actualiza la libreria comun :c

}

void cpu_seek_file(socket_connection* connection, char** args) {
	int fd = atoi(args[0]);
	int pos = atoi(args[1]);
	int pid = atoi(args[2]);

	bool result = set_pointer(pos, fd, pid);
	if (result)
		runFunction(connection->socket, "kernel_response_file", 1, string_itoa(fd));
	else
		runFunction(connection->socket, "kernel_response_file", 1,
				string_itoa(ERROR_SIN_DEFINIR)); //TODO ARCHIVO_SIN_ABRIR_PREVIAMENTE deberia ser pero no se me actualiza la libreria comun :c

}

void cpu_write_file(socket_connection* connection, char** args) {
	int fd = atoi(args[0]);
	char* info = args[1];
	int size = atoi(args[2]);
	int pid = atoi(args[3]);

	bool result = write_file(fd, pid, info, size);

	char* path = get_path_by_fd_and_pid(fd, pid);
	if (path == NULL && !result) {
		runFunction(connection->socket, "kernel_response_file", 1,
				string_itoa(ERROR_SIN_DEFINIR));
		return; //TODO ARCHIVO_SIN_ABRIR_PREVIAMENTE deberia ser pero no se me actualiza la libreria comun :c
	}

	if (!result)
		fd = ESCRIBIR_SIN_PERMISOS;

	runFunction(connection->socket, "kernel_response", 1, string_itoa(fd));

	log_debug(logger, "Resultado Kernel Escribir '%d'", result);
}

void cpu_read_file(socket_connection* connection, char** args) {

	int fd = atoi(args[0]);
	int offset = atoi(args[1]);
	int size = atoi(args[2]);
	int pid = atoi(args[3]);

	t_open_file* process = get_open_file_by_fd_and_pid(fd, pid);
	char* path = get_path_by_gfd(process->gfd);
	char* flags = process->flag;

	if (is_allowed(pid, fd, flags)) {
		runFunction(fs_socket, "kernel_get_data", 3, path, offset, size);
		wait_response(fs_mutex);
		runFunction(connection->socket, "kernel_response_read_file", 2, fs_read_buffer, string_itoa(fd));

	} else
		fd = LEER_SIN_PERMISOS;
	runFunction(connection->socket, "kernel_response_file", 1, string_itoa(fd));
}

/*
 * MEMORY
 */
void memory_identify(socket_connection* connection, char** args) {
	runFunction(connection->socket, "client_identify", 1, KERNEL);
}
void memory_response_start_program(socket_connection* connection, char** args) {
	int response = atoi(args[0]);

	pcb* n_pcb = find_pcb_by_pid(process_struct.pid);
	n_pcb->exit_code = response;

	if (response == NO_ERRORES) {
		move_to_list(n_pcb, READY_LIST);
		add_process_in_memory();
		short_planning();
	} else {
		move_to_list(n_pcb, EXIT_LIST);
	}

	runFunction(process_struct.socket, "kernel_response_load_program", 2,
			string_itoa(response), string_itoa(p_counter));
}
void memory_page_size(socket_connection* connection, char** args) {
	int page_size = atoi(args[0]);

	mem_page_size = page_size;
	runFunction(mem_socket, "kernel_stack_size", 1, string_itoa(stack_size));
}

/*
 * FILESYSTEM
 */
void fs_response_file(socket_connection* connection, char** args) {
	fs_response = atoi(args[0]);
	signal_response(fs_mutex);
}

void fs_response_get_data(socket_connection* connection, char** args) {
	fs_read_buffer = args[0];
	signal_response(fs_mutex);
}

/*
 * SERVER
 */
void server_connectionClosed(socket_connection* connection) {
	char* client = (connection->socket == fs_socket) ? FILESYSTEM : MEMORY;
	log_debug(logger, "%s has disconnected. Socket = %d, IP = %s, Port = %d.\n",
			client, connection->socket, connection->ip, connection->port);
	exit(EXIT_FAILURE);
}

/*
 * CLIENT
 */
void newClient(socket_connection* connection) {
	char* client = (connection->port == port_cpu) ? CPU : CONSOLE;

	log_debug(logger, "%s has connected. Socket = %d, IP = %s, Port = %d.\n",
			client, connection->socket, connection->ip, connection->port);

	if (!strcmp(client, CPU)) {
		t_cpu* cpu = malloc(sizeof(t_cpu));
		cpu->busy = false;
		cpu->socket = connection->socket;
		pthread_mutex_lock(&cpu_mutex);
		list_add(cpu_list, cpu);
		pthread_mutex_unlock(&cpu_mutex);
		runFunction(connection->socket, "kernel_page_stack_size", 2,
				string_itoa(mem_page_size), string_itoa(stack_size));
	}
}
void connectionClosed(socket_connection* connection) {
	char* client = (connection->port == port_cpu) ? CPU : CONSOLE;

	log_debug(logger, "%s has disconnected. Socket = %d, IP = %s, Port = %d.\n",
			client, connection->socket, connection->ip, connection->port);

	if (!strcmp(client, CONSOLE)) {
		pcb* l_pcb = find_pcb_by_socket(connection->socket);
		move_to_list(l_pcb, EXIT_LIST);
		substract_process_in_memory();
		runFunction(mem_socket, "i_finish_program", 1, string_itoa(l_pcb->pid));
	} else if (!strcmp(client, CPU)) {
		t_cpu* cpu = find_cpu_by_socket(connection->socket);
		remove_cpu_from_cpu_list(cpu);
		free(cpu);
	}
}
