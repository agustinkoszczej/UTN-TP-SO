#include "memoria.h"
#include "memoria_interface.h"

const char *CONFIG_FIELDS[] = { PUERTO, MARCOS, MARCO_SIZE,
ENTRADAS_CACHE, CACHE_X_PROC, RETARDO_MEMORIA };

void create_function_dictionary() {
	fns = dictionary_create();

	dictionary_put(fns, "client_identify", &client_identify);

	dictionary_put(fns, "i_start_program", &i_start_program);
	dictionary_put(fns, "i_read_bytes_from_page", &i_read_bytes_from_page);
	dictionary_put(fns, "i_store_bytes_in_page", &i_store_bytes_in_page);
	dictionary_put(fns, "i_add_pages_to_program", &i_add_pages_to_program);
	dictionary_put(fns, "i_finish_program", &i_finish_program);

	dictionary_put(fns, "kernel_stack_size", &kernel_stack_size);
}

void mem_allocate_fullspace() {
	pthread_mutex_lock(&frames_mutex);

	frames = malloc(frames_count * frame_size);
	frames_cache = malloc(cache_size * frame_size);

	pthread_mutex_unlock(&frames_mutex);
}

void print_menu() {
	clear_screen();

	show_title("MEMORY - MAIN MENU");
	println("Enter your choice:");
	println("> RETARD");
	println("> DUMP");
	println("> FLUSH");
	println("> SIZE\n");
}

bool has_available_frames(int n_frames) {
	pthread_mutex_lock(&frames_mutex);
	bool find(void* element) {
		t_adm_table* adm_table = element;
		return adm_table->pid < 0;
	}

	int available_pages = list_size(list_filter(adm_list, find));
	pthread_mutex_unlock(&frames_mutex);
	return available_pages >= n_frames;
}

void start_program(int pid, int n_frames) {
	pthread_mutex_lock(&frames_mutex);
	int page_c = n_frames;
	int i;
	for (i = 0; i < list_size(adm_list); i++) {
		t_adm_table* adm_table = list_get(adm_list, i);
		if (adm_table->pid < 0) {
			adm_table->pid = pid;
			adm_table->frame = i;
			adm_table->pag = n_frames - page_c;
			if (--page_c <= 0)
				break;
		}
	}
	pthread_mutex_unlock(&frames_mutex);
	add_pages(pid, stack_size);
}

void finish_program(int pid) {
	pthread_mutex_lock(&frames_mutex);
	bool find(void* element) {
		t_adm_table* adm_table = element;
		return adm_table->pid == pid;
	}

	t_list* adm_tables = list_filter(adm_list, find);
	int i;
	for (i = 0; i < list_size(adm_tables); i++) {
		t_adm_table* adm_table = list_get(adm_tables, i);
		adm_table->pid = -1;
	}
	pthread_mutex_unlock(&frames_mutex);

	pthread_mutex_lock(&frames_cache_mutex);
	for (i = 0; i < list_size(cache_list); i++) {
		t_cache* cache = list_get(cache_list, i);
		cache->adm_table->pid = -1;
	}
	pthread_mutex_unlock(&frames_cache_mutex);
}

void add_pages(int pid, int n_frames) {
	pthread_mutex_lock(&frames_mutex);
	bool find(void* element) {
		t_adm_table* adm_table = element;
		return adm_table->pid == pid;
	}
	int i;

	int known_pages = list_size(list_filter(adm_list, find));

	for (i = 0; i < list_size(adm_list); i++) {
		t_adm_table* adm_table = list_get(adm_list, i);
		if (adm_table->pid < 0) {
			adm_table->pid = pid;
			adm_table->pag = known_pages++;
			if (--n_frames <= 0)
				break;
		}
	}
	pthread_mutex_unlock(&frames_mutex);
}

t_adm_table* get_from_cache(int pid, int page) {
	bool find(void* element) {
		t_cache* n_cache = element;
		return n_cache->adm_table->pag == page && n_cache->adm_table->pid == pid;
	}

	t_cache* n_cache = list_find(cache_list, find);
	return n_cache->adm_table;
}

void increase_cache_lru() {
	pthread_mutex_lock(&frames_cache_mutex);
	int i;
	for (i = 0; i < list_size(cache_list); i++) {
		t_cache* cache = list_get(cache_list, i);
		cache->lru++;
	}
	pthread_mutex_unlock(&frames_cache_mutex);
}

bool exists_in_cache(int pid, int page) {
	bool find(void* element) {
		t_cache* cache = element;
		return cache->adm_table->pag == page && cache->adm_table->pid == pid;
	}

	return list_any_satisfy(cache_list, find);
}

void save_victim(t_adm_table* adm_table) {
	char* buffer_cache = read_bytes(adm_table->pid, adm_table->pag, 0, frame_size);
	store_bytes(adm_table->pid, adm_table->pag, 0, frame_size, buffer_cache);
}

int find_cache_victim() {
	int i, lru_max = -1, pos;
	t_cache* cache;
	for (i = 0; i < list_size(cache_list); i++) {
		cache = list_get(cache_list, i);
		if (cache->lru > lru_max) {
			lru_max = cache->lru;
			pos = i;
		}
	}

	save_victim(cache->adm_table);
	return pos;
}

int find_free_pos_cache() {
	int i;
	for (i = 0; i < list_size(cache_list); i++) {
		t_cache* cache = list_get(cache_list, i);
		if (cache->adm_table->pid == -1)
			return i;
	}
	return -1;
}

void store_in_cache(t_adm_table* n_adm_table) {
	pthread_mutex_lock(&frames_cache_mutex);
	bool find(void* element) {
		t_adm_table* adm_table = element;
		return adm_table->pid == n_adm_table->pid;
	}

	int cache_proc_size = list_size(list_filter(adm_list, find));
	if (cache_proc_size < 3) {
		int free_pos = find_free_pos_cache();
		if (free_pos < 0) {
			free_pos = find_cache_victim();
		}

		t_cache* n_cache = list_get(cache_list, free_pos);
		n_cache->adm_table->pag = n_adm_table->pag;
		n_cache->adm_table->pid = n_adm_table->pid;
		n_cache->lru = -1;
	}
	pthread_mutex_unlock(&frames_cache_mutex);
	increase_cache_lru();
}

char* read_bytes_cache(int pid, int page, int offset, int size) {
	pthread_mutex_lock(&frames_cache_mutex);
	t_adm_table* adm_table = get_from_cache(pid, page);

	int start = frame_size * adm_table->frame + offset;
	int end = start + size;
	char* buffer = string_substring(frames_cache, start, end);
	pthread_mutex_unlock(&frames_cache_mutex);
	return buffer;
}

void store_bytes_cache(int pid, int page, int offset, int size, char* buffer) {
	pthread_mutex_lock(&frames_cache_mutex);
	t_adm_table* adm_table = get_from_cache(pid, page);

	int start = frame_size * adm_table->frame + offset;
	int end = start + size;

	int i, b = 0;
	for (i = start; i < end; i++) {
		frames_cache[i] = buffer[b];
		b++;
	}
	pthread_mutex_unlock(&frames_cache_mutex);
}

char* read_bytes(int pid, int page, int offset, int size) {
	pthread_mutex_lock(&frames_mutex);
	bool find(void* element) {
		t_adm_table* adm_table = element;
		return adm_table->pag == page && adm_table->pid == pid;
	}

	t_adm_table* adm_table;
	if (exists_in_cache(pid, page)) {
		adm_table = get_from_cache(pid, page);
	} else {
		adm_table = list_find(adm_list, find);
		sleep(mem_delay / 1000);
	}

	int start = frame_size * adm_table->frame + offset;
	char* buffer = string_substring(frames, start, size);

	pthread_mutex_unlock(&frames_mutex);
	store_in_cache(adm_table);
	return buffer;
}

void store_bytes(int pid, int page, int offset, int size, char* buffer) {
	pthread_mutex_lock(&frames_mutex);
	bool find(void* element) {
		t_adm_table* adm_table = element;
		return adm_table->pag == page && adm_table->pid == pid;
	}

	t_adm_table* adm_table;
	bool is_cache = exists_in_cache(pid, page);
	if (is_cache) {
		adm_table = get_from_cache(pid, page);
	} else {
		adm_table = list_find(adm_list, find);
		sleep(mem_delay / 1000);
	}

	int start = frame_size * adm_table->frame + offset;
	int end = start + size;

	int i, b = 0;
	for (i = start; i < end; i++) {
		if (is_cache)
			frames_cache[i] = buffer[b];
		else
			frames[i] = buffer[b];
		b++;
	}
	pthread_mutex_unlock(&frames_mutex);
	if (!is_cache)
		store_in_cache(adm_table);
}

void init_memory(t_config *config) {
	int port = config_get_int_value(config, PUERTO);

	pthread_mutex_init(&frames_mutex, NULL);
	pthread_mutex_init(&frames_cache_mutex, NULL);
	pthread_mutex_init(&cpu_sockets_mutex, NULL);

	frames_count = config_get_int_value(config, MARCOS);
	frame_size = config_get_int_value(config, MARCO_SIZE);
	mem_delay = config_get_int_value(config, RETARDO_MEMORIA);
	cache_size = config_get_int_value(config, ENTRADAS_CACHE);
	cache_x_proc = config_get_int_value(config, CACHE_X_PROC);

	mem_allocate_fullspace();

	m_sockets.cpu_sockets = list_create();
	adm_list = list_create();
	cache_list = list_create();

	int i;
	for (i = 0; i < frames_count; i++) {
		t_adm_table* adm_table = malloc(sizeof(t_adm_table));
		adm_table->frame = i;
		adm_table->pag = 0;
		adm_table->pid = -1;

		list_add(adm_list, adm_table);
	}
	for (i = 0; i < cache_size; i++) {
		t_cache* cache = malloc(sizeof(t_cache));
		t_adm_table* adm_table = malloc(sizeof(t_adm_table));
		adm_table->frame = i;
		adm_table->pag = 0;
		adm_table->pid = -1;

		cache->adm_table = adm_table;
		cache->lru = -1;

		list_add(cache_list, cache);
	}

	if ((m_sockets.k_socket = createListen(port, &newClient, fns, &connectionClosed, NULL) == -1)) {
		log_error(logger, "Error at creating listener at port %d", port);
		error_show(" at creating listener at port %d", port);
		exit(EXIT_FAILURE);
	}
	log_debug(logger, "Listening new clients at %d.\n", port);
}

void dump(int pid) {
	int i;

	pthread_mutex_lock(&frames_cache_mutex);
	char* dump_cache = string_new();
	char* dump_cache_struct = string_new();
	char* dump_cache_content = string_from_format("%s", frames_cache);

	for (i = 0; i < list_size(cache_list); i++) {
		t_cache* cache = list_get(cache_list, i);
		string_append_with_format(&dump_cache_struct, "PID: %d | PAGE: %d | LRU COUNTER: %d\n", cache->adm_table->pid, cache->adm_table->pag, cache->lru);
	}
	dump_cache = string_from_format("%s\n\n%s", dump_cache_struct, dump_cache_content);
	pthread_mutex_unlock(&frames_cache_mutex);

	pthread_mutex_lock(&frames_mutex);
	char* dump_mem_struct = string_new();
	char* dump_act_process = string_new();
	char* dump_mem_content;

	if (pid == -1) {
		string_append(&dump_mem_content, frames);
	} else {
		for (i = 0; i < list_size(adm_list); i++) {
			t_adm_table* adm_table = list_get(adm_list, i);
			if (adm_table->pid == pid) {
				char* frame = string_substring(frames, adm_table->frame * frame_size, frame_size);
				string_append_with_format(&dump_mem_content, "FRAME: %d | PID: %d | PAGE: %d\n%s\n", adm_table->frame, adm_table->pid, adm_table->pag, frame);
			}
		}
	}

	for (i = 0; i < frames_count; i++) {
		t_adm_table* adm_table = list_get(adm_list, i);
		string_append_with_format(&dump_mem_struct, "FRAME: %d | PID: %d | PAGE: %d\n", adm_table->frame, adm_table->pid, adm_table->pag);

		if (adm_table->pid >= 0)
			string_append_with_format(&dump_act_process, "ACTIVE PID: %d\n", adm_table->pid);
	}

	char* dump_total = string_new();
	string_append_with_format(&dump_total, "CACHE:\n%s\n", dump_cache);
	string_append_with_format(&dump_total, "TABLA DE PAGINAS:\n%s\n", dump_act_process);
	string_append_with_format(&dump_total, "LISTADO DE PROCESOS ACTIVOS:%s\n", dump_mem_struct);
	string_append_with_format(&dump_total, "CONTENIDO MEMORIA:%s\n\n", dump_mem_content);

	pthread_mutex_unlock(&frames_mutex);
	clear_screen();
	printf("%s", dump_total);

	wait_any_key();
}

void do_dump(char* sel) {
	if (!strcmp(sel, "D")) {
		char pid[255];
		printf("> PID (-1 for all): ");
		fgets(pid, sizeof(pid), stdin);
		strtok(pid, "\n");

		dump(atoi(pid));
	}
}

void do_retard(char* sel) {
	if (!strcmp(sel, "R")) {
		char retard[255];
		printf("> Milliseconds [%d]: ", mem_delay);
		fgets(retard, sizeof(retard), stdin);
		strtok(retard, "\n");

		mem_delay = atoi(retard);
	}
}

void do_flush(char* sel) {
	if (!strcmp(sel, "F")) {
		int i;
		for (i = 0; i < list_size(cache_list); i++) {
			t_cache* cache = list_get(cache_list, i);
			save_victim(cache->adm_table);
			cache->adm_table->pid = -1;
		}
	}
}

void size(int option) {
	bool find_used(void* element) {
		t_adm_table* adm_table = element;
		return adm_table->pid < 0;
	}
	bool find_free(void* element) {
		t_adm_table* adm_table = element;
		return adm_table->pid >= 0;
	}
	bool find_pid(void* element) {
		t_adm_table* adm_table = element;
		int pid = option;
		return adm_table->pid == pid;
	}

	char* size_s = string_new();

	pthread_mutex_lock(&frames_mutex);
	if (option < 0) {
		int used_c = list_size(list_filter(adm_list, find_used));
		int free_c = list_size(list_filter(adm_list, find_free));

		string_append_with_format(&size_s, "FRAMES: %d\n", frames_count);
		string_append_with_format(&size_s, "USED: %d\n", used_c);
		string_append_with_format(&size_s, "FREE: %d\n", free_c);
	} else {
		int pid_c = list_size(list_filter(adm_list, find_pid));
		string_append_with_format(&size_s, "USED BY [%d]: %d\n", option, pid_c);
	}
	pthread_mutex_unlock(&frames_mutex);
	clear_screen();
	printf("%s", size_s);

	wait_any_key();
}

void do_size(char* sel) {
	if (!strcmp(sel, "S")) {
		char option[255];
		printf("> PID (-1 for Memory): ");
		fgets(option, sizeof(option), stdin);
		strtok(option, "\n");

		size(atoi(option));
	}
}

void ask_option(char *sel) {
	print_menu();
	fgets(sel, 255, stdin);
	strtok(sel, "\n");
	string_to_upper(sel);
}

int main(int argc, char *argv[]) {
	clear_screen();
	char sel[255];
	t_config* config = malloc(sizeof(t_config));

	remove("log");
	logger = log_create("log", "MEMORY", false, LOG_LEVEL_DEBUG);
	create_function_dictionary();

	load_config(&config, argc, argv[1]);
	init_memory(config);

	print_config(config, CONFIG_FIELDS, CONFIG_FIELDS_N);

	wait_any_key();

	ask_option(sel);
	do {
		do_retard(sel);
		do_dump(sel);
		do_flush(sel);
		do_size(sel);
		ask_option(sel);
	} while (true);

	return EXIT_SUCCESS;
}
