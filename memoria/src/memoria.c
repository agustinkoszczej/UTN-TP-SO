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
	dictionary_put(fns, "i_free_page", &i_free_page);
	dictionary_put(fns, "i_get_page_from_pointer", &i_get_page_from_pointer);
	dictionary_put(fns, "i_get_frame_from_pid_and_page",
			&i_get_frame_from_pid_and_page);

	dictionary_put(fns, "kernel_stack_size", &kernel_stack_size);
}

unsigned int hash(int pid, int page) {
	char* str = malloc(strlen(string_itoa(pid)));
	strcpy(str, string_itoa(pid));
	strcat(str, string_itoa(page));
	unsigned int indice = atoi(str) % frames_count;
	free(str);
	return indice;
}

void mem_allocate_fullspace() {
	int mem_size = frames_count * frame_size;

	pthread_mutex_lock(&frames_mutex);

	frames = malloc(mem_size);
	frames_cache = malloc(cache_size * frame_size);

	memset(frames, '#', mem_size);

	pthread_mutex_unlock(&frames_mutex);
}

void print_menu() {
	clear_screen();

	show_title("MEMORY - MAIN MENU");
	println("Enter your choice:");
	println("> [R]ETARD");
	println("> [D]UMP");
	println("> [F]LUSH");
	println("> [S]IZE\n");
}

bool has_available_frames(int n_frames) {
	pthread_mutex_lock(&frames_mutex);
	bool find(void* element) {
		t_adm_table* adm_table = element;
		return adm_table->pid < 0;
	}
	t_list* available_pages_list = list_filter(adm_list, find);
	int available_pages = list_size(available_pages_list);
	list_destroy(available_pages_list);
	pthread_mutex_unlock(&frames_mutex);
	return available_pages >= n_frames;
}

void start_program(int pid, int n_frames) {
	pthread_mutex_lock(&frames_mutex);
	int page_c = n_frames;
	int i;

	// HASH
	while (page_c > 0) {
		int frame_pos = hash(pid, n_frames - page_c);
		t_adm_table* adm_table = list_get(adm_list, frame_pos);
		log_debug(logger, "HASH START(%d, %d) = %d | PID = %d, PAG = %d", pid, n_frames - page_c, frame_pos, adm_table->pid, adm_table->pag);
		log_debug(logger, "%s", (adm_table->pid < 0) ? "EUREKA!" : "SIGA PARTICIPANDO.");
		if (adm_table->pid < 0) {
			adm_table->pid = pid;
			adm_table->frame = frame_pos;
			adm_table->pag = n_frames - page_c;
			update_administrative_register_adm_table(adm_table);
			if (--page_c <= 0)
				break;
		} else
			break;
	}
	// HASH

	for (i = 0; i < list_size(adm_list) && page_c > 0; i++) {
		t_adm_table* adm_table = list_get(adm_list, i);
		if (adm_table->pid < 0) {
			log_debug(logger, "start_program in frame: '%d'", adm_table->frame);
			adm_table->pid = pid;
			adm_table->frame = i;
			adm_table->pag = n_frames - page_c;
			update_administrative_register_adm_table(adm_table);
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

	t_list* adm_tables = list_filter(adm_list, &find);

	if(adm_tables == NULL)
		return;

	int i;
	for (i = 0; i < list_size(adm_tables); i++) {
		t_adm_table* adm_table = list_get(adm_tables, i);
		adm_table->pid = -1;
		adm_table->pag = 0;
		update_administrative_register_adm_table(adm_table);
		clean_frame(adm_table->frame);
	}
	pthread_mutex_unlock(&frames_mutex);

	pthread_mutex_lock(&frames_cache_mutex);
	bool find_cache(void* element) {
		t_cache* cache = element;
		return cache->adm_table->pid == pid;
	}

	t_list* cache_tables = list_filter(cache_list, &find_cache);

	if(cache_tables == NULL)
		return;

	for (i = 0; i < list_size(cache_tables); i++) {
		t_cache* cache = list_get(cache_tables, i);
		cache->adm_table->pid = -1;
		cache->adm_table->pag = 0;
		cache->adm_table->frame = 0;
		cache->lru = -1;
		update_administrative_register_cache(cache, i);
	}
	pthread_mutex_unlock(&frames_cache_mutex);

	log_debug(logger, "finish_program, pid = %d\n", pid);
	list_destroy(adm_tables);
	list_destroy(cache_tables);
}

void add_pages(int pid, int n_frames) {
	log_debug(logger, "add_pages, pid = '%d', n_frames = '%d'\n", pid, n_frames);
	pthread_mutex_lock(&frames_mutex);
	bool find(void* element) {
		t_adm_table* adm_table = element;
		return adm_table->pid == pid;
	}

	int i;

	t_list* know_pages_list = list_filter(adm_list, &find);
	int known_pages = list_size(know_pages_list);
	list_destroy(know_pages_list);

	// HASH
	while (n_frames > 0) {
		int frame_pos = hash(pid, known_pages);
		t_adm_table* adm_table = list_get(adm_list, frame_pos);
		log_debug(logger, "HASH ADD(%d, %d) = %d | PID = %d, PAG = %d", pid, known_pages, frame_pos,  adm_table->pid, adm_table->pag);
		log_debug(logger, "%s", (adm_table->pid < 0) ? "EUREKA!" : "SIGA PARTICIPANDO.");
		if (adm_table->pid < 0) {
			adm_table->pid = pid;
			adm_table->frame = frame_pos;
			adm_table->pag = known_pages++;
			update_administrative_register_adm_table(adm_table);
			if (--n_frames <= 0)
				break;
		} else
			break;
	}
	// HASH

	for (i = 0; i < list_size(adm_list)&&(n_frames>0); i++) {
		t_adm_table* adm_table = list_get(adm_list, i);
		if (adm_table->pid < 0) {
			adm_table->pid = pid;
			adm_table->pag = known_pages++;
			update_administrative_register_adm_table(adm_table);
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
		update_administrative_register_cache(cache, i);
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

int find_cache_victim(int pid) {
	int i, lru_max = -1, pos;
	t_cache* cache;
	for (i = 0; i < list_size(cache_list); i++) {
		cache = list_get(cache_list, i);
		if (cache->lru > lru_max) {
			lru_max = cache->lru;
			pos = i;
		}
	}

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
	log_debug(logger, "store_in_cache: pid: '%d, page: '%d', frame: '%d'",
			n_adm_table->pid, n_adm_table->pag, n_adm_table->frame);
	pthread_mutex_lock(&frames_cache_mutex);
	bool find(void* element) {
		t_cache* adm_table = element;
		return adm_table->adm_table->pid == n_adm_table->pid;
	}
	t_list* cache_proc_size_list = list_filter(cache_list, &find);
	int cache_proc_size = list_size(cache_proc_size_list);
	list_destroy(cache_proc_size_list);
	log_debug(logger, "store_in_cache: cache_proc_size: '%d', cache_x_proc: '%d'", cache_proc_size, cache_x_proc);
	if (cache_proc_size < cache_x_proc) {
		int free_pos = find_free_pos_cache();
		if (free_pos < 0) {
			free_pos = find_cache_victim(n_adm_table->pid);
		}

		t_cache* n_cache = list_get(cache_list, free_pos);
		if(n_cache != NULL) {
			n_cache->adm_table->frame = n_adm_table->frame;
			n_cache->adm_table->pag = n_adm_table->pag;
			n_cache->adm_table->pid = n_adm_table->pid;
			n_cache->lru = -1;
			update_administrative_register_cache(n_cache, free_pos);
		}
	}
	pthread_mutex_unlock(&frames_cache_mutex);
	increase_cache_lru();
}

char* read_bytes(int pid, int page, int offset, int size) {
	pthread_mutex_lock(&frames_mutex);
	bool find(void* element) {
		t_adm_table* adm_table = element;
		return adm_table->pag == page && adm_table->pid == pid;
	}

	t_adm_table* adm_table;
	bool is_cache = exists_in_cache(pid, page);
	log_debug(logger, "read_bytes: Page: '%d' is_cache: '%s', cache_size: '%d'", page,  is_cache ? "YES" : "NO", cache_size);
	if (is_cache) {
		adm_table = get_from_cache(pid, page);
	} else {
		// HASH
		int frame_pos = hash(pid, page);
		adm_table = list_get(adm_list, frame_pos);
		log_debug(logger, "HASH READ(%d, %d) = %d | PID = %d, PAG = %d", pid, page, frame_pos, adm_table->pid, adm_table->pag);
		log_debug(logger, "%s", (adm_table->pid == pid && adm_table->pag == page) ? "EUREKA!" : "SIGA PARTICIPANDO.");
		if (adm_table->pid != pid || adm_table->pag != page) {
		// HASH
			adm_table = list_find(adm_list, &find);
		}
		log_debug(logger, "read_bytes: mem_delay: '%d'", mem_delay);
		usleep(mem_delay * 1000);
	}

	int start = frame_size * adm_table->frame + offset;
	char* buffer = string_substring(frames, start, size);

	pthread_mutex_unlock(&frames_mutex);

	if (!is_cache && cache_size != 0) {
		store_in_cache(adm_table);
		//store_bytes_cache(pid, page, offset, size, buffer);
	}
	return buffer;
}

int store_bytes(int pid, int page, int offset, int size, char* buffer) {
	pthread_mutex_lock(&frames_mutex);
	bool find(void* element) {
		t_adm_table* adm_table = element;
		return adm_table->pag == page && adm_table->pid == pid;
	}

	t_adm_table* adm_table;
	bool is_cache = exists_in_cache(pid, page);
	log_debug(logger, "store_bytes: Page: '%d' is_cache: '%s', cache_size: '%d'", page,  is_cache ? "YES" : "NO", cache_size);
	if (is_cache) {
		adm_table = get_from_cache(pid, page);
	} else {
		// HASH
		int frame_pos = hash(pid, page);
		adm_table = list_get(adm_list, frame_pos);
		log_debug(logger, "HASH STORE(%d, %d) = %d | PID = %d, PAG = %d", pid, page, frame_pos, adm_table->pid, adm_table->pag);
		log_debug(logger, "%s", (adm_table->pid == pid && adm_table->pag == page) ? "EUREKA!" : "SIGA PARTICIPANDO.");
		if (adm_table->pid != pid || adm_table->pag != page) {
		// HASH
			adm_table = list_find(adm_list, &find);
		}
		log_debug(logger, "store_bytes: mem_delay: '%d'", mem_delay);
		usleep(mem_delay * 1000);
	}

	int start, end;
	if (adm_table != NULL) {
		start = frame_size * adm_table->frame + offset;
		end = start + size;

		int i, b = 0;
		for (i = start; i < end; i++) {
			//if (is_cache)
			//	frames_cache[i] = buffer[b];
			//else
				frames[i] = buffer[b];
			b++;
		}
		pthread_mutex_unlock(&frames_mutex);
		if (!is_cache && cache_size != 0) {
			store_in_cache(adm_table);
			//store_bytes_cache(pid, page, offset, size, buffer);
		}
	}
	return start;
}


void update_administrative_register_adm_table(t_adm_table* adm_table) {
	int size_reg_adm_table = (sizeof(int) + sizeof(int) + sizeof(int));
	int i;

	int offset = size_reg_adm_table * adm_table->frame;
	char* buffer = intToChar4(adm_table->frame);
	for (i = 0; i < sizeof(int); i++)
		frames[offset + i] = buffer[i];

	offset += sizeof(int);
	buffer = intToChar4(adm_table->pid);
	for (i = 0; i < sizeof(int); i++)
		frames[offset + i] = buffer[i];

	offset += sizeof(int);
	buffer = intToChar4(adm_table->pag);
	for (i = 0; i < sizeof(int); i++)
		frames[offset + i] = buffer[i];
	free(buffer);
}

void update_administrative_register_cache(t_cache* cache, int pos_list) {
	int size_reg_cache = (sizeof(int) + sizeof(int) + sizeof(int) + sizeof(int));
	int i;

	int offset = size_reg_cache * pos_list;
	char* buffer = intToChar4(cache->adm_table->frame);
	for (i = 0; i < sizeof(int); i++)
		frames[offset + i] = buffer[i];

	offset += sizeof(int);
	buffer = intToChar4(cache->adm_table->pid);
	for (i = 0; i < sizeof(int); i++)
		frames[offset + i] = buffer[i];

	offset += sizeof(int);
	buffer = intToChar4(cache->adm_table->pag);
	for (i = 0; i < sizeof(int); i++)
		frames[offset + i] = buffer[i];

	offset += sizeof(int);
	buffer = intToChar4(cache->lru);
	for (i = 0; i < sizeof(int); i++)
		frames[offset + i] = buffer[i];
}

void show_administrative_structures_located_in_memory() {
	int j = 0, offset;

	printf("TABLA DE PAGINAS:\n");
	log_debug(logger, "TABLA DE PAGINAS:\n");

	for (offset = 0; offset < bytes_reserved_for_page_table; offset +=
			sizeof(int)) {
		char* buffer = string_substring(frames, offset, sizeof(int));
		j++;

		if (j == 1) {
			printf("> FRAME: %d | ", char4ToInt(buffer));
			log_debug(logger, "> FRAME: %d", char4ToInt(buffer));
		}
		if (j == 2) {
			printf("PID: %d | ", char4ToInt(buffer));
			log_debug(logger, "> PID: %d", char4ToInt(buffer));
		}
		if (j == 3) {
			printf("PAGE: %d\n", char4ToInt(buffer));
			log_debug(logger, "> PAGE: %d\n", char4ToInt(buffer));
			j = 0;
		}
		free(buffer);
	}

	j = 0;

	printf("\nCACHE:\n");
	log_debug(logger, "CACHE:\n");

	int total_bytes = bytes_reserved_for_cache + bytes_reserved_for_page_table;
	for (; offset < total_bytes; offset += sizeof(int)) {
		char* buffer = string_substring(frames, offset, sizeof(int));
		j++;

		if (j == 1) {
			printf("> FRAME: %d | ", char4ToInt(buffer));
			log_debug(logger, "> FRAME: %d", char4ToInt(buffer));
		}
		if (j == 2) {
			printf("PID: %d | ", char4ToInt(buffer));
			log_debug(logger, "> PID: %d", char4ToInt(buffer));
		}
		if (j == 3) {
			printf("PAGE: %d | ", char4ToInt(buffer));
			log_debug(logger, "> PAGE: %d", char4ToInt(buffer));
		}
		if (j == 4) {
			printf("LRU: %d\n", char4ToInt(buffer));
			log_debug(logger, "> LRU: %d\n", char4ToInt(buffer));
			j = 0;
		}
		free(buffer);
	}
}

void store_administrative_structures() {
	pages_reserved_for_admin = 0;
	bytes_reserved_for_page_table = 0;
	bytes_reserved_for_cache = 0;

	int size_reg_adm_table = (sizeof(int) + sizeof(int) + sizeof(int));
//(sizeof(nroFrame) + sizeof(nroDePID) + sizeof(nroDePagina)) * cant_frames
	bytes_reserved_for_page_table += size_reg_adm_table * frames_count;
//(sizeof(nroFrame) + sizeof(nroDePID) + sizeof(nroDePagina)) + sizeof(lru) * cache_size
	int size_reg_cache = (sizeof(int) + sizeof(int) + sizeof(int) + sizeof(int));

	bytes_reserved_for_cache += size_reg_cache * cache_size;
	pages_reserved_for_admin = bytes_reserved_for_cache
			+ bytes_reserved_for_page_table;

	pages_reserved_for_admin = ceil(
			(float) pages_reserved_for_admin / frame_size);

	log_debug(logger, "pages_reserved_for_admin: '%d'",
			pages_reserved_for_admin);

	int i, page_c = pages_reserved_for_admin, offset = 0;
	char* buffer = string_new();

	for (i = 0; i < list_size(adm_list); i++) {
		t_adm_table* adm_table = list_get(adm_list, i);
		if (adm_table->pid < 0 && page_c > 0) {
			adm_table->pid = PID_ADM_STRUCT;
			adm_table->frame = i;
			adm_table->pag = pages_reserved_for_admin - page_c;
			page_c--;
		} else {
			adm_table->pag = 0;
		}
	}

//ESTRUCTURA TABLA DE PAGINAS
	int j;
	for (i = 0; i < list_size(adm_list); i++) {
		t_adm_table* adm_table = list_get(adm_list, i);

		buffer = intToChar4(adm_table->frame);
		for (j = 0; j < sizeof(int); j++)
			frames[offset + j] = buffer[j];

		offset += sizeof(int);

		buffer = intToChar4(adm_table->pid);
		for (j = 0; j < sizeof(int); j++)
			frames[offset + j] = buffer[j];

		offset += sizeof(int);

		buffer = intToChar4(adm_table->pag);
		for (j = 0; j < sizeof(int); j++)
			frames[offset + j] = buffer[j];

		offset += sizeof(int);
	}

//ESTRUCTURA CACHE
	for (i = 0; i < list_size(cache_list); i++) {
		t_cache* cache_table = list_get(cache_list, i);

		buffer = intToChar4(cache_table->adm_table->frame);
		for (j = 0; j < sizeof(int); j++)
			frames[offset + j] = buffer[j];

		offset += sizeof(int);

		buffer = intToChar4(cache_table->adm_table->pid);
		for (j = 0; j < sizeof(int); j++)
			frames[offset + j] = buffer[j];

		offset += sizeof(int);

		buffer = intToChar4(cache_table->adm_table->pag);
		for (j = 0; j < sizeof(int); j++)
			frames[offset + j] = buffer[j];

		offset += sizeof(int);

		buffer = intToChar4(cache_table->lru);
		for (j = 0; j < sizeof(int); j++)
			frames[offset + j] = buffer[j];

		offset += sizeof(int);
	}
	free(buffer);
}

void init_memory(t_config *config) {
	int port = config_get_int_value(config, PUERTO);

	pthread_mutex_init(&frames_mutex, NULL);
	pthread_mutex_init(&frames_cache_mutex, NULL);
	pthread_mutex_init(&cpu_sockets_mutex, NULL);
	pthread_mutex_init(&mem_mutex, NULL);

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
		adm_table->frame = 0;
		adm_table->pag = 0;
		adm_table->pid = -1;

		cache->adm_table = adm_table;
		cache->lru = -1;

		list_add(cache_list, cache);
	}

	store_administrative_structures();

	if ((m_sockets.k_socket = createListen(port, &newClient, fns,
			&connectionClosed,
			NULL) == -1)) {
		log_error(logger, "Error at creating listener at port %d", port);
		error_show(" at creating listener at port %d", port);
		exit(EXIT_FAILURE);
	}
}

void clean_frame(int frame) {
	int start = frame * frame_size;
	int end = start + frame_size;
	int i;
	for (i = start; i < end; i++) {
		frames[i] = '#';
	}
}

void free_page(int pid, int page) {
	int i;

	pthread_mutex_lock(&frames_mutex);
	// HASH
	int frame_pos = hash(pid, page);
	t_adm_table* adm_table = list_get(adm_list, frame_pos);
	log_debug(logger, "HASH FREE(%d, %d) = %d | PID = %d, PAG = %d", pid, page, frame_pos, adm_table->pid, adm_table->pag);
	log_debug(logger, "%s", (adm_table->pid == pid && adm_table->pag == page) ? "EUREKA!" : "SIGA PARTICIPANDO.");
	if (adm_table->pid != pid || adm_table->pag != page) {
	// HASH
		for (i = 0; list_size(adm_list); i++) {
			t_adm_table* adm_table = list_get(adm_list, i);
			if (adm_table->pid == pid && adm_table->pag == page) {
				adm_table->pid = -1;
				adm_table->pag = 0;
				update_administrative_register_adm_table(adm_table);
				clean_frame(adm_table->frame);
				break;
			}
		}
	} else {
		adm_table->pid = -1;
		adm_table->pag = 0;
		update_administrative_register_adm_table(adm_table);
		clean_frame(adm_table->frame);
	}
	pthread_mutex_unlock(&frames_mutex);

	pthread_mutex_lock(&frames_cache_mutex);
	for (i = 0; i < list_size(cache_list); i++) {
		t_cache* cache = list_get(cache_list, i);
		if (cache->adm_table->pid == pid && cache->adm_table->pag == page) {
			cache->adm_table->pid = -1;
			cache->adm_table->frame = 0;
			cache->adm_table->pag = 0;
			cache->lru = -1;
			break;
		}
	}
	pthread_mutex_unlock(&frames_cache_mutex);
}
bool exists_pid(int pid){
	int i;
	for(i=0; i<list_size(adm_list);i++){
		t_adm_table* adm_table = list_get(adm_list, i);
		if (adm_table->pid == pid) return true;
	}
	return false;
}

void dump(int pid) {
	int i;
	if(!exists_pid(pid)) return;

	pthread_mutex_lock(&frames_cache_mutex);
	char* dump_cache = string_new();
	char* dump_cache_struct = string_new();
	char* dump_cache_content = string_from_format("%s", frames_cache);

	for (i = 0; i < list_size(cache_list); i++) {
		t_cache* cache = list_get(cache_list, i);
		if (cache->adm_table->pid == pid || pid == -1) {
			string_append_with_format(&dump_cache_struct,
					"PID: %d | PAGE: %d | LRU COUNTER: %d\n",
					cache->adm_table->pid, cache->adm_table->pag, cache->lru);
		}
	}
	dump_cache = string_from_format("%s\n%s", dump_cache_struct,
			dump_cache_content);
	pthread_mutex_unlock(&frames_cache_mutex);

	pthread_mutex_lock(&frames_mutex);
	char* dump_mem_struct = string_new();
	char* dump_act_process = string_new();
	char* dump_mem_content = string_new();

	if (pid == -1) {
		string_append(&dump_mem_content, frames);
	} else {
		for (i = 0; i < list_size(adm_list); i++) {
			t_adm_table* adm_table = list_get(adm_list, i);
			if (adm_table->pid == pid) {
				char* frame = string_substring(frames,
						adm_table->frame * frame_size, frame_size);
				string_append_with_format(&dump_mem_content,
						"FRAME: %d | PID: %d | PAGE: %d\n%s\n",
						adm_table->frame, adm_table->pid, adm_table->pag,
						frame);
			}
		}
	}

	for (i = 0; i < frames_count; i++) {
		t_adm_table* adm_table = list_get(adm_list, i);
		if (adm_table->pid == pid || pid < 0) {
			string_append_with_format(&dump_mem_struct,
					"FRAME: %d | PID: %d | PAGE: %d\n", adm_table->frame,
					adm_table->pid, adm_table->pag);
		}
		if (adm_table->pid > 0 && adm_table->pag == 0) {
			string_append_with_format(&dump_act_process, "ACTIVE PID: %d\n",
					adm_table->pid);
		}
	}

	char* dump_total = string_new();
	if(cache_size != 0)string_append_with_format(&dump_total, "CACHE:\n%s\n", dump_cache);
	string_append_with_format(&dump_total, "TABLA DE PAGINAS:\n%s\n",
			dump_mem_struct);
	string_append_with_format(&dump_total, "LISTADO DE PROCESOS ACTIVOS:\n%s\n",
			dump_act_process);
	string_append_with_format(&dump_total, "CONTENIDO MEMORIA:\n%s\n\n",
			dump_mem_content);

	pthread_mutex_unlock(&frames_mutex);

	log_debug(logger, "Content dumped: %s\n\n", dump_total);
	clear_screen();
	printf("%s\n", dump_total);

	/*list_clean_and_destroy_elements(active_process, free);
	 list_clean_and_destroy_elements(active_process_no_duplicates, free);*/
	wait_any_key();
	free(dump_act_process);
	free(dump_cache);
	free(dump_cache_content);
	free(dump_cache_struct);
	free(dump_mem_content);
	free(dump_mem_struct);
	free(dump_total);
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
		pthread_mutex_lock(&frames_mutex);
		mem_delay = atoi(retard);
		pthread_mutex_unlock(&frames_mutex);
	}
}

void do_flush(char* sel) {
	if (!strcmp(sel, "F")) {
		pthread_mutex_lock(&frames_cache_mutex);
		int i;
		for (i = 0; i < list_size(cache_list); i++) {
			t_cache* cache = list_get(cache_list, i);
			//save_victim(cache->adm_table);
			cache->adm_table->pid = -1;
			cache->lru = -1;
			update_administrative_register_cache(cache, i);
		}
		pthread_mutex_unlock(&frames_cache_mutex);
	}
}

void size(int option) {
	bool find_free(void* element) {
		t_adm_table* adm_table = element;
		return adm_table->pid < 0;
	}
	bool find_used(void* element) {
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
		t_list* used_c_list = list_filter(adm_list, find_used);
		int used_c = list_size(used_c_list);
		list_destroy(used_c_list);
		t_list* free_c_list = list_filter(adm_list, find_free);
		int free_c = list_size(free_c_list);
		list_destroy(free_c_list);

		string_append_with_format(&size_s, "FRAMES: %d\n", frames_count);
		string_append_with_format(&size_s, "USED: %d\n", used_c);
		string_append_with_format(&size_s, "FREE: %d\n", free_c);
	} else {
		t_list* pid_c_list = list_filter(adm_list, find_pid);
		int pid_c = list_size(pid_c_list);
		list_destroy(pid_c_list);
		string_append_with_format(&size_s, "USED BY [%d]: %d\n", option, pid_c);
	}
	pthread_mutex_unlock(&frames_mutex);
	clear_screen();
	printf("%s", size_s);

	wait_any_key();
	free(size_s);
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

void do_finish(char* sel) {
	if (!strcmp(sel, "F")) {
		list_destroy_and_destroy_elements(adm_list, free);
		list_destroy_and_destroy_elements(cache_list, free);
		exit(EXIT_SUCCESS);
	}
}

void ask_option(char *sel) {
	print_menu();
	fgets(sel, 255, stdin);
	strtok(sel, "\n");
	string_to_upper(sel);
}

int get_page_from_pointer(int pointer) {
	int frame = pointer / frame_size;
	int i;
	for (i = 0; i < list_size(adm_list); i++) {
		t_adm_table* adm_table = list_get(adm_list, i);
		if (adm_table->frame == frame)
			return adm_table->pag;
	}
	return -1;
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
