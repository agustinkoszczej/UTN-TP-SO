#ifndef CPU_INTERFACE_H_
#define CPU_INTERFACE_H_

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <socket.h>
#include <pcb.h>
#include <parser/parser.h>
#include <exit_codes.h>

#include "cpu.h"
#include "cpu_primitivas.h"

int mem_socket;
int kernel_socket;

void server_connectionClosed(socket_connection * connection);

/*
 * MEMORY
 */
void memory_identify(socket_connection* connection, char** args);
void memory_response_read_bytes_from_page(socket_connection* connection, char** args);
void memory_response_store_bytes_in_page(socket_connection* connection, char** args);

/*
 * KERNEL
 */
void kernel_receive_pcb(socket_connection* connection, char** args);
void kernel_quantum_page_stack_size(socket_connection* connection, char** args);
void kernel_response_get_shared_var(socket_connection* connection, char** args);
void kernel_response_set_shared_var(socket_connection* connection, char** args);
void kernel_response_file(socket_connection* connection, char** args);
void kernel_response_validate_file(socket_connection* connection, char** args);
void kernel_response_read_file(socket_connection* connection, char** args);

void kernel_response(socket_connection* connection, char** args);


bool finished;

/*
 * PRIMITIVAS CPU
 */
t_puntero cpu_definirVariable(t_nombre_variable variable);
t_puntero cpu_obtenerPosicionVariable(t_nombre_variable variable);
t_valor_variable cpu_dereferenciar(t_puntero puntero);
void cpu_asignar(t_puntero puntero, t_valor_variable variable);
t_valor_variable cpu_obtenerValorCompartida(t_nombre_compartida variable);
t_valor_variable cpu_asignarValorCompartida(t_nombre_compartida variable, t_valor_variable valor);
void cpu_irAlLabel(t_nombre_etiqueta t_nombre_etiqueta);
void cpu_llamarSinRetorno(t_nombre_etiqueta etiqueta);
void cpu_llamarConRetorno(t_nombre_etiqueta etiqueta, t_puntero donde_retornar);
void cpu_finalizar(void);
void cpu_retornar(t_valor_variable retorno);

/*
 * PRIMITIVAS KERNEL
 */
void kernel_wait(t_nombre_semaforo identificador_semaforo);
void kernel_signal(t_nombre_semaforo identificador_semaforo);
t_puntero kernel_reservar(t_valor_variable espacio);
void kernel_liberar(t_puntero puntero);
t_descriptor_archivo kernel_abrir(t_direccion_archivo direccion, t_banderas flags);
void kernel_borrar(t_descriptor_archivo descriptor_archivo);
void kernel_cerrar(t_descriptor_archivo descriptor_archivo);
void kernel_moverCursor(t_descriptor_archivo descriptor_archivo, t_valor_variable posicion);
void kernel_escribir(t_descriptor_archivo descriptor_archivo, void* informacion, t_valor_variable tamanio);
void kernel_leer(t_descriptor_archivo descriptor_archivo, t_puntero informacion, t_valor_variable tamanio);

AnSISOP_funciones functions;
AnSISOP_kernel kernel_functions;

t_stack* stack_create();

t_puntero* malloc_pointer;

#endif /* CPU_INTERFACE_H_ */
