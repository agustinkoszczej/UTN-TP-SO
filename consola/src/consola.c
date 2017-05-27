#include "consola.h"

const char* CONFIG_FIELDS[] = { IP_KERNEL, PUERTO_KERNEL };

t_process* find_process_by_socket(int socket) {
	pthread_mutex_lock(&process_list_mutex);
	bool find(void* element) {
		t_process* process = element;
		return process->socket == socket;
	}

	t_process* process = list_find(process_list, find);
	pthread_mutex_unlock(&process_list_mutex);

	return process;
}

t_process* find_process_by_pid(int pid) {
	pthread_mutex_lock(&process_list_mutex);
	bool find_pid(void* element) {
		t_process* p = element;
		return p->pid == pid;
	}

	t_process* process = list_find(process_list, find_pid);
	pthread_mutex_unlock(&process_list_mutex);
	return process;
}

void remove_from_process_list(t_process* process) {
	int i;
	pthread_mutex_lock(&process_list_mutex);
	for (i = 0; i < list_size(process_list); i++) {
		t_process* process2 = list_get(process_list, i);
		if (process->socket == process2->socket) {
			list_remove(process_list, i);
			break;
		}
	}
	pthread_mutex_unlock(&process_list_mutex);
}

void show_message(int i) {
	t_message* message = list_get(messages_list, i);
	printf("[%s] [%d] %s\n", message->time, message->pid, message->message);
}

void print_menu() {
	int i;
	pthread_mutex_lock(&print_menu_mutex);
	clear_screen();

	printf("[Procesos: %02d]\n", p_counter);
	pthread_mutex_lock(&messages_list_mutex);
	for (i = 0; i < 5; i++)
		if (i < list_size(messages_list))
			show_message(i);
		else
			println("");
	pthread_mutex_unlock(&messages_list_mutex);

	println("");
	show_title("CONSOLE - MAIN MENU");
	println("Enter your choice:");
	println("> START_PROGRAM");
	println("> ABORT_PROGRAM");
	println("> DISCONNECT_CONSOLE");
	println("> CLEAR_MESSAGES\n");
	pthread_mutex_unlock(&print_menu_mutex);
}

void new_message(char* text, int pid) {
	t_message* message = malloc(sizeof(t_message));
	message->pid = pid;
	message->message = text;
	message->time = temporal_get_string_time();

	t_process* process = find_process_by_pid(pid);
	process->c_message++;

	pthread_mutex_lock(&messages_list_mutex);
	list_add(messages_list, message);
	if (list_size(messages_list) > 5)
		list_remove(messages_list, 0);
	pthread_mutex_unlock(&messages_list_mutex);

	print_menu();
}

void create_function_dictionary() {
	fns = dictionary_create();

	dictionary_put(fns, "kernel_print_message", &kernel_print_message);
	dictionary_put(fns, "kernel_response_load_program", &kernel_response_load_program);
	dictionary_put(fns, "kernel_stop_process", &kernel_stop_process);
}

void start_program(char* location) {
	char* buffer;
	int size;
	t_process* process = malloc(sizeof(t_process));

	char* time_start = temporal_get_string_time();
	process->time_start = malloc(string_length(time_start));
	process->time_start = time_start;

	process->c_message = 0;
	process->pid = -1;

	if ((process->socket = connectServer(ip, port, fns, &server_connectionClosed, NULL)) == -1) {
		log_error(logger, "Error at connecting to KERNEL. IP = %s, Port = %d.", ip, port);
		new_message("Error at creating process.", -1);
		return;
	}
	//log_debug(logger, "Connected to KERNEL. Socket = %d, IP = %s, Port = %d.", process->socket, ip, port);

	FILE* file = fopen(string_from_format("resources/%s", location), "r");
	if (file == NULL) {
		log_error(logger, "Couldn't open file %s", location);
		new_message("Couldn't open file", -1);
		return;
	}

	fseek(file, 0L, SEEK_END);
	size = ftell(file);
	fseek(file, 0L, SEEK_SET);

	buffer = malloc(size);
	fread(buffer, size, 1, file);
	buffer = string_substring_until(buffer, size);

	pthread_mutex_lock(&process_list_mutex);
	list_add(process_list, process);
	pthread_mutex_unlock(&process_list_mutex);

	runFunction(process->socket, "console_load_program", 1, buffer);
}

void ask_option(char *sel) {
	fgets(sel, 255, stdin);
	strtok(sel, "\n");
	string_to_upper(sel);
}

void do_disconnect_console(char* sel) {
	if (!strcmp(sel, "D")) {
		int i;
		for (i = 0; i < list_size(process_list); i++) {
			pthread_mutex_lock(&process_list_mutex);
			t_process* process = list_get(process_list, i);
			pthread_mutex_unlock(&process_list_mutex);
			abort_program(process, DESCONEXION_CONSOLA);
		}
		pthread_mutex_lock(&process_list_mutex);
		list_clean(process_list);
		pthread_mutex_unlock(&process_list_mutex);
	}
}

void do_start_program(char* sel) {
	if (!strcmp(sel, "S")) {
		char location[255];

		pthread_mutex_lock(&print_menu_mutex);
		printf("> File path: ");
		fgets(location, sizeof(location), stdin);
		strtok(location, "\n");
		pthread_mutex_unlock(&print_menu_mutex);

		start_program(location);
	}
}

void abort_program(t_process* process, int exit_code) {
	if (process->pid > 0) {
		char* time_finish = temporal_get_string_time();
		process->time_finish = malloc(string_length(time_finish));
		process->time_finish = time_finish;

		new_message(dictionary_get(message_map, string_itoa(exit_code)), process->pid);
		char* message = string_from_format("[%s] [%s] [%d]", process->time_start, process->time_finish, process->c_message);
		new_message(message, process->pid);

		pthread_mutex_lock(&p_counter_mutex);
		p_counter--;
		pthread_mutex_unlock(&p_counter_mutex);

		//log_debug(logger, dictionary_get(message_map, string_itoa(exit_code)));
		//runFunction(process->socket, "console_abort_program", 2, CONSOLE, string_itoa(process->pid));
	}
	close(process->socket);
}

void do_abort_program(char* sel) {
	if (!strcmp(sel, "A")) {
		char pid[255];

		pthread_mutex_lock(&print_menu_mutex);
		printf("> PID: ");
		fgets(pid, sizeof(pid), stdin);
		strtok(pid, "\n");
		pthread_mutex_unlock(&print_menu_mutex);

		int i;
		pthread_mutex_lock(&process_list_mutex);
		for (i = 0; i < list_size(process_list); i++) {
			pthread_mutex_unlock(&process_list_mutex);

			pthread_mutex_lock(&process_list_mutex);
			t_process* process = list_get(process_list, i);
			pthread_mutex_unlock(&process_list_mutex);

			if (!strcmp(string_itoa(process->pid), pid)) {
				abort_program(process, DESCONEXION_CONSOLA);

				pthread_mutex_lock(&process_list_mutex);
				list_remove(process_list, i);
				pthread_mutex_unlock(&process_list_mutex);

				break;
			}
			pthread_mutex_lock(&process_list_mutex);
		}
		pthread_mutex_unlock(&process_list_mutex);
	}
}

void do_clear_messages(char* sel) {
	if (!strcmp(sel, "C")) {
		pthread_mutex_lock(&messages_list_mutex);
		list_clean(messages_list);
		pthread_mutex_unlock(&messages_list_mutex);
	}
}

void config_connection(t_config* config) {
	if (config_has_property(config, IP_KERNEL) && config_has_property(config, PUERTO_KERNEL)) {
		ip = config_get_string_value(config, IP_KERNEL);
		port = config_get_int_value(config, PUERTO_KERNEL);
	}
}

void init_console() {
	p_counter = 0;
	pthread_mutex_init(&p_counter_mutex, NULL);
	pthread_mutex_init(&process_list_mutex, NULL);
	pthread_mutex_init(&messages_list_mutex, NULL);
	pthread_mutex_init(&print_menu_mutex, NULL);

	messages_list = list_create();
	process_list = list_create();
	message_map = dictionary_create();

	dictionary_put(message_map, string_itoa(NO_SE_PUEDEN_RESERVAR_RECURSOS), "No se pueden reservar recursos.");
	dictionary_put(message_map, string_itoa(ERROR_SIN_DEFINIR), "Error sin definición.");
	dictionary_put(message_map, string_itoa(DESCONEXION_CONSOLA), "Desconexion de consola.");
}

int main(int argc, char *argv[]) {
	clear_screen();
	char sel[255];
	t_config* config = malloc(sizeof(t_config));

	//remove("log");
	//logger = log_create("log", "CONSOLE", false, LOG_LEVEL_DEBUG);

	create_function_dictionary();

	load_config(&config, argc, argv[1]);
	config_connection(config);
	print_config(config, CONFIG_FIELDS, CONFIG_FIELDS_N);

	init_console();

	wait_any_key();

	do {
		print_menu();
		ask_option(sel);
		do_start_program(sel);
		do_abort_program(sel);
		do_disconnect_console(sel);
		do_clear_messages(sel);
	} while (true);

	return EXIT_SUCCESS;
}
