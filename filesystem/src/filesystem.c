#include "filesystem.h"

const char * CONFIG_FIELDS[] = { PORT, MOUNT_POINT };

void update_bitmap_file() {
	FILE* bitmap_f = fopen(string_from_format("%s/Metadata/Bitmap.bin", mount_point), "w");
	fputs(bitmap->bitarray, bitmap_f);
	fclose(bitmap_f);
}

int create_block() {
	int i;
	for (i = 0; i < bitmap->size; i++) {
		if (bitarray_test_bit(bitmap, i)) {
			bitarray_set_bit(bitmap, i);
			mkdir(string_from_format("%s/Bloques/%d.bin", mount_point, i), 0777);
			update_bitmap_file();
			return i;
		}
	}

	return -1;
}

bool add_blocks_if_needed(char* path, int offset, int size) {
	char* path_file = string_from_format("%s/Archivos/%s", mount_point, path);
	t_config* config_file = config_create(path_file);

	char* buffer_blocks_array = config_get_string_value(config_file, BLOQUES);
	buffer_blocks_array = string_substring_until(buffer_blocks_array, string_length(buffer_blocks_array) - 1);

	char** blocks_arr = config_get_array_value(config_file, BLOQUES);

	int start_block = offset / block_size;
	int end_block = ceil((double) (offset + size) / block_size);

	int asigned_blocks = 0;
	while (blocks_arr[++asigned_blocks] != NULL)
		;
	int total_blocks_needed = end_block - start_block;
	int blocks_to_add = total_blocks_needed - asigned_blocks;
	if (blocks_to_add > 0) {
		int i;
		for (i = 0; i < blocks_to_add; i++) {
			int new_block_pos = create_block();

			if (new_block_pos < 0)
				return false;
			else
				string_append_with_format(&buffer_blocks_array, ",%d", new_block_pos);
		}
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
	return true;
}

void write_block(int block_pos, int offset, int length, char* buffer) {
	FILE* block = fopen(string_from_format("%s/Bloques/%d.bin", mount_point, block_pos), "r+");
	fseek(block, offset, SEEK_SET);
	fwrite(buffer, 1, length, block);
	fclose(block);
}

void update_file_size(char* path, int offset, int size) {
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
}

bool save_data(char* path, int offset, int size, char* buffer) {
	if (!add_blocks_if_needed(path, offset, size))
		return false;

	char* path_file = string_from_format("%s/Archivos/%s", mount_point, path);
	t_config* config_file = config_create(path_file);

	char** blocks_arr = config_get_array_value(config_file, BLOQUES);

	int start_block = offset / block_size;
	int end_block = ceil((double) (offset + size) / block_size);

	int offset2 = offset - start_block * block_size;
	int len = (size > block_size) ? block_size : size;
	len = -offset2;
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

	update_file_size(path, offset, size);

	return true;
}

char* get_block_data(int pos) {
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

	return buffer;
}

char* get_data(char* path, int offset, int size) {
	char* path_file = string_from_format("%s/Archivos/%s", mount_point, path);
	t_config* config_file = config_create(path_file);

	int start_block = offset / block_size;
	int end_block = ceil((double) (offset + size) / block_size);
	char** blocks_arr = config_get_array_value(config_file, BLOQUES);

	char* buffer;
	while (start_block < end_block && blocks_arr[start_block] != NULL) {
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

	return buffer;
}

bool validate_file(char* path) {
	FILE* file = fopen(path, "r");
	bool result = false;

	if (file) {
		result = true;
		fclose(file);
	}

	return result;
}

void free_blocks(char** blocks_arr) {
	int i = 0;
	while (blocks_arr[i] != NULL) {
		int bit_pos = atoi(blocks_arr[i]);
		bitarray_clean_bit(bitmap, bit_pos);
	}

	update_bitmap_file();
}

bool delete_file(char* path) {
	char* path_file = string_from_format("%s/Archivos/%s", mount_point, path);
	if (validate_file(path_file)) {
		t_config* config_file = config_create(path_file);
		char** blocks_arr = config_get_array_value(config_file, BLOQUES);
		config_destroy(config_file);

		free_blocks(blocks_arr);
		remove(path_file);

		return true;
	}

	return false;
}

bool create_file(char* path) {
	char* path_file = string_from_format("%s/Archivos/%s", mount_point, path);
	mkdir(path_file, 0777);

	int first_block_pos = create_block();

	if (first_block_pos < 0)
		return false;

	FILE* f = fopen(path_file, "w");
	fputs("TAMANIO=0\n", f);
	fputs(string_from_format("BLOQUES=[%d]", first_block_pos), f);
	fclose(f);
	return true;
}

void create_function_dictionary() {
	fns = dictionary_create();

	dictionary_put(fns, "kernel_validate_file", &kernel_validate_file);
	dictionary_put(fns, "kernel_create_file", &kernel_create_file);
	dictionary_put(fns, "kernel_delete_file", &kernel_delete_file);
	dictionary_put(fns, "kernel_get_data", &kernel_get_data);
	dictionary_put(fns, "kernel_save_data", &kernel_save_data);
}

void init_bitmap() {
	FILE* bitmap_f = fopen(string_from_format("%s/Metadata/Bitmap.bin", mount_point), "r");

	int size;
	char* buffer;
	fseek(bitmap_f, 0L, SEEK_END);
	size = ftell(bitmap_f);
	fseek(bitmap_f, 0L, SEEK_SET);

	buffer = malloc(size);
	fread(buffer, size, 1, bitmap_f);
	buffer = string_substring_until(buffer, size);

	bitmap = bitarray_create_with_mode(buffer, block_quantity, LSB_FIRST);
	free(buffer);
	fclose(bitmap_f);
}

void init_metadata() {
	t_config* config_meta = config_create(string_from_format("%s/Metadata/Metadata.bin", mount_point));
	block_size = config_get_int_value(config_meta, TAMANIO_BLOQUES);
	block_quantity = config_get_int_value(config_meta, CANTIDAD_BLOQUES);

	config_destroy(config_meta);
}

void init_filesystem(t_config* config) {
	log_debug(logger, "Initiating FILESYSTEM.");

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

