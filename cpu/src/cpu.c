#include "cpu.h"

const char* CONFIG_FIELDS[] = { IP_KERNEL, PUERTO_KERNEL, IP_MEMORIA, PUERTO_MEMORIA };

/*void connect_to_kernel() {
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
 }*/

//TODO
page_size; //se lo tengo que pedir a Memoria, despues veo como hacerlo

char* ask_next_instruction_memory(){
	t_intructions* index = pcbActual->i_code;

	index += pcbActual->pc;
	t_intructions* instruccion = index;
	int comienzo = instruccion->start;
	int longitud = instruccion->offset;

	// Obtengo la direccion logica de la instruccion a partir del I­ndice de codigo:
	t_retvar* direccionInstruccion = (t_retvar*)malloc(sizeof(t_retvar));

	direccionInstruccion->pag = comienzo / page_size;
	direccionInstruccion->off = comienzo % page_size;
	direccionInstruccion->size = longitud;

	aplicar_protocolo_enviar(fdUMC, PEDIDO_LECTURA_INSTRUCCION, direccionInstruccion);
	free(direccionInstruccion);

	if(recibirYvalidarEstadoDelPedidoAUMC()){
		int head;
		void* entrada = NULL;
		entrada = aplicar_protocolo_recibir(fdUMC, &head);

		if(head == DEVOLVER_INSTRUCCION){
			return (char*) entrada; // UMC aceptÃ³ el pedido y me devuelve la instrucciÃ³n
		}
		else { // retorno null por error en el head
			return NULL;
		}
	} // retorno null porque UMC rechazÃ³ el pedido
	return NULL;
}

void execute_cpu_burst(int quantum){

		while (quantum > 0){
			char* next_instruction = ask_next_instruction_memory();

					if (proximaInstruccion != NULL){ // LlegÃ³ una instrucciÃ³n, analizo si es o no 'end':

						limpiarInstruccion(proximaInstruccion);

						if ( pcbActual->pc >= (pcbActual->cantidad_instrucciones -1)
								&& string_starts_with(proximaInstruccion, "end") ){
							// Es 'end'. Finalizo ejecuciÃ³n por EXIT:
							log_info(logger, "El programa actual ha finalizado con Ã©xito.");
							// Mando solamente el pid, porque al NÃºcleo ya no le sirve el PCB.
							aplicar_protocolo_enviar(fdNucleo, PCB_FIN_EJECUCION, &(pcbActual->pid));
							free(proximaInstruccion); proximaInstruccion = NULL;
							exitProceso();
							return;
						}
						// Si no es 'end'. Ejecuto la prÃ³xima instrucciÃ³n:
						log_info(logger, "InstrucciÃ³n recibida: %s", proximaInstruccion);

						analizadorLinea(proximaInstruccion, &funcionesAnSISOP, &funcionesKernel);

						if (huboStackOverflow){
							log_info(logger, "Se ha producido Stack Overflow. Abortando programa actual.");
							// Mando solamente el pid, porque al NÃºcleo ya no le sirve el PCB.
							aplicar_protocolo_enviar(fdNucleo, ABORTO_PROCESO, &(pcbActual->pid));
							free(proximaInstruccion); proximaInstruccion = NULL;
							exitProceso();
							return;
						}
						if(finalizoPrograma){
							// Finalizo ejecuciÃ³n por EXIT:
							log_info(logger, "El programa actual ha finalizado con Ã©xito.");
							// Mando solamente el pid, porque al NÃºcleo ya no le sirve el PCB.
							aplicar_protocolo_enviar(fdNucleo, PCB_FIN_EJECUCION, &(pcbActual->pid));
							free(proximaInstruccion); proximaInstruccion = NULL;
							exitProceso();
							return;
						}

						quantum--; // Decremento el quantum actual
						(pcbActual->pc)++; // Incremento Program Counter del PCB

						switch (devolvioPcb){
						case POR_IO:{
							log_info(logger, "Expulsando proceso por pedido de I/O.");
							aplicar_protocolo_enviar(fdNucleo, PCB_ENTRADA_SALIDA, pcbActual);
							free(proximaInstruccion); proximaInstruccion = NULL;
							exitProceso();
							return;
						}
						case POR_WAIT:{
							log_info(logger, "Expulsando proceso por operaciÃ³n WAIT bloqueante.");
							aplicar_protocolo_enviar(fdNucleo, PCB_WAIT, pcbActual);
							free(proximaInstruccion); proximaInstruccion = NULL;
							exitProceso();
							return;
						}
					} // fin switch devolviÃ³ PCB

						usleep(pcbActual->quantum_sleep * 1000); // Retardo de quantum

						free(proximaInstruccion); proximaInstruccion = NULL;
					} // fin if not null
					else { // LlegÃ³ instrucciÃ³n null por error o rechazo de UMC:
						exitPorErrorUMC();
						return;
					} // fin else not null
		}
}

void create_function_dictionary() {
	fns = dictionary_create();

	dictionary_put(fns, "memory_identify", &memory_identify);
	dictionary_put(fns, "kernel_execute_program", &kernel_execute_program);

}
void connect_to_server(t_config* config, char* name) {
	int port;
	char* ip_server;
	int* socket;

	if (!strcmp(name, KERNEL)) {
		ip_server = config_get_string_value(config, IP_KERNEL);
		port = config_get_int_value(config, PUERTO_KERNEL);
		socket = &kernel_socket;
	} else if (!strcmp(name, MEMORY)) {
		ip_server = config_get_string_value(config, IP_MEMORIA);
		port = config_get_int_value(config, PUERTO_MEMORIA);
		socket = &mem_socket;
	} else {
		log_error(logger, "Error at connecting to %s.", name);
		error_show(" at connecting to %s.\n", name);
		exit(EXIT_FAILURE);
	}

	if ((*socket = connectServer(ip_server, port, fns, &server_connectionClosed, NULL)) == -1) {
		log_error(logger, "Error at connecting to %s.", name);
		error_show(" at connecting to %s.\n", name);
		exit(EXIT_FAILURE);
	}
	log_debug(logger, "Connected to %s.", name);
}

void init_cpu(t_config* config){
	connect_to_server(config, KERNEL);
	connect_to_server(config, MEMORY);
}

int main(int argc, char *argv[]) {
	clear_screen();
	char sel[255];
	t_config* config = malloc(sizeof(t_config));

	remove("log");
	logger = log_create("log", "CPU", false, LOG_LEVEL_DEBUG);

	create_function_dictionary();
	load_config(&config, argc, argv[1]);
	print_config(config, CONFIG_FIELDS, CONFIG_FIELDS_N);

	init_cpu(config);

	wait_any_key();

	do{


	}while(true);

	return EXIT_SUCCESS; // Fin exitoso
}
