#include "kernel_interface.h"

/*
 * CONSOLE
 */
void console_load_program(socket_connection* connection, char** args) {
	log_debug(logger, "console_load_program");
	char* program = args[0];

	if (process_in_memory + 1 > multiprog) {
		runFunction(connection->socket, "kernel_response_load_program", 2, string_itoa(NO_SE_PUEDEN_RESERVAR_RECURSOS), string_itoa(-1));
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

	/*if (metadata->cantidad_de_etiquetas + metadata->cantidad_de_funciones > 0) {
	 char** labels = string_split(metadata->etiquetas, "\0");
	 for (i = 0; i < metadata->cantidad_de_etiquetas + metadata->cantidad_de_funciones; i++) {
	 char* label = labels[i];
	 t_puntero_instruccion* instruction = malloc(sizeof(t_puntero_instruccion));
	 *instruction = metadata_buscar_etiqueta(label, metadata->etiquetas, metadata->etiquetas_size); //aca rompia con Program3, por el cantidad_de_etiquetas
	 dictionary_put(new_pcb->i_label, label, instruction);
	 }
	 }*/

	char* label = string_new();
	for (i = 0; i < metadata->etiquetas_size; i++) {
		if (metadata->etiquetas[i] != '\0') {
			string_append_with_format(&label, "%c", metadata->etiquetas[i]);
		} else {
			t_puntero_instruccion* instruction = malloc(sizeof(t_puntero_instruccion));
			*instruction = metadata_buscar_etiqueta(label, metadata->etiquetas, metadata->etiquetas_size);
			dictionary_put(new_pcb->i_label, label, instruction);
			label = string_new();
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

	t_socket_pcb* socket_pcb = malloc(sizeof(t_socket_pcb));
	socket_pcb->pid = new_pcb->pid;
	socket_pcb->state = new_pcb->state;
	socket_pcb->socket = connection->socket;
	list_add(socket_pcb_list, socket_pcb);

	process_struct.socket = connection->socket;
	process_struct.pid = new_pcb->pid;
	process_struct.state = new_pcb->state;
	pthread_mutex_unlock(&pcb_list_mutex);
	runFunction(mem_socket, "i_start_program", 2, string_itoa(new_pcb->pid), program);
}
void console_abort_program(socket_connection* connection, char** args) {
	int pid = atoi(args[0]);
	pthread_mutex_lock(&abort_console_mutex);
	pcb* l_pcb = find_pcb_by_pid(pid);
	l_pcb->exit_code = FINALIZADO_CONSOLA;

	if (l_pcb->state != EXEC_LIST) {
		remove_from_list_sems(l_pcb->pid);
		substract_process_in_memory();
		runFunction(mem_socket, "i_finish_program", 1, string_itoa(l_pcb->pid));
		move_to_list(l_pcb, EXIT_LIST);
	}
	pthread_mutex_unlock(&abort_console_mutex);
}

/*
 * CPU
 */
void cpu_has_quantum_changed(socket_connection* connection, char** args) {
	send_dynamic_message(connection->socket, string_itoa(quantum_sleep));
	log_debug(logger, "cpu_has_quantum_changed: '%d'", quantum_sleep);
}
void cpu_has_aborted(socket_connection* connection, char** args) {
	int pid = atoi(args[0]);
	pthread_mutex_lock(&abort_console_mutex);
	pcb* l_pcb = find_pcb_by_pid(pid);
	int result = l_pcb->exit_code;

	send_dynamic_message(connection->socket, string_itoa(result));
	pthread_mutex_unlock(&abort_console_mutex);
	if(result < 0)
		log_debug(logger, "cpu_has_aborted: '%d'", result);
}
void cpu_received_page_stack_size(socket_connection* connection, char** args) {
	log_debug(logger, "cpu_received_page_stack_size");
	short_planning();
}
void cpu_get_shared_var(socket_connection* connection, char** args) {
	log_debug(logger, "cpu_get_shared_var");

	char* var_name = string_new();
	string_append(&var_name, args[0]);

	pthread_mutex_lock(&shared_vars_mutex);
	bool find_var_get(void* s) {
		t_shared_var* shared = s;
		return !strcmp(shared->var, var_name);
	}

	log_debug(logger, "cpu_get_shared_var: var_name=%s", var_name);
	t_shared_var* shared = list_find(shared_vars, &find_var_get);
	log_debug(logger, "cpu_get_shared_var: shared is null? %s", shared == NULL ? "true" : "false");
	send_dynamic_message(connection->socket, string_itoa(shared->value));
	pthread_mutex_unlock(&shared_vars_mutex);
	log_debug(logger, "fin cpu_get_shared_var");
	free(var_name);
}
void cpu_set_shared_var(socket_connection* connection, char** args) {
	log_debug(logger, "cpu_set_shared_var");

	char* var_name = string_new();
	string_append(&var_name, args[0]);

	int var_value = atoi(args[1]);
	pthread_mutex_lock(&shared_vars_mutex);
	bool find_var_set(void* s) {
		t_shared_var* shared = s;
		return !strcmp(shared->var, var_name);
	}

	log_debug(logger, "cpu_set_shared_var: var_name=%s, var_value=%d", var_name, var_value);
	t_shared_var* shared = list_find(shared_vars, &find_var_set);
	log_debug(logger, "cpu_set_shared_var: shared is null? %s", shared == NULL ? "true" : "false");
	shared->value = var_value;
	log_debug(logger, "cpu_set_shared_var: var_value=%d", var_value);
	send_dynamic_message(connection->socket, string_itoa(NO_ERRORES));
	pthread_mutex_unlock(&shared_vars_mutex);
	log_debug(logger, "fin cpu_set_shared_var");
	free(var_name);
}
void set_new_pcb(pcb** o_pcb, pcb* n_pcb) {
	*o_pcb = n_pcb;
}
void cpu_task_finished(socket_connection* connection, char** args) {
	log_debug(logger, "cpu_task_finished");
	//printf("");
	pcb* n_pcb = string_to_pcb(args[0]);
	bool finished = atoi(args[1]);
	bool is_locked = atoi(args[2]);
	bool is_abrupted = atoi(args[3]);

	pcb* o_pcb = find_pcb_by_pid(n_pcb->pid);
	if(o_pcb->exit_code == FINALIZADO_CONSOLA || o_pcb->exit_code == FINALIZADO_KERNEL)
		n_pcb->exit_code = o_pcb->exit_code;
	set_new_pcb(&o_pcb, n_pcb);

	t_cpu* n_cpu = find_cpu_by_socket(connection->socket);
	if (!is_abrupted){
		n_cpu->busy = false;
	}

	void free_heap(void* element) {
		t_heap_manage* heap = element;
		free(heap);
	}

	if (finished) {
		if (n_pcb->exit_code == FINALIZADO_CONSOLA || n_pcb->exit_code == FINALIZADO_KERNEL) {
			int pos = find_heap_pages_pos_in_list(process_heap_pages, n_pcb->pid);
			list_remove_and_destroy_element(process_heap_pages, pos, &free_heap);
			move_to_list(o_pcb, EXIT_LIST);
			substract_process_in_memory();
			runFunction(mem_socket, "i_finish_program", 1, string_itoa(n_pcb->pid));
		} else if (is_locked) {
			move_to_list(o_pcb, BLOCK_LIST);
		} else {
			int pos = find_heap_pages_pos_in_list(process_heap_pages, n_pcb->pid);
			list_remove_and_destroy_element(process_heap_pages, pos, &free_heap);
			move_to_list(n_pcb, EXIT_LIST);
			substract_process_in_memory();
			runFunction(mem_socket, "i_finish_program", 1, string_itoa(n_pcb->pid));
			if (n_pcb->exit_code != FINALIZADO_CONSOLA && n_pcb->exit_code != FINALIZADO_KERNEL) {
				t_socket_pcb* socket_pcb = find_socket_by_pid(n_pcb->pid);
				runFunction(socket_pcb->socket, "kernel_stop_process", 2, string_itoa(n_pcb->pid), string_itoa(n_pcb->exit_code));
			}
		}
	} else {
		if (n_pcb->exit_code != FINALIZADO_CONSOLA && n_pcb->exit_code != FINALIZADO_KERNEL) {
			move_to_list(o_pcb, READY_LIST);
		} else {
			int pos = find_heap_pages_pos_in_list(process_heap_pages, n_pcb->pid);
			list_remove_and_destroy_element(process_heap_pages, pos, &free_heap);
			move_to_list(o_pcb, EXIT_LIST);
			substract_process_in_memory();
			runFunction(mem_socket, "i_finish_program", 1, string_itoa(n_pcb->pid));
		}
	}

	if(is_locked)
		pthread_mutex_unlock(&sems_mutex);

	short_planning();
}

char* add_blocked_process(char* blocked_pids, int process_pid) {
	pthread_mutex_lock(&sems_blocked_list);

	log_debug(logger, "add_blocked_process");
	char** arr = string_get_string_as_array(blocked_pids);
	char* res = string_new();
	int i = 0;
	while (arr[i] != NULL)
		string_append_with_format(&res, "%s,", arr[i++]);
	string_append_with_format(&res, "%d]", process_pid);

	pthread_mutex_unlock(&sems_blocked_list);
	return string_from_format("[%s", res);
}

void add_sem_pid_list(char* id_sem, int pid) {
	bool find_sem_pid(void* e) {
		t_sem_pid* s = e;
		return s->pid == pid && !strcmp(s->sem, id_sem);
	}

	t_sem_pid* f = list_find(sem_pid_list, &find_sem_pid);

	if(f == NULL) {
		t_sem_pid* sem_pid = malloc(sizeof(t_sem_pid));
		sem_pid->pid = pid;
		sem_pid->sem = string_from_format("%s", id_sem);
		list_add(sem_pid_list, sem_pid);
	}
}

void cpu_wait_sem(socket_connection* connection, char** args) {
	log_debug(logger, "cpu_wait_sem");
	pthread_mutex_lock(&sems_mutex);
	void free_sem(void* v) {
		sem_status* sem = v;
		free(sem);
	}

	char* id_sem = string_new();
	string_append(&id_sem, args[0]);

	string_trim(&id_sem);
	t_cpu* _cpu = find_cpu_by_socket(connection->socket);
	log_debug(logger, "cpu_wait_sem: _cpu is null? %s", _cpu == NULL ? "true" : "false");
	int process_pid = _cpu->xpid;

	bool find_sem(void* s) {
		t_sem* sem = s;
		return !strcmp(sem->id, id_sem);
	}

	t_sem* sem_curr = list_find(sem_ids, &find_sem);
	log_debug(logger, "cpu_wait_sem: sem_curr is null? %s", sem_curr == NULL ? "true" : "false");


	bool is_locked = false;
	if (sem_curr->value <= 0) {
		pcb* p = find_pcb_by_pid(process_pid);
		if(p->state != EXIT_LIST) {
			char* temp = add_blocked_process(sem_curr->blocked_pids, process_pid);
			sem_curr->blocked_pids = string_new();
			string_append(&sem_curr->blocked_pids, temp);
		}
		is_locked = true;
		send_dynamic_message(connection->socket, string_itoa(is_locked));
		sem_curr->value--;
	} else {
		add_sem_pid_list(id_sem, process_pid);
		send_dynamic_message(connection->socket, string_itoa(is_locked));
		sem_curr->value--;
		pthread_mutex_unlock(&sems_mutex);
	}

	log_debug(logger, "cpu_wait_sem: exit");
	free(id_sem);
}

int get_first(char* blocked_pids) {
	log_debug(logger, "get_last");
	char** arr = string_get_string_as_array(blocked_pids);

	if (arr[0] != NULL)
		return atoi(arr[0]);
	return -1;
}

char* remove_first(char* blocked_pids) {
	log_debug(logger, "remove_last");
	pthread_mutex_lock(&sems_blocked_list);
	char** arr = string_get_string_as_array(blocked_pids);
	char* res = string_new();

	int i = 1;
	while (arr[i] != NULL) {
		string_append_with_format(&res, "%s,", arr[i]);
		i++;
	}

	if (string_length(res) > 0)
		res = string_substring_until(res, string_length(res) - 1);
	pthread_mutex_unlock(&sems_blocked_list);
	return string_from_format("[%s]", res);
}

void cpu_signal_sem(socket_connection* connection, char** args) {
	log_debug(logger, "cpu_signal_sem");
	pthread_mutex_lock(&sems_mutex);
	void free_sem(void* v) {
		sem_status* sem = v;
		free(sem);
	}

	char* id_sem = string_new();
	string_append(&id_sem, args[0]);

	string_trim(&id_sem);

	bool find_sem(void* s) {
		t_sem* sem = s;
		return !strcmp(sem->id, id_sem);
	}

	t_sem* sem_curr = list_find(sem_ids, &find_sem);
	log_debug(logger, "cpu_signal_sem: sem_curr is null? %s", sem_curr == NULL ? "true" : "false");

	sem_curr->value++;

	int process = get_first(sem_curr->blocked_pids);
	if(process > -1) {
		pcb* _pcb = find_pcb_by_pid(process);
		if(_pcb->state == BLOCK_LIST) {
			char* temp = remove_first(sem_curr->blocked_pids);
			sem_curr->blocked_pids = string_new();
			string_append(&sem_curr->blocked_pids, temp);
			if (process >= 0) {
				log_debug(logger, "cpu_signal_sem: _pcb is null? %s", _pcb == NULL ? "true" : "false");
				move_to_list(_pcb, READY_LIST);
				short_planning();
			}
		}
	}

	send_dynamic_message(connection->socket, string_itoa(NO_ERRORES));
	pthread_mutex_unlock(&sems_mutex);
	free(id_sem);
}

pthread_mutex_t malloc_mutex = PTHREAD_MUTEX_INITIALIZER;
void cpu_malloc(socket_connection* connection, char** args) {
	pthread_mutex_lock(&malloc_mutex);
	log_debug(logger, "cpu_malloc");
	int space = atoi(args[0]);
	int pid = atoi(args[1]);

	log_debug(logger, "cpu_malloc");
	int pos = malloc_memory(pid, space);
	log_debug(logger, "cpu_malloc: pos '%d'", pos);
	//runFunction(connection->socket, "kernel_response_malloc_pointer", 1, string_itoa(pos));
	send_dynamic_message(connection->socket, string_itoa(pos));
	log_debug(logger, "cpu_malloc: send");
	pthread_mutex_unlock(&malloc_mutex);
}

void cpu_free(socket_connection* connection, char** args) {
	pthread_mutex_lock(&malloc_mutex);
	log_debug(logger, "cpu_free");
	int pointer = atoi(args[0]);
	int pid = atoi(args[1]);
	free_memory(pid, pointer);
	//runFunction(connection->socket, "kernel_response", 0);
	send_dynamic_message(connection->socket, string_itoa(NO_ERRORES));
	pthread_mutex_unlock(&malloc_mutex);
}

void cpu_validate_file(socket_connection* connection, char** args) {
	log_debug(logger, "cpu_validate_file");
	char* path = args[0];
	int pid = atoi(args[1]);
	bool validate = validate_file_from_fs(path, pid);

	//runFunction(connection->socket, "kernel_response_validate_file", 1, string_itoa(validate));
	send_dynamic_message(connection->socket, string_itoa(validate));
}

void cpu_open_file(socket_connection* connection, char** args) {
	log_debug(logger, "cpu_open_file");
	char* path = args[0];
	char* flags = args[1];
	bool validate = atoi(args[2]);
	int pid = atoi(args[3]);

	if (!validate && string_contains(flags, "c")) {
		runFunction(fs_socket, "kernel_create_file", 2, path, string_itoa(pid));
		wait_response(&fs_mutex);
		if (fs_response == 0) {
			send_dynamic_message(connection->socket, string_itoa(ERROR_CREAR_ARCHIVO));
			log_debug(logger, "Error de cpu_open_file que nunca deberia llegar");
			return;
		}
	}

	int fd_assigned = open_file_in_process_table(path, flags, pid);
	send_dynamic_message(connection->socket, string_itoa(fd_assigned));
	//runFunction(connection->socket, "kernel_response_file", 1, string_itoa(fd_assigned));
}

void cpu_delete_file(socket_connection* connection, char** args) {
	log_debug(logger, "cpu_delete_file");
	int fd_delete = atoi(args[0]);
	int pid = atoi(args[1]);

	t_open_file* open_file = get_open_file_by_fd_and_pid(fd_delete, pid);
	char* path = get_path_by_gfd(open_file->gfd);

	int result = delete_file_from_global_table(open_file->gfd);
	if (result < 0) {
		send_dynamic_message(connection->socket, string_itoa(result));
		return;
	}

	runFunction(fs_socket, "kernel_delete_file", 2, path, string_itoa(pid));
	wait_response(&fs_mutex);
	if (fs_response == 0) {
		//TODO aca llega cuando delete_file = false, que no se que significa del lado de FileSystem xd
		log_debug(logger, "Error de cpu_delete_file que nunca deberia llegar");
		send_dynamic_message(connection->socket, string_itoa(ERROR_SIN_DEFINIR));
		return;
	}

	//runFunction(connection->socket, "kernel_response_file", 1, string_itoa(gfd_delete));
	send_dynamic_message(connection->socket, string_itoa(fd_delete));
}

void cpu_close_file(socket_connection* connection, char** args) {
	log_debug(logger, "cpu_close_file");
	int fd_close = atoi(args[0]);
	int pid = atoi(args[1]);

	bool result = close_file(fd_close, pid);
	if (result) {
		//runFunction(connection->socket, "kernel_response_file", 1, string_itoa(fd_close));
		send_dynamic_message(connection->socket, string_itoa(fd_close));
		return;
	}

	send_dynamic_message(connection->socket, string_itoa(ARCHIVO_SIN_ABRIR_PREVIAMENTE));
	//runFunction(connection->socket, "kernel_response_file", 1, string_itoa(ARCHIVO_SIN_ABRIR_PREVIAMENTE));

}

void cpu_seek_file(socket_connection* connection, char** args) {
	log_debug(logger, "cpu_seek_file");
	int fd = atoi(args[0]);
	int pos = atoi(args[1]);
	int pid = atoi(args[2]);

	bool result = set_pointer(pos, fd, pid);
	if (result) {
		send_dynamic_message(connection->socket, string_itoa(fd));
		//runFunction(connection->socket, "kernel_response_file", 1, string_itoa(fd));
	} else {
		send_dynamic_message(connection->socket, string_itoa(ARCHIVO_SIN_ABRIR_PREVIAMENTE));
		//runFunction(connection->socket, "kernel_response_file", 1, string_itoa(ARCHIVO_SIN_ABRIR_PREVIAMENTE));
	}
}

void cpu_write_file(socket_connection* connection, char** args) {
	log_debug(logger, "cpu_write_file");
	int fd = atoi(args[0]);
	char* info = args[1];
	int size = atoi(args[2]);
	int pid = atoi(args[3]);

	int result = write_file(fd, pid, info, size);

	//char* path = string_from_format("%s", get_path_by_fd_and_pid(fd, pid));
	if (result < 0) {
		//runFunction(connection->socket, "kernel_response_file", 1, string_itoa(ARCHIVO_SIN_ABRIR_PREVIAMENTE));
		send_dynamic_message(connection->socket, string_itoa(result));
		return;
	}
	//runFunction(connection->socket, "kernel_response", 1, string_itoa(fd));
	send_dynamic_message(connection->socket, string_itoa(fd));

	log_debug(logger, "runFunction a socket: '%d', Resultado Kernel Escribir '%d'", connection->socket, result);
}

void cpu_read_file(socket_connection* connection, char** args) {
	log_debug(logger, "cpu_read_file");
	int fd = atoi(args[0]);
	int direccion_variable = atoi(args[1]);
	int size = atoi(args[2]);
	int pid = atoi(args[3]);

	t_open_file* process = get_open_file_by_fd_and_pid(fd, pid);
	char* path = get_path_by_gfd(process->gfd);
	char* flags = process->flag;
	int offset = process->pointer;

	if (is_allowed(pid, fd, flags)) {
		runFunction(fs_socket, "kernel_get_data", 4, path, string_itoa(offset), string_itoa(size), string_itoa(pid));
		wait_response(&fs_mutex);
		runFunction(mem_socket, "i_get_page_from_pointer", 1, string_itoa(direccion_variable));
		wait_response(&mem_response);

		int n_frame = direccion_variable / mem_page_size;
		int n_page = page_from_pointer;
		int n_offset = direccion_variable - n_frame * mem_page_size;
		int n_size = string_length(fs_read_buffer);

		if (string_length(fs_read_buffer) > 0) {
			runFunction(mem_socket, "i_store_bytes_in_page", 5, string_itoa(pid), string_itoa(n_page), string_itoa(n_offset), string_itoa(n_size), fs_read_buffer);
			wait_response(&mem_response);
		} else
			fd = ERROR_LEER_ARCHIVO;
	} else
		fd = LEER_SIN_PERMISOS;

	//runFunction(connection->socket, "kernel_response_file", 1, string_itoa(fd));
	send_dynamic_message(connection->socket, string_itoa(fd));

	log_debug(logger, "cpu_read_file: fd: '%d', fs_read_buffer: '%s", fd, fs_read_buffer);
}

/*
 * MEMORY
 */
void memory_identify(socket_connection* connection, char** args) {
	runFunction(connection->socket, "client_identify", 1, KERNEL);
}
void memory_response_start_program(socket_connection* connection, char** args) {
	log_debug(logger, "memory_response_start_program");
	int response = atoi(args[0]);

	pcb* n_pcb = find_pcb_by_pid(process_struct.pid);
	n_pcb->exit_code = response;

	if (response == NO_ERRORES) {
		t_heap_manage* heap_manage = malloc(sizeof(t_heap_manage));
		t_heap_stats* heap_stats = malloc(sizeof(t_heap_stats));

		heap_manage->heap_pages = list_create();
		heap_manage->pid = n_pcb->pid;

		heap_stats->pid = n_pcb->pid;
		heap_stats->malloc_c = 0;
		heap_stats->malloc_b = 0;
		heap_stats->free_c = 0;
		heap_stats->free_b = 0;

		list_add(process_heap_pages, heap_manage);
		list_add(heap_stats_list, heap_stats);
		move_to_list(n_pcb, READY_LIST);
		add_process_in_memory();
		short_planning();
	} else {
		move_to_list(n_pcb, EXIT_LIST);
	}

	runFunction(process_struct.socket, "kernel_response_load_program", 2, string_itoa(response), string_itoa(p_counter));
	pthread_mutex_unlock(&console_mutex);
}
void memory_page_size(socket_connection* connection, char** args) {
	log_debug(logger, "memory_response_start_program");
	int page_size = atoi(args[0]);

	mem_page_size = page_size;
	runFunction(mem_socket, "kernel_stack_size", 1, string_itoa(stack_size));
}

void memory_response_heap(socket_connection* connection, char** args) {
	log_debug(logger, "memory_response_heap");
	memory_response = atoi(args[0]);
	signal_response(&mem_response);
}

void memory_response_store_bytes_in_page(socket_connection* connection, char** args) {
	memory_response = atoi(args[0]);
	mem_offset_abs = atoi(args[1]);
	log_debug(logger, "memory_response_store_bytes_in_page: mem_offset_abs=%d", mem_offset_abs);
	signal_response(&mem_response);
}

void memory_response_read_bytes_from_page(socket_connection* connection, char** args) {
	log_debug(logger, "memory_response_read_bytes_from_page: buffer=%s", args[0]);

	mem_read_buffer = string_new();
	string_append(&mem_read_buffer, args[0]);
	signal_response(&mem_response);
}

void memory_response_get_page_from_pointer(socket_connection* connection, char** args) {
	log_debug(logger, "memory_response_get_page_from_pointer: page_from_pointer=%s", args[0]);

	page_from_pointer = atoi(args[0]);
	signal_response(&mem_response);
}

/*
 * FILESYSTEM
 */
void fs_response_file(socket_connection* connection, char** args) {
	log_debug(logger, "fs_response_file");

	fs_response = atoi(args[0]);
	signal_response(&fs_mutex);
}

void fs_response_get_data(socket_connection* connection, char** args) {
	log_debug(logger, "fs_response_get_data");

	fs_read_buffer = string_new();
	string_append(&fs_read_buffer, args[0]);
	signal_response(&fs_mutex);
}

/*
 * SERVER
 */
void server_connectionClosed(socket_connection* connection) {
	char* client = (connection->socket == fs_socket) ? FILESYSTEM : MEMORY;
	log_debug(logger, "%s has disconnected. Socket = %d, IP = %s, Port = %d.\n", client, connection->socket, connection->ip, connection->port);
	exit(EXIT_FAILURE);
}

/*
 * CLIENT
 */
void newClient(socket_connection* connection) {
	char* client = (connection->port == port_cpu) ? CPU : CONSOLE;

	log_debug(logger, "%s has connected. Socket = %d, IP = %s, Port = %d.\n", client, connection->socket, connection->ip, connection->port);

	if (!strcmp(client, CPU)) {
		t_cpu* cpu = malloc(sizeof(t_cpu));
		cpu->busy = false;
		cpu->socket = connection->socket;
		pthread_mutex_lock(&cpu_mutex);
		list_add(cpu_list, cpu);
		pthread_mutex_unlock(&cpu_mutex);
		runFunction(connection->socket, "kernel_page_stack_size", 2, string_itoa(mem_page_size), string_itoa(stack_size));
	}
}
void connectionClosed(socket_connection* connection) {
	char* client = (connection->port == port_cpu) ? CPU : CONSOLE;

	log_debug(logger, "%s has disconnected. Socket = %d, IP = %s, Port = %d.\n", client, connection->socket, connection->ip, connection->port);

	if (!strcmp(client, CONSOLE)) {
		pcb* l_pcb = find_pcb_by_socket(connection->socket);
		if (l_pcb != NULL && l_pcb->state != EXIT_LIST) {
			move_to_list(l_pcb, EXIT_LIST);
			substract_process_in_memory();
			runFunction(mem_socket, "i_finish_program", 1, string_itoa(l_pcb->pid));
		}
	} else if (!strcmp(client, CPU)) {
		t_cpu* cpu = find_cpu_by_socket(connection->socket);
		remove_cpu_from_cpu_list(cpu);
		free(cpu);
	}
}
