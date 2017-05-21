#include "kernel_interface.h"

char* pcb_n_program_to_memory(pcb n_pcb, char* program) {
	cJSON* root = cJSON_CreateObject();
	cJSON* stack_arr = cJSON_CreateArray();

	cJSON_AddItemToObject(root, "pid", cJSON_CreateNumber(n_pcb.pid));
	cJSON_AddItemToObject(root, "program", cJSON_CreateString(program));

	void it_stack(void* s) {
		t_stack* stack = s;
		cJSON* stack_o = cJSON_CreateObject();

		if (stack->args != NULL) {
			t_arg_var* args = stack->args;
			cJSON* args_o = cJSON_CreateObject();
			cJSON_AddItemToObject(args_o, "id", cJSON_CreateNumber(args->id));
			cJSON_AddItemToObject(args_o, "pag", cJSON_CreateNumber(args->pag));
			cJSON_AddItemToObject(args_o, "off", cJSON_CreateNumber(args->off));
			cJSON_AddItemToObject(args_o, "size", cJSON_CreateNumber(args->size));

			cJSON_AddItemToObject(stack_o, "args", args_o);
		}

		if (stack->vars != NULL) {
			t_list* vars = stack->vars;
			cJSON* vars_arr = cJSON_CreateArray();
			void it_vars(void* v) {
				t_arg_var* var = v;
				cJSON* var_o = cJSON_CreateObject();
				cJSON_AddItemToObject(var_o, "id", cJSON_CreateNumber(var->id));
				cJSON_AddItemToObject(var_o, "pag", cJSON_CreateNumber(var->pag));
				cJSON_AddItemToObject(var_o, "off", cJSON_CreateNumber(var->off));
				cJSON_AddItemToObject(var_o, "size", cJSON_CreateNumber(var->size));

				cJSON_AddItemToArray(vars_arr, var_o);
			}
			list_iterate(vars, it_vars);
			cJSON_AddItemToObject(stack_o, "vars", vars_arr);
		}

		if (stack->retpos != NULL) {
			int retpos = *stack->retpos;
			cJSON_AddItemToObject(stack_o, "retpos", cJSON_CreateNumber(retpos));
		}

		if (stack->retvar != NULL) {
			t_retvar* retvar = stack->retvar;
			cJSON* retvar_o = cJSON_CreateObject();
			cJSON_AddItemToObject(retvar_o, "pag", cJSON_CreateNumber(retvar->pag));
			cJSON_AddItemToObject(retvar_o, "off", cJSON_CreateNumber(retvar->off));
			cJSON_AddItemToObject(retvar_o, "size", cJSON_CreateNumber(retvar->size));

			cJSON_AddItemToObject(stack_o, "retvar", retvar_o);
		}

		cJSON_AddItemToArray(stack_arr, stack_o);
	}

	if (n_pcb.i_stack != NULL) {
		list_iterate(n_pcb.i_stack, it_stack);
		cJSON_AddItemToObject(root, "stack", stack_arr);
	}

	return cJSON_Print(root);
}

/*
 * CONSOLE
 */
void console_load_program(socket_connection* connection, char** args) {
	char* sender = args[0];
	char* program = args[1];

	if (!strcmp(sender, CONSOLE)) {
		pcb* new_pcb = malloc(sizeof(pcb));

		pthread_mutex_lock(&p_counter_mutex);
		new_pcb->pid = ++p_counter;
		pthread_mutex_unlock(&p_counter_mutex);

		new_pcb->page_c = 0;
		new_pcb->state = NEW_LIST;
		new_pcb->i_code = list_create();
		new_pcb->i_label = dictionary_create();
		new_pcb->i_stack = list_create();

		t_stack* stack = malloc(sizeof(t_stack));
		stack->args = NULL;
		stack->vars = NULL;
		stack->retpos = NULL;
		stack->retvar = NULL;
		list_add(new_pcb->i_stack, stack);

		new_pcb->statistics.cycles = 0;
		new_pcb->statistics.op_priviliges = 0;

		set_indices(new_pcb, program);

		pthread_mutex_lock(&pcb_list_mutex);
		queue_push(new_queue, &new_pcb);
		pthread_mutex_unlock(&pcb_list_mutex);

		pthread_mutex_lock(&console_mutex);
		process_struct.socket = connection->socket;
		/*
		 program_struct.socket = connection->socket;
		 program_struct.program = malloc(string_length(program));
		 strcpy(program_struct.program, program);
		 program_struct.pcb = &new_pcb;
		 */

		runFunction(mem_socket, "i_start_program", 1, string_itoa(new_pcb->pid), program);

		/*
		 pthread_attr_t attr;
		 pthread_attr_init(&attr);
		 pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		 pthread_create(&program_struct.thread_info, &attr, &kernel_create_pcb, NULL);
		 */
	}
}

/*
 * MEMORY
 */
void memory_identify(socket_connection* connection, char** args) {
	runFunction(connection->socket, "client_identify", 1, KERNEL);
}
void memory_response_start_program(socket_connection* connection, char** args) {
	int response = atoi(args[0]);

	process_struct.pcb->exit_code = response;

	runFunction(process_struct.socket, "kernel_response_load_program", 2, string_itoa(response), string_itoa(p_counter));
	pthread_mutex_unlock(&console_mutex);
}
void memory_page_size(socket_connection* connection, char** args) {
	int page_size = atoi(args[1]);

	if (!strcmp(args[0], MEMORY))
		mem_page_size = page_size;
}

/*
 * SERVER
 */
void server_connectionClosed(socket_connection* connection) {
	char* client = (connection->socket == fs_socket) ? FILESYSTEM : MEMORY;
	log_debug(logger, "%s has disconnected. Socket = %d, IP = %s, Port = %d.\n", client, connection->socket, connection->ip, connection->port);
	exit(EXIT_FAILURE);
}

/*
 * CLIENT
 */
void newClient(socket_connection* connection) {
	char* client = (connection->port == port_cpu) ? CPU : CONSOLE;

	log_debug(logger, "%s has connected. Socket = %d, IP = %s, Port = %d.\n", client, connection->socket, connection->ip, connection->port);

	if(!strcmp(client, CPU)) {
		t_cpu* cpu = malloc(sizeof(t_cpu));
		cpu->busy = false;
		cpu->socket = connection->socket;
		pthread_mutex_lock(&cpu_mutex);
		list_add(cpu_list, cpu);
		pthread_mutex_unlock(&cpu_mutex);
	}
}
void connectionClosed(socket_connection* connection) {
	char* client = (connection->port == port_cpu) ? CPU : CONSOLE;

	log_debug(logger, "%s has disconnected. Socket = %d, IP = %s, Port = %d.\n", client, connection->socket, connection->ip, connection->port);

	if (strcmp(client, CONSOLE)) {
		pcb* l_pcb = find_pcb_by_socket(connection->socket);
		move_to_list(l_pcb, EXIT_LIST);
		remove_pcb_from_socket_pcb_list(l_pcb);
	} else if(!strcmp(client, CPU)) {
		t_cpu* cpu = find_cpu_by_socket(connection->socket);
		remove_cpu_from_cpu_list(cpu);
	}
}
