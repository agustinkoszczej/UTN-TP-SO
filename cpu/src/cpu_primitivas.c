/*
 * cpu_primitivas.c
 *
 *  Created on: 25/5/2017
 *      Author: utnso
 */

#include "cpu_primitivas.h"


static const int CONTENIDO_VARIABLE = 20;
static const int POSICION_MEMORIA = 0x10;
bool termino = false;



bool terminoElPrograma(void) {
	return termino;
}

bool isArgument(t_nombre_variable variable){
	if(isdigit(variable)){
		return true;
	}else{
		return false;
	}
}

t_puntero cpu_definirVariable(t_nombre_variable variable) {
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

	t_arg_var* n_var = malloc(sizeof(t_arg_var));
	n_var->id = variable;
	n_var->off = -1;
	n_var->pag = -1;
	n_var->size = -1;

	t_stack* last_stack = list_get(pcbActual-> i_stack, list_size(pcbActual-> i_stack)-1);
	t_list* vars_args_stack;

	if(!isArgument(variable)){
		log_trace(logger, "Definiendo nueva variable: '%c'.", variable);
		vars_args_stack = last_stack->vars;
	}
	else{
		log_trace(logger, "Definiendo nuevo argumento: '%c'.", variable);
		vars_args_stack = last_stack->args;
	}

	list_add(vars_args_stack, n_var);

	return n_var;


		// Defino una nueva posicion en el stack para la variable:
		/*int var_page = pcbActual->i_stack;
		int var_offset = pcbActual->stackPointer;

		while(var_offset > tamanioPagina){
			(var_page)++;
			var_offset -= tamanioPagina;
		}
		// Verifico si se desborda la pila en memoria:
		if(pcbActual->stackPointer + 4 > (tamanioPagina*tamanioStack)){
				log_trace(logger, "Stack Overflow al definir variable '%c'.", variable);
				huboStackOverflow = true;

			return ERROR;
		}else{
			// Selecciono registro actual del Ã­ndice de stack:
			registroStack* regStack = list_get(pcbActual->indiceStack, pcbActual->indiceStack->elements_count -1);

			if(regStack == NULL){ // si no hay registros, creo uno nuevo
				regStack = reg_stack_create();
				// Guardo el nuevo registro en el Ã­ndice:
				list_add(pcbActual->indiceStack, regStack);
			}

			if(!esArgumento(variable)){ // agrego nueva variable
				variable* new_var = malloc(sizeof(variable));
				new_var->nombre = variable;
				new_var->direccion.pagina = var_page;
				new_var->direccion.offset = var_offset;
				new_var->direccion.size = INT;

				list_add(regStack->vars, new_var);
			}
			else{ // agrego nuevo argumento
				variable* new_arg = malloc(sizeof(variable));
				new_arg->nombre = variable;
				new_arg->direccion.pagina = var_page;
				new_arg->direccion.offset = var_offset;
				new_arg->direccion.size = INT;

				list_add(regStack->args, new_arg);
			}

			log_trace(logger, "'%c' -> DirecciÃ³n lÃ³gica definida: %i, %i, %i.", variable, var_page, var_offset, INT);

			// Actualizo parÃ¡metros del PCB:
			int total_heap_offset = (pcbActual->paginas_codigo * tamanioPagina) + pcbActual->stackPointer;
			pcbActual->stackPointer += INT;
			pcbActual->paginaActualStack = (total_heap_offset + INT) / tamanioPagina;

			return total_heap_offset;
		} // fin else ERROR


	pcbActual->i_stack;

	return POSICION_MEMORIA;*/
}

t_puntero cpu_obtenerPosicionVariable(t_nombre_variable variable) {
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
	printf("Obtener posicion de %c\n", variable);
	return POSICION_MEMORIA;
}

t_valor_variable cpu_dereferenciar(t_puntero puntero) {
	/*
	 * DEREFERENCIAR
	 *
	 * Obtiene el valor resultante de leer a partir de direccion_variable, sin importar cual fuera el contexto actual
	 *
	 * @sintax	TEXT_DEREFERENCE_OP (*)
	 * @param	direccion_variable	Lugar donde buscar
	 * @return	Valor que se encuentra en esa posicion
	 */
	printf("Dereferenciar %d y su valor es: %d\n", puntero, CONTENIDO_VARIABLE);
	return CONTENIDO_VARIABLE;
}

void cpu_asignar(t_puntero puntero, t_valor_variable variable) {
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
	printf("Asignando en %d el valor %d\n", puntero, variable);
}

t_valor_variable cpu_obtenerValorCompartida(t_nombre_compartida variable) {
	/*
	 * OBTENER VALOR de una variable COMPARTIDA
	 *
	 * Pide al kernel el valor (copia, no puntero) de la variable compartida por parametro.
	 *
	 * @sintax	TEXT_VAR_START_GLOBAL (!)
	 * @param	variable	Nombre de la variable compartida a buscar
	 * @return	El valor de la variable compartida
	 */

}
t_valor_variable cpu_asignarValorCompartida(t_nombre_compartida variable,
		t_valor_variable valor) {
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

}
void cpu_irAlLabel(t_nombre_etiqueta t_nombre_etiqueta) {
	/*
	 * IR a la ETIQUETA
	 *
	 * Cambia la linea de ejecucion a la correspondiente de la etiqueta buscada.
	 *
	 * @sintax	TEXT_GOTO (goto)
	 * @param	t_nombre_etiqueta	Nombre de la etiqueta
	 * @return	void
	 */

}
void cpu_llamarSinRetorno(t_nombre_etiqueta etiqueta) {
	/*
	 * LLAMAR SIN RETORNO
	 *
	 * Preserva el contexto de ejecución actual para poder retornar luego al mismo.
	 * Modifica las estructuras correspondientes para mostrar un nuevo contexto vacío.
	 *
	 * Los parámetros serán definidos luego de esta instrucción de la misma manera que una variable local, con identificadores numéricos empezando por el 0.
	 *
	 * @sintax	Sin sintaxis particular, se invoca cuando no coresponde a ninguna de las otras reglas sintacticas
	 * @param	etiqueta	Nombre de la funcion
	 * @return	void
	 */

}
void cpu_llamarConRetorno(t_nombre_etiqueta etiqueta, t_puntero donde_retornar) {
	/*
	 * LLAMAR CON RETORNO
	 *
	 * Preserva el contexto de ejecución actual para poder retornar luego al mismo, junto con la posicion de la variable entregada por donde_retornar.
	 * Modifica las estructuras correspondientes para mostrar un nuevo contexto vacío.
	 *
	 * Los parámetros serán definidos luego de esta instrucción de la misma manera que una variable local, con identificadores numéricos empezando por el 0.
	 *
	 * @sintax	TEXT_CALL (<-)
	 * @param	etiqueta	Nombre de la funcion
	 * @param	donde_retornar	Posicion donde insertar el valor de retorno
	 * @return	void
	 */
}

void cpu_finalizar(void) {
	/*
	 * FINALIZAR
	 *
	 * Cambia el Contexto de Ejecución Actual para volver al Contexto anterior al que se está ejecutando, recuperando el Cursor de Contexto Actual y el Program Counter previamente apilados en el Stack.
	 * En caso de estar finalizando el Contexto principal (el ubicado al inicio del Stack), deberá finalizar la ejecución del programa.
	 *
	 * @sintax	TEXT_END (end)
	 * @param	void
	 * @return	void
	 */
	termino = true;
	printf("Finalizar\n");
}

void cpu_retornar(t_valor_variable retorno) {
	/*
	 * RETORNAR
	 *
	 * Cambia el Contexto de Ejecución Actual para volver al Contexto anterior al que se está ejecutando, recuperando el Cursor de Contexto Actual, el Program Counter y la direccion donde retornar, asignando el valor de retorno en esta, previamente apilados en el Stack.
	 *
	 * @sintax	TEXT_RETURN (return)
	 * @param	retorno	Valor a ingresar en la posicion corespondiente
	 * @return	void
	 */
}

void kernel_wait(t_nombre_semaforo identificador_semaforo) {
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
}

void kernel_signal(t_nombre_semaforo identificador_semaforo) {
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
}

t_puntero kernel_reservar(t_valor_variable espacio) {
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
}

void kernel_liberar(t_puntero puntero) {
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
}

t_descriptor_archivo kernel_abrir(t_direccion_archivo direccion,
		t_banderas flags) {
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
}

void kernel_borrar(t_descriptor_archivo descriptor_archivo) {
	/*
	 * BORRAR ARCHIVO
	 *
	 * Informa al Kernel que el proceso requiere que se borre un archivo.
	 *
	 * @syntax 	TEXT_DELETE_FILE (borrar)
	 * @param	descriptor_archivo		Descriptor de archivo del archivo a borrar
	 * @return	void
	 */
}

void kernel_cerrar(t_descriptor_archivo descriptor_archivo) {
	/*
	 * CERRAR ARCHIVO
	 *
	 * Informa al Kernel que el proceso requiere que se cierre un archivo.
	 *
	 * @syntax 	TEXT_CLOSE_FILE (cerrar)
	 * @param	descriptor_archivo		Descriptor de archivo del archivo abierto
	 * @return	void
	 */
}

void kernel_moverCursor(t_descriptor_archivo descriptor_archivo,
		t_valor_variable posicion) {
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
}

void kernel_escribir(t_descriptor_archivo descriptor_archivo, void* informacion,
		t_valor_variable tamanio) {
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
}

void kernel_leer(t_descriptor_archivo descriptor_archivo, t_puntero informacion,
		t_valor_variable tamanio) {
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
}



