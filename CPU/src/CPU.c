#include "cpu.h"
#include "cpu_interface.h"

void connect_to_kernel() {
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
}

int main(int argc, char *argv[]) {
	int close_flag = 0;
	char sel[255];

	fns = dictionary_create();
	dictionary_put(fns, "server_handshake", &server_handshake);
	dictionary_put(fns, "server_identify", &server_identify); // <--
	dictionary_put(fns, "server_print_message", &server_print_message); // <--

	ask_option(sel);
	do {
		do_start_cpu(sel);
		do_exit_cpu(sel, &close_flag);
		do_ask_option(sel, close_flag);
	} while (!close_flag);

	return EXIT_SUCCESS; // Fin exitoso
}
