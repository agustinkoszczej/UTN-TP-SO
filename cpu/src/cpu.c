#include "cpu.h"

const char* CONFIG_FIELDS[] = { IP_KERNEL, PUERTO_KERNEL, IP_MEMORIA, PUERTO_MEMORIA };

/*void connect_to_kernel() {
 if ((socket_server = connectServer(ip, port, fns, &server_connectionClosed, NULL)) == -1) {
 error_show(" al intentar conectar a servidor. IP = %s, Port = %d.\n", ip, port);
 exit(EXIT_FAILURE);
 }
 printf("Connected to server. Socket = %d, IP = %s, Port = %d.\n", socket_server, ip, port);
 }

 void print_menu() {
 clear_screen();
 show_title("CPU - MAIN MENU");
 println("Enter your choice:");
 println("> START_CPU");
 println("> EXIT_CPU\n");
 }

 void ask_option(char *sel) {
 print_menu();
 fgets(sel, sizeof(char) * 255, stdin);
 strtok(sel, "\n");
 string_to_upper(sel);
 }

 void do_ask_option(char* sel, int close_flag) {
 if (!close_flag)
 ask_option(sel);
 }

 void do_start_cpu(char* sel) {
 if (!strcmp(sel, "START_CPU")) {
 println_with_clean(">>>> INITIATING CPU");
 connect_to_kernel();
 pthread_mutex_init(&mx_main, NULL);
 pthread_mutex_lock(&mx_main);
 pthread_mutex_lock(&mx_main);
 }
 }

 void do_exit_cpu(char* sel, int *close_flag) {
 if (!strcmp(sel, "EXIT_CPU")) {
 println_with_clean(">>>> CLOSING");
 *close_flag = 1;
 }
 }*/

void create_function_dictionary() {
	fns = dictionary_create();

	dictionary_put(fns, "memory_identify", &memory_identify);
	dictionary_put(fns, "kernel_receive_pcb", &kernel_receive_pcb);
	dictionary_put(fns, "memory_response_read_bytes_from_page", &memory_response_read_bytes_from_page);
	dictionary_put(fns, "kernel_page_size", &kernel_page_size);
}
void connect_to_server(t_config* config, char* name) {
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
}

void init_cpu(t_config* config) {
	log_debug(logger, "Initiating CPU");

	pthread_mutex_init(&mx_main, NULL);
	pthread_mutex_init(&planning_mutex, NULL);

	connect_to_server(config, KERNEL);
	connect_to_server(config, MEMORY);

	FRAME_SIZE = 256;

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

	pthread_mutex_lock(&mx_main);
	pthread_mutex_lock(&mx_main);
}

int main(int argc, char *argv[]) {
	clear_screen();
	char sel[255];
	t_config* config = malloc(sizeof(t_config));

	remove("log");
	logger = log_create("log", "CPU", false, LOG_LEVEL_DEBUG);

	create_function_dictionary();
	load_config(&config, argc, argv[1]);
	print_config(config, CONFIG_FIELDS, CONFIG_FIELDS_N);

	init_cpu(config);

	return EXIT_SUCCESS; // Fin exitoso
}
