#include "cpu.h"

const char* CONFIG_FIELDS[] = { IP_KERNEL, PUERTO_KERNEL, IP_MEMORIA, PUERTO_MEMORIA };

int calculate_offset_for_var() {
	log_debug(logger, "calculate_offset_for_var: void");

	int space_occupied = 0;
	int i, j;
	for (i = 0; i < list_size(pcb_actual->i_stack); i++) {
		t_stack* stack = list_get(pcb_actual->i_stack, i);

		for (j = 0; j < list_size(stack->vars); j++)
			space_occupied += 4;
	}

	int pages_used = ceil((double) space_occupied / frame_size);

	log_debug(logger, "calculate_offset_for_var: int=%d", space_occupied - pages_used * frame_size);
	return space_occupied - pages_used * frame_size;
}

int calculate_page_for_var() {
	log_debug(logger, "calculate_page_for_var: void");

	int space_occupied = 0;
	int i, j;
	for (i = 0; i < list_size(pcb_actual->i_stack); i++) {
		t_stack* stack = list_get(pcb_actual->i_stack, i);

		for (j = 0; j < list_size(stack->vars); j++)
			space_occupied += 4;
	}

	log_debug(logger, "calculate_page_for_var: int=%d", ceil((double) space_occupied / frame_size));
	return ceil((double) space_occupied / frame_size);
}

int vars_in_stack() {
	log_debug(logger, "calculate_page_for_var: void");

	int vars_c = 0;
	int i, j;
	for (i = 0; i < list_size(pcb_actual->i_stack); i++) {
		t_stack* stack = list_get(pcb_actual->i_stack, i);
		for (j = 0; j < list_size(stack->vars); j++)
			vars_c++;
	}

	log_debug(logger, "calculate_page_for_var: int=%d", vars_c);
	return vars_c;
}

void wait_response(pthread_mutex_t* mutex) {
	/*struct timespec abs_time;
	clock_gettime(CLOCK_REALTIME, &abs_time);
	abs_time.tv_sec += (double) mem_delay / 1000.0f + (double) quantum_sleep / 1000.0f + .5f;*/
	pthread_mutex_lock(mutex);
}

void signal_response(pthread_mutex_t* mutex) {
	pthread_mutex_unlock(mutex);
}

char* get_flag(t_banderas flags) {
	char* n_flag = string_new();
	if (flags.creacion)
		string_append(&n_flag, CREATE);
	if (flags.lectura)
		string_append(&n_flag, READ);
	if (flags.escritura)
		string_append(&n_flag, WRITE);

	log_debug(logger, "get_flag: t_banderas");
	return n_flag;
}

void create_function_dictionary() {
	log_debug(logger, "create_function_dictionary: void");

	fns = dictionary_create();

	//MEMORY
	dictionary_put(fns, "memory_identify", &memory_identify);
	dictionary_put(fns, "memory_retard", &memory_retard);
	dictionary_put(fns, "memory_response_read_bytes_from_page", &memory_response_read_bytes_from_page);
	dictionary_put(fns, "memory_response_store_bytes_in_page", &memory_response_store_bytes_in_page);
	dictionary_put(fns, "memory_response_get_frame_from_pid_and_page", &memory_response_get_frame_from_pid_and_page);
	dictionary_put(fns, "memory_response_get_page_from_pointer", &memory_response_get_page_from_pointer);
	//KERNEL
	dictionary_put(fns, "kernel_receive_pcb", &kernel_receive_pcb);
	dictionary_put(fns, "kernel_page_stack_size", &kernel_page_stack_size);
	dictionary_put(fns, "kernel_response_get_shared_var", &kernel_response_get_shared_var);
	dictionary_put(fns, "kernel_response_set_shared_var", &kernel_response_set_shared_var);
	dictionary_put(fns, "kernel_response_file", &kernel_response_file);
	dictionary_put(fns, "kernel_response_validate_file", &kernel_response_validate_file);
	dictionary_put(fns, "kernel_response", &kernel_response);
	dictionary_put(fns, "kernel_response_read_file", &kernel_response_read_file);
	dictionary_put(fns, "kernel_update_quantum_sleep", &kernel_update_quantum_sleep);


	log_debug(logger, "create_function_dictionary: void");
}

void connect_to_server(t_config* config, char* name) {
	log_debug(logger, "connect_to_server: t_config*, name=%s", name);

	int port;
	char* ip_server;
	int* socket;

	if (!strcmp(name, KERNEL)) {
		ip_server = config_get_string_value(config, IP_KERNEL);
		port = config_get_int_value(config, PUERTO_KERNEL);
		socket = &kernel_socket;
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
	log_debug(logger, "connect_to_server: void");
}

void init_cpu(t_config* config) {
	log_debug(logger, "init_cpu: void");

	pthread_mutex_init(&mx_main, NULL);
	pthread_mutex_init(&planning_mutex, NULL);

	connect_to_server(config, MEMORY);
	connect_to_server(config, KERNEL);

	finished = false;

	functions.AnSISOP_definirVariable = cpu_definirVariable;
	functions.AnSISOP_obtenerPosicionVariable = cpu_obtenerPosicionVariable;
	functions.AnSISOP_dereferenciar = cpu_dereferenciar;
	functions.AnSISOP_asignar = cpu_asignar;
	functions.AnSISOP_obtenerValorCompartida = cpu_obtenerValorCompartida;
	functions.AnSISOP_asignarValorCompartida = cpu_asignarValorCompartida;
	functions.AnSISOP_irAlLabel = cpu_irAlLabel;
	functions.AnSISOP_llamarSinRetorno = cpu_llamarSinRetorno;
	functions.AnSISOP_llamarConRetorno = cpu_llamarConRetorno;
	functions.AnSISOP_finalizar = cpu_finalizar;
	functions.AnSISOP_retornar = cpu_retornar;

	kernel_functions.AnSISOP_wait = kernel_wait;
	kernel_functions.AnSISOP_signal = kernel_signal;
	kernel_functions.AnSISOP_reservar = kernel_reservar;
	kernel_functions.AnSISOP_liberar = kernel_liberar;
	kernel_functions.AnSISOP_abrir = kernel_abrir;
	kernel_functions.AnSISOP_borrar = kernel_borrar;
	kernel_functions.AnSISOP_cerrar = kernel_cerrar;
	kernel_functions.AnSISOP_moverCursor = kernel_moverCursor;
	kernel_functions.AnSISOP_escribir = kernel_escribir;
	kernel_functions.AnSISOP_leer = kernel_leer;


	pthread_mutex_lock(&planning_mutex);
	pthread_mutex_lock(&mx_main);
	pthread_mutex_lock(&mx_main);

	log_debug(logger, "init_cpu: void");
}

int main(int argc, char *argv[]) {
	clear_screen();
	t_config* config = malloc(sizeof(t_config));

	remove("log");
	logger = log_create("log", "CPU", false, LOG_LEVEL_DEBUG);

	create_function_dictionary();
	load_config(&config, argc, argv[1]);
	print_config(config, CONFIG_FIELDS, CONFIG_FIELDS_N);

	init_cpu(config);

	return EXIT_SUCCESS;
}
