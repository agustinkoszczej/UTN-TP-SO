#include "memoria_interface.h"

/*
 * INTERFACE
 */
void i_start_program(socket_connection* connection, char** args) {
	pthread_mutex_lock(&mem_mutex);
	int pid = atoi(args[0]);
	char* buffer = args[1];

	int size_buffer = string_length(buffer);
	int n_frames = ceil((float) size_buffer / frame_size);
	log_debug(logger, "i_start_program: pid: '%d', tot_frames: '%d'",pid, n_frames + stack_size);
	if (has_available_frames(n_frames + stack_size)) {
		start_program(pid, n_frames);
		int i;
		for (i = 0; i < n_frames; i++) {
			int size = size_buffer < frame_size ? size_buffer : frame_size;
			char* buffer2 = string_substring(buffer, frame_size * i, size);
			store_bytes(pid, i, 0, size, buffer2);
			free(buffer2);
		}
		char* response = string_itoa(NO_ERRORES);
		runFunction(m_sockets.k_socket, "memory_response_start_program", 1, response);
		free(response);
	} else{
		char* response = string_itoa(NO_SE_PUEDEN_RESERVAR_RECURSOS);
		runFunction(m_sockets.k_socket, "memory_response_start_program", 1, response);
		free(response);
	}
	pthread_mutex_unlock(&mem_mutex);
}
void i_read_bytes_from_page(socket_connection* connection, char** args) {
	//pthread_mutex_lock(&mem_mutex);
	int pid = atoi(args[0]);
	int page = atoi(args[1]);
	int offset = atoi(args[2]);
	int size = atoi(args[3]);
	log_debug(logger, "i_read_bytes_from_page: socket: '%d', pid: '%d', page: '%d', offset: '%d', size: '%d'", connection->socket, pid, page, offset, size);

	char* buffer = read_bytes(pid, page, offset, size);

	runFunction(connection->socket, "memory_response_read_bytes_from_page", 1, buffer);
	log_debug(logger, "memory_response_read_bytes_from_page: socket: '%d', pid: '%d', page: '%d', offset_rel: '%d', size: '%d', buffer: '%s'", connection->socket, pid, page, offset, size, buffer);
	//pthread_mutex_unlock(&mem_mutex);
}
void i_store_bytes_in_page(socket_connection* connection, char** args) {
	//pthread_mutex_lock(&mem_mutex);
	int pid = atoi(args[0]);
	int page = atoi(args[1]);
	int offset = atoi(args[2]);
	int size = atoi(args[3]);
	char* buffer = args[4];

	int offset_abs = store_bytes(pid, page, offset, size, buffer);
	char* response1 = string_itoa(NO_ERRORES);
	char* response2 = string_itoa(offset_abs);
	runFunction(connection->socket, "memory_response_store_bytes_in_page", 2, response1, response2);
	log_debug(logger, "i_store_bytes_in_page: socket: '%d', pid: '%d', page: '%d', offset_rel: '%d', offset_abs: '%d', size: '%d'\nbuffer: '%s'", connection->socket, pid, page, offset, offset_abs, size, buffer);
	free(response1);
	free(response2);
	//pthread_mutex_unlock(&mem_mutex);
}
void i_add_pages_to_program(socket_connection* connection, char** args) {
	pthread_mutex_lock(&mem_mutex);
	int pid = atoi(args[0]);
	int n_page = atoi(args[1]);
	//TODO Habria que ver de agregarla a la cache
	if (has_available_frames(1)) {
		int n_frames = 1;
		pthread_mutex_lock(&frames_mutex);
			int i;
			// HASH
			while (n_frames > 0) {
				int frame_pos = hash(pid, n_page);
				t_adm_table* adm_table = list_get(adm_list, frame_pos);
				log_debug(logger, "HASH HEAP(%d, %d) = %d | PID = %d, PAG = %d", pid, n_page, frame_pos, adm_table->pid, adm_table->pag);
				log_debug(logger, "%s", (adm_table->pid < 0) ? "EUREKA!" : "SIGA PARTICIPANDO.");
				if (adm_table->pid < 0) {
					adm_table->pid = pid;
					adm_table->frame = frame_pos;
					adm_table->pag = n_page;
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
					adm_table->pag = n_page;
					update_administrative_register_adm_table(adm_table);
					if (--n_frames <= 0)
						break;
				}
			}
			pthread_mutex_unlock(&frames_mutex);
		/*pthread_mutex_lock(&frames_mutex);
		bool find(void* element) {
			t_adm_table* adm_table = element;
			return adm_table->pid == pid && adm_table->pag == n_page;
		}
		int i;
		for (i = 0; i < list_size(adm_list); i++) {
			t_adm_table* adm_table = list_get(adm_list, i);
			if (adm_table->pid < 0) {
				adm_table->pid = pid;
				adm_table->pag = n_page;
				update_administrative_register_adm_table(adm_table);
				if (cache_size != 0) {
					//store_in_cache(adm_table);
				}
				break;
			}
		}
		pthread_mutex_unlock(&frames_mutex);
		*/

		char* response = string_itoa(NO_ERRORES);
		runFunction(connection->socket, "memory_response_heap", 1, response);
		free(response);
	} else{
		char* response = string_itoa(NO_SE_PUEDEN_ASIGNAR_MAS_PAGINAS);
		runFunction(connection->socket, "memory_response_heap", 1, response);
		free(response);
	}
	pthread_mutex_unlock(&mem_mutex);
}
void i_finish_program(socket_connection* connection, char** args) {
	pthread_mutex_lock(&mem_mutex);
	int pid = atoi(args[0]);
	finish_program(pid);
	pthread_mutex_unlock(&mem_mutex);
}

void i_free_page(socket_connection* connection, char** args) {
	pthread_mutex_lock(&mem_mutex);
	int pid = atoi(args[0]);
	int page = atoi(args[1]);

	free_page(pid, page);
	char* response = string_itoa(NO_ERRORES);
	runFunction(connection->socket, "memory_response_heap", 1, response);
	free(response);
	pthread_mutex_unlock(&mem_mutex);
}

void kernel_stack_size(socket_connection* connection, char** args) {
	pthread_mutex_lock(&mem_mutex);
	int stack_s = atoi(args[0]);

	stack_size = stack_s;
	pthread_mutex_unlock(&mem_mutex);
}

void i_get_page_from_pointer(socket_connection* connection, char** args) {
	pthread_mutex_lock(&mem_mutex);
	int pointer = atoi(args[0]);

	int page = get_page_from_pointer(pointer);
	log_debug(logger, "i_get_page_from_pointer: '%d'", page);
	char* response = string_itoa(page);
	runFunction(connection->socket, "memory_response_get_page_from_pointer", 1, response);
	free(response);
	pthread_mutex_unlock(&mem_mutex);
}

void i_get_frame_from_pid_and_page(socket_connection* connection, char** args) {
	pthread_mutex_lock(&mem_mutex);

	int pid = atoi(args[0]);
	int page = atoi(args[1]);

	int i, frame;
	for (i = 0; i < list_size(adm_list); i++) {
		t_adm_table* adm_table = list_get(adm_list, i);

		if (adm_table->pid == pid && adm_table->pag == page) {
			frame = adm_table->frame;
			break;
		}
	}

	//send_dynamic_message(connection->socket, string_itoa(frame));
	char* response = string_itoa(frame);
	runFunction(connection->socket, "memory_response_get_frame_from_pid_and_page", 1, response);
	log_debug(logger, "i_get_frame_from_pid_and_page: '%d'", frame);
	free(response);
	pthread_mutex_unlock(&mem_mutex);
}

/*
 * SERVER
 */
void client_identify(socket_connection* connection, char** args) {
	pthread_mutex_lock(&mem_mutex);
	char* sender = args[0];

	log_debug(logger, "%s has connected. Socket = %d, IP = %s, Port = %d.\n", args[0], connection->socket, connection->ip, connection->port);

	if (!strcmp(sender, CPU)) {
		pthread_mutex_lock(&cpu_sockets_mutex);
		list_add(m_sockets.cpu_sockets, &connection->socket);
		pthread_mutex_unlock(&cpu_sockets_mutex);
		char* response = string_itoa(mem_delay);
		runFunction(connection->socket, "memory_retard", 1, response);
		free(response);
	} else if (!strcmp(sender, KERNEL)) {
		m_sockets.k_socket = connection->socket;
		char* response = string_itoa(frame_size);
		runFunction(connection->socket, "memory_page_size", 1, response);
		free(response);
	}
	pthread_mutex_unlock(&mem_mutex);
}

/*
 * CLIENT
 */
void newClient(socket_connection* connection) {
	pthread_mutex_lock(&mem_mutex);
	runFunction(connection->socket, "memory_identify", 0);
	pthread_mutex_unlock(&mem_mutex);
}
void connectionClosed(socket_connection* connection) {
	pthread_mutex_lock(&mem_mutex);
	char* client = (connection->socket == m_sockets.k_socket) ? KERNEL : CPU;
	log_debug(logger, "%s has disconnected. Socket = %d, IP = %s, Port = %d.\n", client, connection->socket, connection->ip, connection->port);

	if (!strcmp(client, CPU)) {
		pthread_mutex_lock(&cpu_sockets_mutex);
		int i;
		for (i = 0; i < list_size(m_sockets.cpu_sockets); i++) {
			int _socket = (int) list_get(m_sockets.cpu_sockets, i);
			if (_socket == connection->socket)
				list_remove(m_sockets.cpu_sockets, i);
		}
		pthread_mutex_unlock(&cpu_sockets_mutex);
	}
	pthread_mutex_unlock(&mem_mutex);
}

