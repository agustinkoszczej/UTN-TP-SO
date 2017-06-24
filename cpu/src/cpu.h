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

#define CREATE "c"
#define READ "r"
#define WRITE "w"

#define CPU "CPU"

#define IP_KERNEL "IP_KERNEL"
#define PUERTO_KERNEL "PUERTO_KERNEL"
#define IP_MEMORIA "IP_MEMORIA"
#define PUERTO_MEMORIA "PUERTO_MEMORIA"
#define CONFIG_FIELDS_N 4

/*
 * MEMORY
 */
char* mem_buffer;
int frame_size;
int stack_size;
int frame_from_pid_and_page;
int page_from_pointer;
/*
 * KERNEL
 */
int mem_delay;
int quantum_sleep;
int kernel_shared_var;
char* read_info;

pcb* pcb_actual;

t_dictionary * fns;
pthread_mutex_t mx_main;
pthread_mutex_t planning_mutex;
t_log* logger;

int vars_in_stack();
int calculate_page_for_var();
int calculate_offset_for_var();
void wait_response(pthread_mutex_t* mutex);
void signal_response(pthread_mutex_t* mutex);

char* get_flag(t_banderas flags);

#endif /* CPU_H_ */
