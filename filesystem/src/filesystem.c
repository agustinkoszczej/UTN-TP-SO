#include "filesystem.h"

const char * CONFIG_FIELDS[] = { PORT, MOUNT_POINT };

void update_bitmap_file() {
	log_debug(logger, "update_bitmap_file: void");

	FILE* bitmap_f = fopen(string_from_format("%s/Metadata/Bitmap.bin", mount_point), "w");
	fputs(bitmap->bitarray, bitmap_f);
	fclose(bitmap_f);

	log_debug(logger, "update_bitmap_file: void");
}

int create_block() {
	log_debug(logger, "create_block: void");

	int i;
	for (i = 0; i < bitmap->size; i++) {
		if (!bitarray_test_bit(bitmap, i)) {
			bitarray_set_bit(bitmap, i);
			FILE* f = fopen(string_from_format("%s/Bloques/%d.bin", mount_point, i), "w");
			if (f == NULL)
				return -1;

			fclose(f);
			update_bitmap_file();
			log_debug(logger, "create_block: int=%d", i);
			return i;
		}
	}

	log_debug(logger, "create_block: int=%d", -1);
	return -1;
}

bool add_blocks_if_needed(char* path, int offset, int size) {
	log_debug(logger, "add_blocks_if_needed: path=%s, offset=%d, size=%d", path, offset, size);

	char* path_file = string_from_format("%s/Archivos/%s", mount_point, path);
	t_config* config_file = config_create(path_file);

	char* buffer_blocks_array = config_get_string_value(config_file, BLOQUES);
	buffer_blocks_array = string_substring_until(buffer_blocks_array, string_length(buffer_blocks_array) - 1);

	char** blocks_arr = config_get_array_value(config_file, BLOQUES);

	int end_block = (offset + size) / block_size;

	int asigned_blocks = 0;
	while (blocks_arr[asigned_blocks] != NULL)
		asigned_blocks++;

	while(asigned_blocks < end_block) {
		int new_block_pos = create_block();

		if (new_block_pos < 0) {
			log_debug(logger, "add_blocks_if_needed: bool=%d", false);
			return false;
		} else
			string_append_with_format(&buffer_blocks_array, ",%d", new_block_pos);

		asigned_blocks++;
	}
	string_append(&buffer_blocks_array, "]");

	int file_size = config_get_int_value(config_file, TAMANIO);

	FILE* f = fopen(path_file, "w");
	fputs(string_from_format("TAMANIO=%d\n", file_size), f);
	fputs(string_from_format("BLOQUES=%s", buffer_blocks_array), f);
	fclose(f);

	free(path_file);
	config_destroy(config_file);
	free(buffer_blocks_array);
	int i = 0;
	while (blocks_arr[i] != NULL)
		free(blocks_arr[++i]);
	free(blocks_arr);

	log_debug(logger, "add_blocks_if_needed: bool=%d", true);
	return true;
}

void write_block(int block_pos, int offset, int length, char* buffer) {
	log_debug(logger, "write_block: block_pos=%d, offset=%d, length=%d, buffer=%s", block_pos, offset, length, buffer);

	FILE* block = fopen(string_from_format("%s/Bloques/%d.bin", mount_point, block_pos), "r+");
	fseek(block, offset, SEEK_SET);
	fwrite(buffer, 1, length, block);
	fclose(block);

	log_debug(logger, "write_block: void");
}

void update_file_size(char* path, int offset, int size) {
	log_debug(logger, "update_file_size: path=%s, offset=%d, size=%d", path, offset, size);

	char* path_file = string_from_format("%s/Archivos/%s", mount_point, path);
	t_config* config_file = config_create(path_file);

	char* blocks_arr = config_get_string_value(config_file, BLOQUES);
	int file_size = config_get_int_value(config_file, TAMANIO);
	int diff = file_size - (offset + size);

	if (diff < 0) {
		file_size += abs(diff);

		FILE* f = fopen(path_file, "w");
		fputs(string_from_format("TAMANIO=%d\n", file_size), f);
		fputs(string_from_format("BLOQUES=%s", blocks_arr), f);
		fclose(f);
	}

	log_debug(logger, "update_file_size: void");
}

bool save_data(char* path, int offset, int size, char* buffer) {
	log_debug(logger, "save_data: path=%s, offset=%d, size=%d, buffer=%s", path, offset, size, buffer);
	int initial_size = size;
	if (!add_blocks_if_needed(path, offset, size)) {
		log_debug(logger, "save_data: bool=%d", false);
		return false;
	}

	char* path_file = string_from_format("%s/Archivos/%s", mount_point, path);
	t_config* config_file = config_create(path_file);

	char** blocks_arr = config_get_array_value(config_file, BLOQUES);

	int start_block = offset / block_size;
	int end_block = (offset + size) / block_size;

	int offset2 = offset - start_block * block_size;
	int len = (size > block_size) ? block_size : size;
	len -= offset2;
	write_block(atoi(blocks_arr[start_block]), offset2, len, string_substring_until(buffer, len));
	free(blocks_arr[start_block]);
	start_block++;
	size -= len;

	int written_chars = len;

	while (start_block < end_block && blocks_arr[start_block] != NULL) {
		int b_len = (size > block_size) ? block_size : size;

		write_block(atoi(blocks_arr[start_block]), 0, b_len, string_substring(buffer, written_chars, b_len));
		start_block++;
		size -= b_len;
		written_chars += b_len;
		free(blocks_arr[start_block]);
	}

	free(blocks_arr);
	config_destroy(config_file);
	free(path_file);

	update_file_size(path, offset, initial_size);

	log_debug(logger, "save_data: bool=%d", true);
	return true;
}

char* get_block_data(int pos) {
	log_debug(logger, "get_block_data: pos=%d", pos);

	FILE* block_f = fopen(string_from_format("%s/Bloques/%d.bin", mount_point, pos), "r");

	int size;
	char* buffer;
	fseek(block_f, 0L, SEEK_END);
	size = ftell(block_f);
	fseek(block_f, 0L, SEEK_SET);

	buffer = malloc(size);
	fread(buffer, size, 1, block_f);
	buffer = string_substring_until(buffer, size);

	fclose(block_f);

	log_debug(logger, "get_block_data: char*=%s", buffer);
	return buffer;
}

char* get_data(char* path, int offset, int size) {
	log_debug(logger, "get_data: path=%s, offset=%d, size=%d", path, offset, size);

	char* path_file = string_from_format("%s/Archivos/%s", mount_point, path);
	t_config* config_file = config_create(path_file);

	int start_block = offset / block_size;
	int end_block = (offset + size) / block_size;

	if (config_get_int_value(config_file, TAMANIO) < offset + size) {
		free(path_file);
		config_destroy(config_file);

		return "";
	}

	char** blocks_arr = config_get_array_value(config_file, BLOQUES);

	char* buffer = string_new();
	while (start_block <= end_block && blocks_arr[start_block] != NULL) {
		int block_pos = atoi(blocks_arr[start_block]);
		char* block_buffer = get_block_data(block_pos);
		int block_size = string_length(block_buffer);

		int len = (size - block_size < 0) ? size : block_size;
		char* block_result = string_substring_until(block_buffer, len);

		string_append(&buffer, block_result);

		free(block_buffer);
		free(block_result);
		free(blocks_arr[start_block]);

		start_block++;
		size -= len;
	}

	free(blocks_arr);
	config_destroy(config_file);

	log_debug(logger, "get_data: char*=%s", buffer);
	return buffer;
}

bool validate_file(char* path) {
	log_debug(logger, "validate_file: path=%s", path);

	FILE* file = fopen(string_from_format("%s/Archivos/%s", mount_point, path), "r");
	bool result = false;

	if (file) {
		result = true;
		fclose(file);
	}

	log_debug(logger, "validate_file: bool=%d", result);
	return result;
}

void free_blocks(char** blocks_arr) {
	log_debug(logger, "free_blocks: blocks_arr**");

	int i = 0;
	while (blocks_arr[i] != NULL) {
		int bit_pos = atoi(blocks_arr[i]);
		bitarray_clean_bit(bitmap, bit_pos);
		i++;
	}

	update_bitmap_file();

	log_debug(logger, "free_blocks: void");
}

bool delete_file(char* path) {
	log_debug(logger, "delete_file: path=%s", path);

	char* path_file = string_from_format("%s/Archivos/%s", mount_point, path);
	if (validate_file(path)) {
		t_config* config_file = config_create(path_file);
		char** blocks_arr = config_get_array_value(config_file, BLOQUES);
		config_destroy(config_file);

		free_blocks(blocks_arr);
		remove(path_file);

		log_debug(logger, "delete_file: bool=%d", true);
		return true;
	}

	log_debug(logger, "delete_file: bool=%d", false);
	return false;
}

bool create_file(char* path) {
	log_debug(logger, "create_file: path=%s", path);
	int first_block_pos = create_block();

	if (first_block_pos < 0) {
		log_debug(logger, "create_file: bool=%d", false);
		return false;
	}

	FILE *file = fopen(string_from_format("%s/Archivos/%s", mount_point, path), "w");
	fputs("TAMANIO=0\n", file);
	char* bloq = string_from_format("BLOQUES=[%d]", first_block_pos);
	fputs(bloq, file);
	fclose(file);

	log_debug(logger, "create_file: bool=%d", true);
	return true;
}

void create_function_dictionary() {
	log_debug(logger, "create_function_dictionary: void");

	fns = dictionary_create();

	dictionary_put(fns, "kernel_validate_file", &kernel_validate_file);
	dictionary_put(fns, "kernel_create_file", &kernel_create_file);
	dictionary_put(fns, "kernel_delete_file", &kernel_delete_file);
	dictionary_put(fns, "kernel_get_data", &kernel_get_data);
	dictionary_put(fns, "kernel_save_data", &kernel_save_data);

	log_debug(logger, "create_function_dictionary: void");
}

void init_bitmap() {
	log_debug(logger, "init_bitmap: void");

	FILE* bitmap_f = fopen(string_from_format("%s/Metadata/Bitmap.bin", mount_point), "r");

	int size;
	char* buffer;
	fseek(bitmap_f, 0L, SEEK_END);
	size = ftell(bitmap_f);
	fseek(bitmap_f, 0L, SEEK_SET);

	buffer = malloc(size);
	fread(buffer, size, 1, bitmap_f);
	buffer = string_substring_until(buffer, size);

	string_append(&buffer, string_repeat('\0', block_quantity / 8 - size));
	bitmap = bitarray_create_with_mode(buffer, block_quantity, MSB_FIRST);

	free(buffer);
	fclose(bitmap_f);

	log_debug(logger, "init_bitmap: void");
}

void init_metadata() {
	log_debug(logger, "init_metadata: void");

	t_config* config_meta = config_create(string_from_format("%s/Metadata/Metadata.bin", mount_point));
	block_size = config_get_int_value(config_meta, TAMANIO_BLOQUES);
	block_quantity = config_get_int_value(config_meta, CANTIDAD_BLOQUES);

	config_destroy(config_meta);

	log_debug(logger, "init_metadata: void");
}

void init_filesystem(t_config* config) {
	log_debug(logger, "init_filesystem: t_config*");

	int port = config_get_int_value(config, PORT);
	char* mnt = string_substring_from(config_get_string_value(config, MOUNT_POINT), 1);
	mount_point = malloc(string_length(mnt));
	mount_point = mnt;

	init_metadata();
	init_bitmap();

	if (createListen(port, &newClient, fns, &connectionClosed, NULL) == -1) {
		log_error(logger, "Error at creating listener at port %d", port);
		exit(EXIT_FAILURE);
	}
	log_debug(logger, "Listening new clients at %d.\n", port);

	pthread_mutex_init(&mx_main, NULL);
	pthread_mutex_init(&request_mutex, NULL);
	pthread_mutex_lock(&mx_main);
	pthread_mutex_lock(&mx_main);

	log_debug(logger, "init_filesystem: void");
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

