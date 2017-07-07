/*
 * cpu_primitivas.h
 *
 *  Created on: 25/5/2017
 *      Author: utnso
 */

#ifndef CPU_PRIMITIVAS_H_
#define CPU_PRIMITIVAS_H_

#include "cpu.h"
#include <parser/parser.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

#include <exit_codes.h>

bool finished;
int aborted_status;

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

#endif /* CPU_PRIMITIVAS_H_ */
