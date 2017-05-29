#ifndef CPU_H_
#define CPU_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <commons/string.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>
#include <commons/error.h>
#include <console.h>
#include <config.h>
#include <socket.h>
#include <pcb.h>
#include "cpu_interface.h"
#include <parser/metadata_program.h>

#define FIFO 1
#define RR 2

#define CPU "CPU"

#define IP_KERNEL "IP_KERNEL"
#define PUERTO_KERNEL "PUERTO_KERNEL"
#define IP_MEMORIA "IP_MEMORIA"
#define PUERTO_MEMORIA "PUERTO_MEMORIA"
#define CONFIG_FIELDS_N 4

char* mem_buffer;
int kernel_shared_var;

t_dictionary * fns;
pthread_mutex_t mx_main;
pthread_mutex_t planning_mutex;
t_log* logger;

pcb* pcb_actual;
int frame_size;
int stack_size;

int vars_in_stack();
int calculate_page_for_var();
int calculate_offset_for_var();
void wait_response();

#endif /* CPU_H_ */
