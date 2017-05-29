#include "kernel.h"

const char* CONFIG_FIELDS[] = { PUERTO_PROG, PUERTO_CPU,
IP_MEMORIA, IP_FILESYSTEM, PUERTO_MEMORIA, IP_FS, PUERTO_FS, QUANTUM,
QUANTUM_SLEEP, ALGORITMO, GRADO_MULTIPROG, SEM_IDS, SEM_INIT,
SHARED_VARS, STACK_SIZE };

bool is_cpu_free() {
	pthread_mutex_lock(&cpu_mutex);
	bool find(void* e) {
		t_cpu* cpu = e;
		return !cpu->busy;
	}

	bool result = list_any_satisfy(cpu_list, find);
	pthread_mutex_unlock(&cpu_mutex);
	return result;
}

t_cpu* get_cpu_free() {
	pthread_mutex_lock(&cpu_mutex);
	bool find(void* e) {
		t_cpu* cpu = e;
		return !cpu->busy;
	}

	t_cpu* result = list_find(cpu_list, find);
	pthread_mutex_unlock(&cpu_mutex);
	return result;
}

void short_planning() {
	pthread_mutex_lock(&planning_mutex);
	if (planning_running && is_cpu_free() && list_size(ready_list) > 0) {
		t_cpu* free_cpu = get_cpu_free();
		pcb* _pcb = list_get(ready_list, 0);

		move_to_list(_pcb, EXEC_LIST);
		free_cpu->busy = true;

		char* pcb_string = pcb_to_string(_pcb);

		runFunction(free_cpu->socket, "kernel_receive_pcb", 3, string_itoa(planning_alg), string_itoa(quantum), pcb_string);
	}
	pthread_mutex_unlock(&planning_mutex);
}

void remove_cpu_from_cpu_list(t_cpu* cpu) {
	pthread_mutex_lock(&cpu_mutex);
	int i;
	for (i = 0; i < list_size(cpu_list); i++) {
		t_cpu* n_cpu = list_get(cpu_list, i);
		if (n_cpu->socket == cpu->socket) {
			list_remove(cpu_list, i);
			break;
		}
	}
	pthread_mutex_unlock(&cpu_mutex);
}

t_cpu* find_cpu_by_socket(int socket) {
	pthread_mutex_lock(&cpu_mutex);
	bool find(void* element) {
		t_cpu* cpu = element;
		return cpu->socket == socket;
	}

	t_cpu* cpu = list_find(cpu_list, find);
	pthread_mutex_unlock(&cpu_mutex);

	return cpu;
}

void remove_pcb_from_socket_pcb_list(pcb* n_pcb) {
	pthread_mutex_lock(&socket_pcb_mutex);
	int i;
	for (i = 0; i < list_size(socket_pcb_list); i++) {
		t_socket_pcb* socket_pcb = list_get(socket_pcb_list, i);
		if (socket_pcb->pcb->pid == n_pcb->pid) {
			list_remove(socket_pcb_list, i);
			break;
		}
	}
	pthread_mutex_unlock(&socket_pcb_mutex);
}

pcb* find_pcb_by_pid(int pid) {
	pthread_mutex_lock(&socket_pcb_mutex);
	bool find(void* element) {
		t_socket_pcb* n_pcb = element;
		return n_pcb->pcb->pid == pid;
	}

	t_socket_pcb* n_pcb = list_find(socket_pcb_list, &find);
	pthread_mutex_unlock(&socket_pcb_mutex);

	return n_pcb->pcb;
}

pcb* find_pcb_by_socket(int socket) {
	pthread_mutex_lock(&socket_pcb_mutex);
	bool find(void* element) {
		t_socket_pcb* n_pcb = element;
		return n_pcb->socket == socket;
	}

	t_socket_pcb* n_pcb = list_find(socket_pcb_list, &find);
	pthread_mutex_unlock(&socket_pcb_mutex);

	return n_pcb->pcb;
}

int find_pcb_pos_in_list(t_list* list, pcb* l_pcb) {
	int i;
	for (i = 0; i < list_size(list); i++) {
		pcb* f_pcb = list_get(list, i);
		if (f_pcb->pid == l_pcb->pid)
			return i;
	}

	return -1;
}

void substract_process_in_memory() {
	pthread_mutex_lock(&process_in_memory_mutex);

	process_in_memory--;
	if (process_in_memory < multiprog)
		pthread_mutex_unlock(&console_mutex);

	pthread_mutex_unlock(&process_in_memory_mutex);
}

void add_process_in_memory() {
	pthread_mutex_lock(&process_in_memory_mutex);

	process_in_memory++;
	if (process_in_memory < multiprog)
		pthread_mutex_unlock(&console_mutex);

	pthread_mutex_unlock(&process_in_memory_mutex);
}

void move_to_list(pcb* pcb, int list_name) {
	int pos;

	pthread_mutex_lock(&pcb_list_mutex);
	switch (pcb->state) {
		case NEW_LIST:
			queue_pop(new_queue);
			break;
		case READY_LIST:
			pos = find_pcb_pos_in_list(ready_list, pcb);
			list_remove(ready_list, pos);
			break;
		case EXEC_LIST:
			pos = find_pcb_pos_in_list(exec_list, pcb);
			list_remove(exec_list, pos);
			break;
		case BLOCK_LIST:
			pos = find_pcb_pos_in_list(block_list, pcb);
			list_remove(block_list, pos);
			break;
		case EXIT_LIST:
			queue_pop(exit_queue);
			break;
	}

	pcb->state = list_name;
	switch (list_name) {
		case NEW_LIST:
			queue_push(new_queue, pcb);
			break;
		case READY_LIST:
			list_add(ready_list, pcb);
			break;
		case EXEC_LIST:
			list_add(exec_list, pcb);
			break;
		case BLOCK_LIST:
			list_add(block_list, pcb);
			break;
		case EXIT_LIST:
			queue_push(exit_queue, pcb);
			break;
	}
	pthread_mutex_unlock(&pcb_list_mutex);
}

void create_function_dictionary() {
	fns = dictionary_create();

	dictionary_put(fns, "console_load_program", &console_load_program);
	dictionary_put(fns, "memory_identify", &memory_identify);
	dictionary_put(fns, "memory_response_start_program", &memory_response_start_program);
	dictionary_put(fns, "memory_page_size", &memory_page_size);
	dictionary_put(fns, "cpu_received_page_stack_size", &cpu_received_page_stack_size);
	dictionary_put(fns, "cpu_get_shared_var", &cpu_get_shared_var);
	dictionary_put(fns, "cpu_task_finished", &cpu_task_finished);
}

void open_socket(t_config* config, char* name) {
	int port;

	if (!strcmp(name, CONSOLE))
		port = config_get_int_value(config, PUERTO_PROG);
	else if (!strcmp(name, CPU))
		port = config_get_int_value(config, PUERTO_CPU);
	else {
		log_error(logger, "Error at creating listener for %s", name);
		error_show(" at creating listener for %s", name);
		exit(EXIT_FAILURE);
	}

	if (createListen(port, &newClient, fns, &connectionClosed, NULL) == -1) {
		log_error(logger, "Error at creating listener for %s", name);
		error_show(" at creating listener for %s", name);
		exit(EXIT_FAILURE);
	}
	log_debug(logger, "Listening new clients of %s at %d.\n", name, port);
}

void connect_to_server(t_config* config, char* name) {
	int port;
	char* ip_server;
	int* socket;

	if (!strcmp(name, FILESYSTEM)) {
		ip_server = config_get_string_value(config, IP_FILESYSTEM);
		port = config_get_int_value(config, PUERTO_FS);
		socket = &fs_socket;
	} else if (!strcmp(name, MEMORY)) {
		ip_server = config_get_string_value(config, IP_MEMORIA);
		port = config_get_int_value(config, PUERTO_MEMORIA);
		socket = &mem_socket;
	} else {
		log_error(logger, "Error at connecting to %s.", name);
		error_show(" at connecting to %s.\n", name);
		exit(EXIT_FAILURE);
	}

	if ((*socket = connectServer(ip_server, port, fns, &server_connectionClosed, NULL)) == -1) {
		log_error(logger, "Error at connecting to %s.", name);
		error_show(" at connecting to %s.\n", name);
		exit(EXIT_FAILURE);
	}
	log_debug(logger, "Connected to %s.", name);
}

void init_kernel(t_config* config) {
	log_debug(logger, "Initiating KERNEL");

	pthread_mutex_init(&cpu_mutex, NULL);
	pthread_mutex_init(&p_counter_mutex, NULL);
	pthread_mutex_init(&socket_pcb_mutex, NULL);
	pthread_mutex_init(&console_mutex, NULL);
	pthread_mutex_init(&pcb_list_mutex, NULL);
	pthread_mutex_init(&planning_mutex, NULL);
	pthread_mutex_init(&process_in_memory_mutex, NULL);
	pthread_mutex_init(&shared_vars_mutex, NULL);

	port_con = config_get_int_value(config, PUERTO_PROG);
	port_cpu = config_get_int_value(config, PUERTO_CPU);
	multiprog = config_get_int_value(config, GRADO_MULTIPROG);
	stack_size = config_get_int_value(config, STACK_SIZE);

	shared_vars = dictionary_create();
	char** global_vars_arr = config_get_array_value(config, SHARED_VARS);
	int i = 0;
	while (global_vars_arr[i] != NULL) {
		dictionary_put(shared_vars, string_substring_from(global_vars_arr[i], 1), 0);
		i++;
	}

	sem_ids = dictionary_create();
	char** sem_ids_arr = config_get_array_value(config, SEM_IDS);
	char** sem_vals_arr = config_get_array_value(config, SEM_INIT);
	i = 0;
	while (sem_ids_arr[i] != NULL) {
		int* val = malloc(sizeof(int));
		*val = atoi(sem_vals_arr[i]);
		dictionary_put(sem_ids, sem_ids_arr[i], val);
		i++;
	}

	char* p = config_get_string_value(config, ALGORITMO);
	if (!strcmp(p, "FIFO")) {
		planning_alg = FIFO;
		quantum = 0;
	} else if (!strcmp(p, "RR")) {
		planning_alg = RR;
		quantum = config_get_int_value(config, QUANTUM) - 1;
	}

	p_counter = 0;
	process_in_memory = 0;
	planning_running = true;

	new_queue = queue_create();
	ready_list = list_create();
	exec_list = list_create();
	block_list = list_create();
	exit_queue = queue_create();

	socket_pcb_list = list_create();

	cpu_list = list_create();

	open_socket(config, CONSOLE);
	open_socket(config, CPU);
	connect_to_server(config, FILESYSTEM);
	connect_to_server(config, MEMORY);
}

void print_menu() {
	clear_screen();

	show_title("KERNEL - MAIN MENU");
	println("Enter your choice:");
	println("> ACTIVE_PROCESS");
	println("> INFO");
	println("> FILE_TABLE");
	println("> MULTIPROGRAMMING");
	println("> STOP_PROCESS");
	if (planning_running)
		println("> STOP_PLANIFICATION\n");
	else
		println("> CONTINUE_PLANIFICATION\n");
}

void ask_option(char *sel) {
	print_menu();
	fgets(sel, 255, stdin);
	strtok(sel, "\n");
	string_to_upper(sel);
}

void show_active_process(char option) {
	pthread_mutex_lock(&pcb_list_mutex);
	t_list* list = list_create();
	char* list_name = string_new();
	int i, j;
	switch (option) {
		case 'A':
			string_append(&list_name, "PROCESS IN ALL\n");

			j = queue_size(new_queue);
			for (i = 0; i < j; i++) {
				pcb* n_pcb = queue_pop(new_queue);
				list_add(list, n_pcb);
			}

			list_add_all(list, ready_list);
			list_add_all(list, block_list);
			list_add_all(list, exec_list);

			j = queue_size(exit_queue);
			for (i = 0; i < j; i++) {
				pcb* n_pcb = queue_pop(exit_queue);
				list_add(list, n_pcb);
			}
			break;
		case 'N':
			string_append(&list_name, "PROCESS IN NEW\n");
			int i, j = queue_size(new_queue);
			for (i = 0; i < j; i++) {
				pcb* n_pcb = queue_pop(new_queue);
				list_add(list, n_pcb);
			}
			break;
		case 'R':
			string_append(&list_name, "PROCESS IN READY\n");
			list_add_all(list, ready_list);
			break;
		case 'E':
			string_append(&list_name, "PROCESS IN EXIT\n");
			j = queue_size(exit_queue);
			for (i = 0; i < j; i++) {
				pcb* n_pcb = queue_pop(exit_queue);
				list_add(list, n_pcb);
			}
			break;
		case 'X':
			string_append(&list_name, "PROCESS IN EXEC\n");
			list_add_all(list, exec_list);
			break;
		case 'B':
			string_append(&list_name, "PROCESS IN BLOCK\n");
			list_add_all(list, block_list);
			break;
	}

	clear_screen();
	for (i = 0; i < list_size(list); i++) {
		pcb* n_pcb = list_get(list, i);
		printf("%d ", n_pcb->pid);
	}

	pthread_mutex_unlock(&pcb_list_mutex);
	wait_any_key();
}

void do_show_active_process(char* sel) {
	if (!strcmp(sel, "A")) {
		char queue[255];
		println("> [A] ALL");
		println("> [N] NEW");
		println("> [R] READY");
		println("> [E] EXIT");
		println("> [X] EXEC");
		println("> [B] BLOCK");
		printf("> QUEUE: ");
		fgets(queue, sizeof(queue), stdin);
		strtok(queue, "\n");
		string_to_upper(queue);

		show_active_process(queue[0]);
	}
}

void show_info(int pid) {
	pcb* n_pcb = find_pcb_by_pid(pid);
	char* info = string_new();
	string_append_with_format(&info, "PID: %d\n", n_pcb->pid);
	string_append_with_format(&info, "RAFAGAS EJECUTADAS: %d\n", n_pcb->statistics.cycles);
	string_append_with_format(&info, "OP. PRIVILEGIADAS: %d\n", n_pcb->statistics.op_priviliges);
	// TODO tabla de archivos abiertos
	// Cantidad de páginas de Heap
	// Cantidad de acciones alocar en operaciones y bytes
	// Cantidad de acciones liberar en operaciones y bytes
	// Cantidad de syscalls

	clear_screen();
	printf("%s\n", info);
	wait_any_key();
}

void do_show_info(char* sel) {
	if (!strcmp(sel, "I")) {
		char pid[255];
		printf("> PID: ");
		fgets(pid, sizeof(pid), stdin);
		strtok(pid, "\n");

		show_info(atoi(pid));
	}
}

void do_show_file_table(char* sel) {
	if (!strcmp(sel, "F")) {
		// TODO
	}
}

void do_change_multiprogramming(char* sel) {
	if (!strcmp(sel, "M")) {
		char multi[255];
		printf("> Num Multiprog [%d]: ", multiprog);
		fgets(multi, sizeof(multi), stdin);
		strtok(multi, "\n");

		multiprog = atoi(multi);
	}
}

t_socket_pcb* find_socket_by_pid(int pid) {
	pthread_mutex_lock(&socket_pcb_mutex);
	bool find(void* element) {
		t_socket_pcb* n_socket_pcb = element;
		return n_socket_pcb->pcb->pid == pid;
	}

	t_socket_pcb* n_socket_pcb = list_find(socket_pcb_list, find);
	pthread_mutex_unlock(&socket_pcb_mutex);
	return n_socket_pcb;
}

void stop_process(int pid) {
	t_socket_pcb* n_socket_pcb = find_socket_by_pid(pid);
	n_socket_pcb->pcb->exit_code = ERROR_SIN_DEFINIR;
	runFunction(n_socket_pcb->socket, "kernel_stop_process", 2, n_socket_pcb->pcb->pid, n_socket_pcb->pcb->exit_code);
}

void do_stop_process(char* sel) {
	if (!strcmp(sel, "S")) {
		char pid[255];
		printf("> PID: ");
		fgets(pid, sizeof(pid), stdin);
		strtok(pid, "\n");

		stop_process(atoi(pid));
	}
}

void do_stop_planification(char* sel) {
	if (!strcmp(sel, "P")) {
		planning_running = !planning_running;
		if (planning_running)
			short_planning();
	}
}

int main(int argc, char *argv[]) {
	clear_screen();
	char sel[255];
	t_config *config = malloc(sizeof(t_config));

	remove("log");
	logger = log_create("log", "KERNEL", false, LOG_LEVEL_DEBUG);
	create_function_dictionary();

	load_config(&config, argc, argv[1]);
	print_config(config, CONFIG_FIELDS, CONFIG_FIELDS_N);

	init_kernel(config);

	wait_any_key();

	ask_option(sel);
	do {
		do_show_active_process(sel);
		do_show_info(sel);
		do_show_file_table(sel);
		do_change_multiprogramming(sel);
		do_stop_process(sel);
		do_stop_planification(sel);
		ask_option(sel);
	} while (true);

	return EXIT_SUCCESS;
}
