#include "filesystem.h"

const char * CONFIG_FIELDS[] = { PORT, MOUNT_POINT };

void create_function_dictionary() {
	fns = dictionary_create();

	dictionary_put(fns, "validateFile", &validateFile);
	dictionary_put(fns, "createFile", &createFile);
	dictionary_put(fns, "deleteFile", &deleteFile);
	dictionary_put(fns, "getData", &getData);
	dictionary_put(fns, "saveData", &saveData);
}

void init_filesystem(t_config* config) {
	log_debug(logger, "Initiating FILESYSTEM.");

	int port = config_get_int_value(config, PORT);
	char* mnt = config_get_string_value(config, MOUNT_POINT);
	mount_point = malloc(string_length(mnt));
	mount_point = mnt;

	if (createListen(port, &newClient, fns, &connectionClosed, NULL) == -1) {
		log_error(logger, "Error at creating listener at port %d", port);
		exit(EXIT_FAILURE);
	}
	log_debug(logger, "Listening new clients at %d.\n", port);

	pthread_mutex_init(&mx_main, NULL);
	pthread_mutex_init(&request_mutex, NULL);
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

