#include "filesystem.h"

const char * CONFIG_FIELDS[] = { PORT, MOUNT_POINT };

void create_function_dictionary() {
	fns = dictionary_create();
}

void init_filesystem(t_config* config) {
	log_debug(logger, "Initiating FILESYSTEM.");

	int port = config_get_int_value(config, PORT);

	if (createListen(port, &newClient, fns, &connectionClosed, NULL) == -1) {
		log_error(logger, "Error at creating listener at port %d", port);
		exit(EXIT_FAILURE);
	}
	log_debug(logger, "Listening new clients at %d.\n", port);

	pthread_mutex_init(&mx_main, NULL);
	pthread_mutex_lock(&mx_main);
	pthread_mutex_lock(&mx_main);
}

int main(int argc, char *argv[]) {
	clear_screen();
	t_config *config = malloc(sizeof(t_config));

	remove("log");
	logger = log_create("log", "FILESYSTEM", false, LOG_LEVEL_DEBUG);

	create_function_dictionary();

	load_config(&config, argc, argv[1]);
	print_config(config, CONFIG_FIELDS, CONFIG_FIELDS_N);

	init_filesystem(config);

	return EXIT_SUCCESS;
}

