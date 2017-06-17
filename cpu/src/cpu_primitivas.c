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
	bool is_digit = isdigit(identificador_variable) ? true : false;

	t_stack* stack = list_get(pcb_actual->i_stack,
			list_size(pcb_actual->i_stack) - 1);

	if (stack->vars == NULL)
		stack->vars = list_create();

	if (stack->args == NULL)
		stack->args = list_create();

	int vars_c = vars_in_stack();

	int occupied_bytes = vars_c * 4;
	int relative_page = occupied_bytes / frame_size;

	t_arg_var* n_var = malloc(sizeof(t_arg_var));
	n_var->id = identificador_variable;
	n_var->pag = pcb_actual->page_c + relative_page;
	n_var->off = occupied_bytes - relative_page * frame_size;
	n_var->size = 4;

	if (!is_digit)
		list_add(stack->vars, n_var);
	else
		list_add(stack->args, n_var);

	log_debug(logger, "Definir variable '%c'", identificador_variable);

	if (relative_page > stack_size) {
		runFunction(kernel_socket, "cpu_error", 1, STACK_OVERFLOW); //TODO Agregar a interface de Kernel
		cpu_finalizar();
	}

	return n_var;
}

/*
 * OBTENER POSICION de una VARIABLE
 *
 * Devuelve el desplazamiento respecto al inicio del segmento Stacken que se encuentra el valor de la variable identificador_variable del contexto actual.
 * En caso de error, retorna -1.
 *
 * @sintax	TEXT_REFERENCE_OP (&)
 * @param	identificador_variable 		Nombre de la variable a buscar (De ser un parametro, se invocara sin el '$')
 * @return	Donde se encuentre la variable buscada
 */
t_puntero cpu_obtenerPosicionVariable(t_nombre_variable identificador_variable) {
	int space_occupied = 0;
	int i, j;
	for (i = 0; i < list_size(pcb_actual->i_stack); i++) {
		t_stack* stack = list_get(pcb_actual->i_stack, i);

		for (j = 0; j < list_size(stack->vars); j++) {
			t_arg_var* var = list_get(stack->vars, j);

			if (var->id == identificador_variable) {
				log_debug(logger, "Obtener Posicion Variable '%c' en '%d'",
						identificador_variable, space_occupied);
				return space_occupied;
			} else
				space_occupied += 4;
		}
	}

	return -1;
}

/*
 * DEREFERENCIAR
 *
 * Obtiene el valor resultante de leer a partir de direccion_variable, sin importar cual fuera el contexto actual
 *
 * @sintax	TEXT_DEREFERENCE_OP (*)
 * @param	direccion_variable	Lugar donde buscar
 * @return	Valor que se encuentra en esa posicion
 */
t_valor_variable cpu_dereferenciar(t_puntero direccion_variable) {
	int n_page = direccion_variable / frame_size;
	int n_offset = direccion_variable - n_page * frame_size;

	char* pid = string_itoa(pcb_actual->pid);
	char* page = string_itoa(pcb_actual->page_c + n_page + 1);
	char* offset = string_itoa(n_offset);
	char* size = string_itoa(4);

	runFunction(mem_socket, "i_read_bytes_from_page", 4, pid, page, offset,
			size);
	wait_response();
	log_debug(logger, "Dereferenciar en '%d', valor = '%s'", direccion_variable,
			mem_buffer);
	return atoi(mem_buffer);
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
	int n_page = direccion_variable / frame_size;
	int n_offset = direccion_variable - n_page * frame_size;

	char* pid = string_itoa(pcb_actual->pid);
	char* page = string_itoa(pcb_actual->page_c + n_page + 1);
	char* offset = string_itoa(n_offset);
	char* size = string_itoa(4);
	char* buffer = string_itoa(valor);

	runFunction(mem_socket, "i_store_bytes_in_page", 5, pid, page, offset, size,
			buffer);
	wait_response();
	log_debug(logger, "Asignar '%s' en '%d'", buffer, direccion_variable);
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
	wait_response();

	log_debug(logger, "Obtener Valor Compartida '%d'", kernel_shared_var);

	return atoi(kernel_shared_var);
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
t_valor_variable cpu_asignarValorCompartida(t_nombre_compartida variable,
		t_valor_variable valor) {
	runFunction(kernel_socket, "cpu_set_shared_var", 2, variable, valor);
	wait_response();

	log_debug(logger, "Asignar Valor Compartida '%d'", valor);

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
	if (dictionary_has_key(pcb_actual->i_label, t_nombre_etiqueta))
		pcb_actual->pc = dictionary_get(pcb_actual->i_label, t_nombre_etiqueta);
	else
		//pcb_actual->pc = -1; //TODO Ver que hacer cuando no existe la funcion

	log_debug(logger, "Ir a Label '%d'", pcb_actual->pc);
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
	log_debug(logger, "Llamar sin retorno"); //TODO no deberia caer nunca aca, ya que siempre nos van a dar ansisop que anden
	t_stack* stack_aux = stack_create();
	stack_aux->retpos = pcb_actual->pc;
	list_add(pcb_actual->i_stack, stack_aux);

	cpu_irAlLabel(etiqueta);
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
	log_debug(logger, "Llamar con retorno '%s', '%d'", etiqueta, donde_retornar);
	t_stack* stack_aux = stack_create();
	t_retvar* retvar_aux = malloc(sizeof(t_retvar));

	retvar_aux->pag = donde_retornar / frame_size + pcb_actual->page_c;
	retvar_aux->off = donde_retornar % frame_size;
	retvar_aux->size = 4;

	stack_aux->retvar = retvar_aux;
	stack_aux->retpos = &(pcb_actual->pc);

	list_add(pcb_actual->i_stack, stack_aux);

	cpu_irAlLabel(etiqueta);
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
	log_debug(logger, "Finalizar Programa");
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
	log_debug(logger, "Retornar '%d'", retorno);
	t_stack* stack_aux = list_get(pcb_actual->i_stack,
			pcb_actual->i_stack->elements_count - 1);

	t_puntero offset_abs = ((stack_aux->retvar)->pag * frame_size)
			+ (stack_aux->retvar)->off;
	cpu_asignar(offset_abs, retorno);

	cpu_finalizar();
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
	log_debug(logger, "Wait: Semaforo \"%s\"", identificador_semaforo);
	runFunction(kernel_socket, "cpu_wait_sem", 1, identificador_semaforo); //TODO Agregar a interface de Kernel
	wait_response();
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
	log_debug(logger, "Signal: Semaforo \"%s\"", identificador_semaforo);
	runFunction(kernel_socket, "cpu_signal_sem", 1, identificador_semaforo); //TODO Agregar a interface de Kernel
	wait_response();
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

	runFunction(kernel_socket, "cpu_malloc", 2, string_itoa(espacio),
			string_itoa(pcb_actual->pid));
	wait_response();
	log_debug(logger, "CPU Reservar '%d' bytes en '%d'", espacio, malloc_pointer);
	if(malloc_pointer < 0){
		runFunction(kernel_socket, "cpu_error", 1,
						string_itoa(malloc_pointer));
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
	runFunction(kernel_socket, "cpu_free", 2, string_itoa(puntero),
			string_itoa(pcb_actual->pid));
	wait_response();
	log_debug(logger, "CPU Libero '%d'", puntero);
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
t_descriptor_archivo kernel_abrir(t_direccion_archivo direccion,
		t_banderas flags) {
	char* n_flag = get_flag(flags);

	runFunction(kernel_socket, "cpu_validate_file", 1, string_itoa(direccion));
	wait_kernel_response();

	if (!validate_file) {
		runFunction(kernel_socket, "cpu_error", 1,
				string_itoa(NO_EXISTE_ARCHIVO));
		cpu_finalizar();
		return NO_EXISTE_ARCHIVO;
	}

	runFunction(kernel_socket, "cpu_open_file", 3, string_itoa(direccion),
			n_flag, string_itoa(pcb_actual->pid));
	wait_kernel_response();
	log_debug(logger, "CPU Abrir en FD '%d'", kernel_file_descriptor);
	return kernel_file_descriptor;
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
	log_debug(logger, "CPU Borrar");
	runFunction(kernel_socket, "cpu_delete_file", 1,
			string_itoa(descriptor_archivo));
	wait_response();
	if (kernel_file_descriptor == NO_EXISTE_ARCHIVO) {
		runFunction(kernel_socket, "cpu_error", 1,
				string_itoa(NO_EXISTE_ARCHIVO));
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
	log_debug(logger, "CPU Cerrar");
	runFunction(kernel_socket, "cpu_close_file", 2,
			string_itoa(descriptor_archivo), string_itoa(pcb_actual->pid));
	wait_kernel_response();
	if (kernel_file_descriptor == ARCHIVO_SIN_ABRIR_PREVIAMENTE){
		runFunction(kernel_socket, "cpu_error", 1,
				string_itoa(ARCHIVO_SIN_ABRIR_PREVIAMENTE));
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
void kernel_moverCursor(t_descriptor_archivo descriptor_archivo,
		t_valor_variable posicion) {
	log_debug(logger, "CPU Mover Cursor");
	runFunction(kernel_socket, "cpu_seek_file", 3,
			string_itoa(descriptor_archivo), string_itoa(posicion),
			string_itoa(pcb_actual->pid));
	wait_kernel_response();
}

/*
 * ESCRIBIR ARCHIVO
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
void kernel_escribir(t_descriptor_archivo descriptor_archivo, void* informacion,
		t_valor_variable tamanio) {

	char* buffer = string_new();
	memcpy(buffer, informacion, tamanio);
	strtok(buffer, "\n");
	log_debug(logger, "CPU Escribir FD: '%d', Info: '%s', Tamanio: '%d'", descriptor_archivo, buffer, tamanio);

	runFunction(kernel_socket, "cpu_write_file", 4,
			string_itoa(descriptor_archivo), buffer, string_itoa(tamanio),
			string_itoa(pcb_actual->pid));

	wait_kernel_response();
	log_debug(logger, "CPU Escribir kernel_response ok");

	if (kernel_file_descriptor == ESCRIBIR_SIN_PERMISOS) {
		runFunction(kernel_socket, "cpu_error", 1,
				string_itoa(ESCRIBIR_SIN_PERMISOS));
		cpu_finalizar();
	}
	if (kernel_file_descriptor == NO_EXISTE_ARCHIVO) {
		runFunction(kernel_socket, "cpu_error", 1,
				string_itoa(NO_EXISTE_ARCHIVO));
		cpu_finalizar();
	}
	signal_kernel_response();
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
void kernel_leer(t_descriptor_archivo descriptor_archivo, t_puntero informacion,
		t_valor_variable tamanio) {
	log_debug(logger, "CPU Leer FD: '%d', Info: '%d', Tamanio: '%d'", descriptor_archivo, informacion, tamanio);

	runFunction(kernel_socket, "cpu_read_file", 4,
			string_itoa(descriptor_archivo), string_itoa(informacion),
			string_itoa(tamanio), string_itoa(pcb_actual->pid));
	wait_kernel_response();

	if (kernel_file_descriptor == LEER_SIN_PERMISOS) {
		runFunction(kernel_socket, "cpu_error", 1,
				string_itoa(kernel_file_descriptor));
		cpu_finalizar();
	}
}

t_stack* stack_create() {
	t_stack* stack = malloc(sizeof(t_stack));
	/*stack->args = list_create();
	 stack->vars = list_create();*/
	return stack;
}
