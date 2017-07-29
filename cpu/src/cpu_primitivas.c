/*
 * cpu_primitivas.c
 *
 *  Created on: 25/5/2017
 *      Author: utnso
 */

#include "cpu_primitivas.h"

/*
 * DEFINIR VARIABLE
 *
 * Reserva en el Contexto de Ejecución Actual el espacio necesario para una variable llamada
 * identificador_variable y la registra tanto en el Stack como en el Diccionario de Variables.
 * Retornando la posición del valor de esta nueva variable del stack
 * El valor de la variable queda indefinido: no deberá inicializarlo con ningún valor default.
 * Esta función se invoca una vez por variable, a pesar que este varias veces en una línea.
 * Ej: Evaluar "variables a, b, c" llamará tres veces a esta función con los parámetros "a", "b" y "c"
 *
 * @sintax	TEXT_VARIABLE (variables)
 * 			-- nota: Al menos un identificador; separado por comas
 * @param	identificador_variable	Nombre de variable a definir
 * @return	Puntero a la variable recien asignada
 */
t_puntero cpu_definirVariable(t_nombre_variable identificador_variable) {
	bool is_digit = isdigit(identificador_variable);

	t_stack* stack = list_get(pcb_actual->i_stack, list_size(pcb_actual->i_stack) - 1);

	if (stack->vars == NULL)
		stack->vars = list_create();

	if (stack->args == NULL)
		stack->args = list_create();

	int vars_c = vars_in_stack();

	int occupied_bytes = vars_c * sizeof(int);
	int relative_page = occupied_bytes / frame_size;

	if (relative_page >= stack_size) {
		log_debug(logger, "Violación de segmento ('core' generado)");
		pcb_actual->exit_code = STACK_OVERFLOW;
		cpu_finalizar();
		return -1;
	}

	t_arg_var* n_var = malloc(sizeof(t_arg_var));
	n_var->id = identificador_variable;
	n_var->pag = pcb_actual->page_c + relative_page;
	n_var->off = occupied_bytes - relative_page * frame_size;
	n_var->size = sizeof(int);

	if (!is_digit)
		list_add(stack->vars, n_var);
	else
		list_add(stack->args, n_var);

	runFunction(mem_socket, "i_get_frame_from_pid_and_page", 2, string_itoa(pcb_actual->pid), string_itoa(n_var->pag));
	//int frame = atoi(receive_dynamic_message(mem_socket));
	wait_response(&planning_mutex);

	int offset_abs = (frame_from_pid_and_page * frame_size) + n_var->off;

	log_debug(logger, "|PRIMITIVA| Definir variable '%c' en '%d'", identificador_variable, offset_abs);
	return (t_puntero) offset_abs;
}

/*
 * OBTENER POSICION de una VARIABLE
 *
 * Devuelve el desplazamiento respecto al inicio del segmento Stack en que se encuentra el valor de la variable identificador_variable del contexto actual.
 * En caso de error, retorna -1.
 *
 * @sintax	TEXT_REFERENCE_OP (&)
 * @param	identificador_variable 		Nombre de la variable a buscar (De ser un parametro, se invocara sin el '$')
 * @return	Donde se encuentre la variable buscada
 */
t_puntero cpu_obtenerPosicionVariable(t_nombre_variable identificador_variable) {
	bool is_digit = isdigit(identificador_variable);

	int space_occupied = 0, offset_abs = -1;
	int i, j;
	for (i = 0; i < list_size(pcb_actual->i_stack); i++) {
		t_stack* stack = list_get(pcb_actual->i_stack, i);
		t_list* vars_args_list = is_digit ? stack->args : stack->vars;

		for (j = 0; j < list_size(vars_args_list); j++) {
			t_arg_var* var = list_get(vars_args_list, j);

			if (var->id == identificador_variable && i == list_size(pcb_actual->i_stack) - 1) {
				int relative_page = space_occupied / frame_size;
				int n_offset = space_occupied - relative_page * frame_size;
				int n_page = pcb_actual->page_c + relative_page;

				runFunction(mem_socket, "i_get_frame_from_pid_and_page", 2, string_itoa(pcb_actual->pid), string_itoa(n_page));
				wait_response(&planning_mutex);

				offset_abs = frame_from_pid_and_page * frame_size + n_offset;
				log_debug(logger, "|PRIMITIVA| Obtener Posicion Variable '%c' en '%d'", identificador_variable, offset_abs);
				return offset_abs;
			} else
				space_occupied += sizeof(int);
		}
	}

	log_debug(logger, "|PRIMITIVA| Obtener Posicion Variable: No existe la variable '%c'", identificador_variable);
	pcb_actual->exit_code = ERROR_SIN_DEFINIR;
	cpu_finalizar();

	return offset_abs;
}

/*
 *
 * DEREFERENCIAR
 *
 * Obtiene el valor resultante de leer a partir de direccion_variable, sin importar cual fuera el contexto actual
 *
 * @sintax	TEXT_DEREFERENCE_OP (*)
 * @param	direccion_variable	Lugar donde buscar
 * @return	Valor que se encuentra en esa posicion
 */
t_valor_variable cpu_dereferenciar(t_puntero direccion_variable) {
	runFunction(mem_socket, "i_get_page_from_pointer", 1, string_itoa(direccion_variable));
	wait_response(&planning_mutex);

	int n_page = page_from_pointer;
	int n_offset = direccion_variable % frame_size;

	char* pid = string_itoa(pcb_actual->pid);
	char* page = string_itoa(n_page);
	char* offset = string_itoa(n_offset);
	char* size = string_itoa(sizeof(int));

	runFunction(mem_socket, "i_read_bytes_from_page", 4, pid, page, offset, size);
	wait_response(&planning_mutex);

	log_debug(logger, "|PRIMITIVA| Dereferenciar en '%d', page: '%s', offset: '%s', value: '%d', ", direccion_variable, page, offset, mem_value);
	return (t_valor_variable) mem_value;
}

/*
 * ASIGNAR
 *
 * Inserta una copia del valor en la variable ubicada en direccion_variable.
 *
 * @sintax	TEXT_ASSIGNATION (=)
 * @param	direccion_variable	lugar donde insertar el valor
 * @param	valor	Valor a insertar
 * @return	void
 */

void cpu_asignar(t_puntero direccion_variable, t_valor_variable valor) {
	runFunction(mem_socket, "i_get_page_from_pointer", 1, string_itoa(direccion_variable));
	wait_response(&planning_mutex);

	int n_frame = direccion_variable / frame_size;
	int n_page = page_from_pointer;
	int n_offset = direccion_variable - n_frame * frame_size;

	char* pid = string_itoa(pcb_actual->pid);
	char* page = string_itoa(n_page);
	char* offset = string_itoa(n_offset);
	char* size = string_itoa(sizeof(int));
	char* buffer = intToChar4(valor);
	int i;
	for(i=0; i< sizeof(int); i++){
		if(buffer[i] == '\0'){
			buffer[i] = '#';
		}
	}
	buffer[i] = '\0';
	//char* size = string_itoa(string_length(buffer));

	runFunction(mem_socket, "i_store_bytes_in_page", 5, pid, page, offset, size, buffer);
	wait_response(&planning_mutex);
	log_debug(logger, "|PRIMITIVA| Asignar '%d' en offset_abs: '%d', offset_rel: '%s'", valor, direccion_variable, offset);
}

/*
 * OBTENER VALOR de una variable COMPARTIDA
 *
 * Pide al kernel el valor (copia, no puntero) de la variable compartida por parametro.
 *
 * @sintax	TEXT_VAR_START_GLOBAL (!)
 * @param	variable	Nombre de la variable compartida a buscar
 * @return	El valor de la variable compartida
 */
t_valor_variable cpu_obtenerValorCompartida(t_nombre_compartida variable) {
	runFunction(kernel_socket, "cpu_get_shared_var", 1, variable);
	kernel_shared_var = atoi(receive_dynamic_message(kernel_socket));

	log_debug(logger, "|PRIMITIVA| Obtener Valor Compartida '%d'", kernel_shared_var);

	return kernel_shared_var;
}

/*
 * ASIGNAR VALOR a variable COMPARTIDA
 *
 * Pide al kernel asignar el valor a la variable compartida.
 * Devuelve el valor asignado.
 *
 * @sintax	TEXT_VAR_START_GLOBAL (!) IDENTIFICADOR TEXT_ASSIGNATION (=) EXPRESION
 * @param	variable	Nombre (sin el '!') de la variable a pedir
 * @param	valor	Valor que se le quire asignar
 * @return	Valor que se asigno
 */
t_valor_variable cpu_asignarValorCompartida(t_nombre_compartida variable, t_valor_variable valor) {
	runFunction(kernel_socket, "cpu_set_shared_var", 2, variable, string_itoa(valor));
	receive_dynamic_message(kernel_socket);

	log_debug(logger, "|PRIMITIVA| Asignar Valor Compartida '%d'", valor);

	return valor;
}

/*
 * IR a la ETIQUETA
 *
 * Cambia la linea de ejecucion a la correspondiente de la etiqueta buscada.
 *
 * @sintax	TEXT_GOTO (goto)
 * @param	t_nombre_etiqueta	Nombre de la etiqueta
 * @return	void
 */
void cpu_irAlLabel(t_nombre_etiqueta t_nombre_etiqueta) {
	if (dictionary_has_key(pcb_actual->i_label, t_nombre_etiqueta)) {
		int* pos_label = dictionary_get(pcb_actual->i_label, t_nombre_etiqueta);
		pcb_actual->pc = *pos_label - 1;
	} else {
		pcb_actual->exit_code = ERROR_SIN_DEFINIR;
		cpu_finalizar();
	}

	log_debug(logger, "|PRIMITIVA| Ir a Label '%s'", t_nombre_etiqueta);
}

/*
 * LLAMAR SIN RETORNO
 *
 * Preserva el contexto de ejecución actual para poder retornar luego al mismo.
 * Modifica las estructuras correspondientes para mostrar un nuevo contexto vacío.
 *
 * Los parámetros serán definidos luego de esta instrucción de la misma manera que una
 * variable local, con identificadores numéricos empezando por el 0.
 *
 * @sintax	Sin sintaxis particular, se invoca cuando no coresponde a ninguna de las otras reglas sintacticas
 * @param	etiqueta	Nombre de la funcion
 * @return	void
 */
void cpu_llamarSinRetorno(t_nombre_etiqueta etiqueta) {
	log_debug(logger, "|PRIMITIVA| Llamar sin retorno");
	/*t_stack* stack_aux = stack_create();
	 stack_aux->retpos = malloc(sizeof(int));
	 *stack_aux->retpos = pcb_actual->pc;
	 list_add(pcb_actual->i_stack, stack_aux);

	 cpu_irAlLabel(etiqueta);*/
	pcb_actual->exit_code = ERROR_SINTAXIS;

	cpu_finalizar();
}

/*
 * LLAMAR CON RETORNO
 *
 * Preserva el contexto de ejecución actual para poder retornar luego al mismo,
 * junto con la posicion de la variable entregada por donde_retornar.
 * Modifica las estructuras correspondientes para mostrar un nuevo contexto vacío.
 *
 * Los parámetros serán definidos luego de esta instrucción de la misma manera que una
 * variable local, con identificadores numéricos empezando por el 0.
 *
 * @sintax	TEXT_CALL (<-)
 * @param	etiqueta	Nombre de la funcion
 * @param	donde_retornar	Posicion donde insertar el valor de retorno
 * @return	void
 */
void cpu_llamarConRetorno(t_nombre_etiqueta etiqueta, t_puntero donde_retornar) {
	log_debug(logger, "|PRIMITIVA| Llamar con retorno '%s', '%d'", etiqueta, donde_retornar);

	runFunction(mem_socket, "i_get_page_from_pointer", 1, string_itoa(donde_retornar));
	wait_response(&planning_mutex);

	t_stack* n_stack = malloc(sizeof(t_stack));

	n_stack->retvar = malloc(sizeof(t_retvar));

	n_stack->retvar->pag = page_from_pointer;
	n_stack->retvar->off = donde_retornar % frame_size;
	n_stack->retvar->size = sizeof(int);

	n_stack->retpos = malloc(sizeof(int));
	*n_stack->retpos = pcb_actual->pc;

	n_stack->args = NULL;
	n_stack->vars = NULL;

	list_add(pcb_actual->i_stack, n_stack);

	int* n_pos = dictionary_get(pcb_actual->i_label, etiqueta);
	pcb_actual->pc = *n_pos - 1;
}

/*
 * FINALIZAR
 *
 * Cambia el Contexto de Ejecución Actual para volver al Contexto anterior al que se
 * está ejecutando, recuperando el Cursor de Contexto Actual y el Program Counter
 * previamente apilados en el Stack.
 * En caso de estar finalizando el Contexto principal (el ubicado al inicio del Stack),
 * deberá finalizar la ejecución del programa.
 *
 * @sintax	TEXT_END (end)
 * @param	void
 * @return	void
 */
void cpu_finalizar(void) {
	finished = true;
	log_debug(logger, "|PRIMITIVA| Finalizar Programa");
}

/*
 * RETORNAR
 *
 * Cambia el Contexto de Ejecución Actual para volver al Contexto anterior
 * al que se está ejecutando, recuperando el Cursor de Contexto Actual, el Program Counter
 * y la direccion donde retornar, asignando el valor de retorno en esta, previamente
 * apilados en el Stack.
 *
 * @sintax	TEXT_RETURN (return)
 * @param	retorno	Valor a ingresar en la posicion corespondiente
 * @return	void,
 */
void cpu_retornar(t_valor_variable retorno) {
	log_debug(logger, "|PRIMITIVA| Retornar '%d'", retorno);

	t_stack* last_stack = list_get(pcb_actual->i_stack, pcb_actual->i_stack->elements_count - 1);
	list_remove(pcb_actual->i_stack, pcb_actual->i_stack->elements_count - 1);

	pcb_actual->pc = *last_stack->retpos;

	char* pid = string_itoa(pcb_actual->pid);
	char* page = string_itoa(last_stack->retvar->pag);
	char* offset = string_itoa(last_stack->retvar->off);
	char* size = string_itoa(last_stack->retvar->size);
	char* buffer = intToChar4(retorno);

	runFunction(mem_socket, "i_store_bytes_in_page", 5, pid, page, offset, size, buffer);
	wait_response(&planning_mutex);

	free(last_stack);
}
/*
 * WAIT
 *
 * Informa al kernel que ejecute la función wait para el semáforo con el nombre identificador_semaforo.
 * El kernel deberá decidir si bloquearlo o no.
 *
 * @sintax	TEXT_WAIT (wait)
 * @param	identificador_semaforo	Semaforo a aplicar WAIT
 * @return	void
 */
void kernel_wait(t_nombre_semaforo identificador_semaforo) {
	pcb_actual->statistics.op_priviliges++;

	log_debug(logger, "|PRIMITIVA| Wait:"
			" Semaforo \"%s\"", identificador_semaforo);
	runFunction(kernel_socket, "cpu_wait_sem", 1, identificador_semaforo);
	is_locked = atoi(receive_dynamic_message(kernel_socket));

	log_debug(logger, "|PRIMITIVA| Wait: IS_LOCKED %s", is_locked ? "true" : "false");

	if (is_locked)
		cpu_finalizar();
}
/*
 * SIGNAL
 *
 * Informa al kernel que ejecute la función signal para el semáforo con el nombre identificador_semaforo.
 * El kernel deberá decidir si desbloquear otros procesos o no.
 *
 * @sintax	TEXT_SIGNAL (signal)
 * @param	identificador_semaforo	Semaforo a aplicar SIGNAL
 * @return	void
 */
void kernel_signal(t_nombre_semaforo identificador_semaforo) {
	pcb_actual->statistics.op_priviliges++;

	log_debug(logger, "|PRIMITIVA| Signal: Semaforo \"%s\"", identificador_semaforo);
	runFunction(kernel_socket, "cpu_signal_sem", 1, identificador_semaforo);
	receive_dynamic_message(kernel_socket);
}

/*
 * RESERVAR MEMORIA
 *
 * Informa al kernel que reserve en el Heap una cantidad de memoria
 * acorde al espacio recibido como parametro.
 *
 * @sintax	TEXT_MALLOC (alocar)
 * @param	valor_variable Cantidad de espacio
 * @return	puntero a donde esta reservada la memoria
 */
t_puntero kernel_reservar(t_valor_variable espacio) {
	pcb_actual->statistics.op_priviliges++;

	runFunction(kernel_socket, "cpu_malloc", 2, string_itoa(espacio), string_itoa(pcb_actual->pid));

	int malloc_pointer = atoi(receive_dynamic_message(kernel_socket));

	log_debug(logger, "|PRIMITIVA| Reservar '%d' bytes en '%d'", espacio, malloc_pointer);
	if (malloc_pointer < 0) {
		pcb_actual->exit_code = malloc_pointer;
		cpu_finalizar();
	}

	return (t_puntero) malloc_pointer;
}

/*
 * LIBERAR MEMORIA
 *
 * Informa al kernel que libera la memoria previamente reservada con RESERVAR.
 * Solo se podra liberar memoria previamente asignada con RESERVAR.
 *
 * @sintax	TEXT_FREE (liberar)
 * @param	puntero Inicio de espacio de memoria a liberar (previamente retornado por RESERVAR)
 * @return	void
 */
void kernel_liberar(t_puntero puntero) {
	pcb_actual->statistics.op_priviliges++;

	runFunction(kernel_socket, "cpu_free", 2, string_itoa(puntero), string_itoa(pcb_actual->pid));
	receive_dynamic_message(kernel_socket);
	log_debug(logger, "|PRIMITIVA| Liberar en '%d'", puntero);
}

/*
 * ABRIR ARCHIVO
 *
 * Informa al Kernel que el proceso requiere que se abra un archivo.
 *
 * @syntax 	TEXT_OPEN_FILE (abrir)
 * @param	direccion		Ruta al archivo a abrir
 * @param	banderas		String que contiene los permisos con los que se abre el archivo
 * @return	El valor del descriptor de archivo abierto por el sistema
 */
t_descriptor_archivo kernel_abrir(t_direccion_archivo direccion, t_banderas flags) {
	pcb_actual->statistics.op_priviliges++;

	char* n_flag = get_flag(flags);

	runFunction(kernel_socket, "cpu_validate_file", 2, direccion, string_itoa(pcb_actual->pid));
	bool validate_file = atoi(receive_dynamic_message(kernel_socket));

	if (!validate_file && !flags.creacion) {
		pcb_actual->exit_code = NO_EXISTE_ARCHIVO;
		cpu_finalizar();
		return NO_EXISTE_ARCHIVO;
	}

	runFunction(kernel_socket, "cpu_open_file", 4, direccion, n_flag, string_itoa(validate_file), string_itoa(pcb_actual->pid));

	int kernel_fd = atoi(receive_dynamic_message(kernel_socket));

	if (kernel_fd < 0) {
		pcb_actual->exit_code = kernel_fd;
		cpu_finalizar();
	}

	log_debug(logger, "|PRIMITIVA| Abrir en FD '%d'", kernel_fd);
	return (t_descriptor_archivo) kernel_fd;
}

/*
 * BORRAR ARCHIVO
 *
 * Informa al Kernel que el proceso requiere que se borre un archivo.
 *
 * @syntax 	TEXT_DELETE_FILE (borrar)
 * @param	descriptor_archivo		Descriptor de archivo del archivo a borrar
 * @return	void
 */
void kernel_borrar(t_descriptor_archivo descriptor_archivo) {
	pcb_actual->statistics.op_priviliges++;

	log_debug(logger, "|PRIMITIVA| Borrar");
	runFunction(kernel_socket, "cpu_delete_file", 2, string_itoa(descriptor_archivo), string_itoa(pcb_actual->pid));

	//wait_response();

	int kernel_fd = atoi(receive_dynamic_message(kernel_socket));

	if (kernel_fd < 0) {
		pcb_actual->exit_code = kernel_fd;
		cpu_finalizar();
	}
}

/*
 * CERRAR ARCHIVO
 *
 * Informa al Kernel que el proceso requiere que se cierre un archivo.
 *
 * @syntax 	TEXT_CLOSE_FILE (cerrar)
 * @param	descriptor_archivo		Descriptor de archivo del archivo abierto
 * @return	void
 */
void kernel_cerrar(t_descriptor_archivo descriptor_archivo) {
	pcb_actual->statistics.op_priviliges++;

	log_debug(logger, "|PRIMITIVA| Cerrar");
	runFunction(kernel_socket, "cpu_close_file", 2, string_itoa(descriptor_archivo), string_itoa(pcb_actual->pid));
	//wait_response();

	int kernel_fd = atoi(receive_dynamic_message(kernel_socket));

	if (kernel_fd < 0) {
		pcb_actual->exit_code = kernel_fd;
		cpu_finalizar();
	}
}

/*
 * MOVER CURSOR DE ARCHIVO
 *
 * Informa al Kernel que el proceso requiere que se mueva el cursor a la posicion indicada.
 *
 * @syntax 	TEXT_SEEK_FILE (buscar)
 * @param	descriptor_archivo		Descriptor de archivo del archivo abierto
 * @param	posicion			Posicion a donde mover el cursor
 * @return	void
 */
void kernel_moverCursor(t_descriptor_archivo descriptor_archivo, t_valor_variable posicion) {
	pcb_actual->statistics.op_priviliges++;

	log_debug(logger, "CPU Mover Cursor");
	runFunction(kernel_socket, "cpu_seek_file", 3, string_itoa(descriptor_archivo), string_itoa(posicion), string_itoa(pcb_actual->pid));
	//wait_response();
	int kernel_fd = atoi(receive_dynamic_message(kernel_socket));

	if (kernel_fd < 0) {
		pcb_actual->exit_code = kernel_fd;
		cpu_finalizar();
	}
}

/*
 *
 ESCRIBIR ARCHIVO
 *
 * Informa al Kernel que el proceso requiere que se escriba un archivo previamente abierto.
 * El mismo escribira "tamanio" de bytes de "informacion" luego del cursor
 * No es necesario mover el cursor luego de esta operación
 *
 * @syntax 	TEXT_WRITE_FILE (escribir)
 * @param	descriptor_archivo		Descriptor de archivo del archivo abierto
 * @param	informacion			Informacion a ser escrita
 * @param	tamanio				Tamanio de la informacion a enviar
 * @return	void
 */
void kernel_escribir(t_descriptor_archivo descriptor_archivo, void* informacion, t_valor_variable tamanio) {

	char* buffer = string_new();
	//memcpy(buffer, informacion, tamanio);
	//if (descriptor_archivo == 1){
		//memcpy(buffer, informacion, tamanio);
	//}else{
		string_append(&buffer, string_substring_until((char*) informacion, tamanio));
	//}

	char* fd = string_itoa(descriptor_archivo);
	char* size = string_itoa(tamanio);
	char* pid = string_itoa(pcb_actual->pid);

	pcb_actual->statistics.op_priviliges++;


	//string_append(&buffer, "HOLA");

	//log_debug(logger, "|PRIMITIVA| Escribir FD: '%d', Info: '%s', Tamanio: '%d'", descriptor_archivo, buffer, tamanio);
	runFunction(kernel_socket, "cpu_write_file", 4, fd, buffer, size, pid);


	//wait_response();
	int kernel_fd = atoi(receive_dynamic_message(kernel_socket));

	log_debug(logger, "CPU Escribir en FD: '%d'", kernel_fd);

	if (kernel_fd < 0) {
		pcb_actual->exit_code = kernel_fd;
		cpu_finalizar();
	}

	//free(informacion);
	free(buffer);
	free(fd);
	free(size);
	free(pid);

}

/*
 * LEER ARCHIVO
 *
 * Informa al Kernel que el proceso requiere que se lea un archivo previamente abierto.
 * El mismo leera "tamanio" de bytes luego del cursor.
 * No es necesario mover el cursor luego de esta operación
 *
 * @syntax 	TEXT_READ_FILE (leer)
 * @param	descriptor_archivo		Descriptor de archivo del archivo abierto
 * @param	informacion			Puntero que indica donde se guarda la informacion leida
 * @param	tamanio				Tamanio de la informacion a leer
 * @return	void
 */
void kernel_leer(t_descriptor_archivo descriptor_archivo, t_puntero informacion, t_valor_variable tamanio) {
	pcb_actual->statistics.op_priviliges++;

	log_debug(logger, "|PRIMITIVA| Leer FD: '%d', Info: '%d', Tamanio: '%d'", descriptor_archivo, informacion, tamanio);


	//informacion = cpu_dereferenciar(informacion);
	runFunction(kernel_socket, "cpu_read_file", 4, string_itoa(descriptor_archivo), string_itoa(informacion), string_itoa(tamanio), string_itoa(pcb_actual->pid));
	//wait_response();

	descriptor_archivo = atoi(receive_dynamic_message(kernel_socket));

	if (descriptor_archivo < 0) {
		pcb_actual->exit_code = descriptor_archivo;
		cpu_finalizar();
	}else{

	}
}
