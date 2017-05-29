#include "cpu_interface.h"
#include <math.h>


void server_connectionClosed(socket_connection * connection) {
	printf("Server has disconnected. Socket = %d, IP = %s, Port = %d.\n",
			connection->socket, connection->ip, connection->port);
	exit(EXIT_FAILURE);
}

/*
 * MEMORY
 */
char* LINE_CODE;
void memory_identify(socket_connection* connection, char** args) {
	runFunction(connection->socket, "client_identify", 1, CPU);
}
void memory_response_read_bytes_from_page(socket_connection* connection, char** args) {
	 LINE_CODE = args[0];
	 pthread_mutex_unlock(&mx_main);
}

char* conseguirDatosDeLaMemoria(char* programa,
		t_puntero_instruccion inicioDeLaInstruccion, t_size tamanio) {
	char* aRetornar = calloc(1, 100);
	memcpy(aRetornar, programa + inicioDeLaInstruccion, tamanio);
	return aRetornar;
}

/*
 * KERNEL
 */

void kernel_receive_pcb(socket_connection* connection, char** args) {
	char* algoritmo = args[0];

	int quantum;

	if (!strcmp(algoritmo, "RR")) {
		quantum = args[1];
		pcbActual = string_to_pcb(args[2]);

	} else if (!strcmp(algoritmo, "FIFO")) {
		pcbActual = string_to_pcb(args[1]);
	}

	running = true;

	/*char *programa = strdup(codigo);
	t_metadata_program *metadata = metadata_desde_literal(programa);*/

	//t_i_code* ind_code = pcbActual->i_code;
	t_intructions* ind_code = pcbActual->i_code;

	while (!terminoElPrograma()) {
		/*char* const linea = conseguirDatosDeLaMemoria(programa,
				metadata->instrucciones_serializado[pcbActual->pc].start,
				metadata->instrucciones_serializado[pcbActual->pc].offset);*/
		int n_page = ind_code->start/FRAME_SIZE;
		int n_offset = (ind_code->start) - n_page * FRAME_SIZE;
		int n_length = ind_code->offset;


		printf("PID=%d\tPage=%d\tOffset=%d\tLength=%d\n", pcbActual->pid, n_page, n_offset, n_length);
		runFunction(mem_socket, "i_read_bytes_from_page", 4, string_itoa(pcbActual->pid), string_itoa(n_page), string_itoa(n_offset), string_itoa(n_length));
		pthread_mutex_lock(&mx_main);
		pthread_mutex_lock(&mx_main);

		//TODO muestra caca
		printf("%s\n", LINE_CODE);
		analizadorLinea(LINE_CODE, &functions, &kernel_functions);
		pcbActual->pc++;
		ind_code = list_get(pcbActual->i_code, pcbActual->pc);
	}
	//metadata_destruir(metadata);
}
