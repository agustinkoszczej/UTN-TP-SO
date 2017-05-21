#include "config.h"

void load_config(t_config **config, int argc, char* dir) {
	if (argc == 2) {
		*config = config_create(dir);

		if (!config) {
			error_show(" config file not found.\n");
			exit(EXIT_FAILURE);
		}
	} else {
		error_show(" config file not declared.\n");
		exit(EXIT_FAILURE);
	}
}

void print_config(t_config *config, const char *config_fields[], int total_fields) {
	int i;

	show_title(CONFIG_TITLE);

	for (i = 0; i < total_fields; i++) {
		char* field = (char*) config_fields[i];
		if (config_has_property(config, field)) {
			char* value = config_get_string_value(config, field);
			printf("%s=%s\n", field, value);
		}
	}
	println("");
}
