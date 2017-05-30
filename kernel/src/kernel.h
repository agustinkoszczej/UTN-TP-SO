#ifndef KERNEL_H_
#define KERNEL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/collections/dictionary.h>
#include <commons/error.h>
#include <commons/log.h>
#include <console.h>
#include <config.h>
#include <socket.h>
#include <pcb.h>
#include "kernel_interface.h"

#define NO_ERRORES 0
#define NO_SE_PUEDEN_RESERVAR_RECURSOS -1
#define ERROR_SIN_DEFINIR -20
#define DESCONEXION_CONSOLA -6

#define NEW_LIST 1
#define READY_LIST 2
#define EXEC_LIST 3
#define BLOCK_LIST 4
#define EXIT_LIST 5

#define FIFO 1
#define RR 2

#define PUERTO_PROG "PUERTO_PROG"
#define PUERTO_CPU "PUERTO_CPU"
#define IP_MEMORIA "IP_MEMORIA"
#define IP_FILESYSTEM "IP_FILESYSTEM"
#define PUERTO_MEMORIA "PUERTO_MEMORIA"
#define IP_FS "IP_FS"
#define PUERTO_FS "PUERTO_FS"
#define QUANTUM "QUANTUM"
#define QUANTUM_SLEEP "QUANTUM_SLEEP"
#define ALGORITMO "ALGORITMO"
#define GRADO_MULTIPROG "GRADO_MULTIPROG"
#define SEM_IDS "SEM_IDS"
#define SEM_INIT "SEM_INIT"
#define SHARED_VARS "SHARED_VARS"
#define STACK_SIZE "STACK_SIZE"
#define CONFIG_FIELDS_N 15

int p_counter;
int multiprog;
int quantum;
int stack_size;
int planning_alg;
t_dictionary* shared_vars;
t_dictionary* sem_ids;

t_queue* new_queue;
t_list* ready_list;
t_list* exec_list;
t_list* block_list;
t_queue* exit_queue;

/*
typedef struct {
	int socket;
//	char* program;
//	pthread_t thread_info;
	pcb* pcb;
} t_program;
*/
//int last_socket;

typedef struct {
	bool busy;
	int socket;
} t_cpu;
t_list* cpu_list;

typedef struct {
	int socket;
	int pid;
	int state;
} t_socket_pcb;

t_socket_pcb process_struct;
t_list* socket_pcb_list;

pthread_mutex_t p_counter_mutex;
pthread_mutex_t console_mutex;
pthread_mutex_t pcb_list_mutex;
pthread_mutex_t socket_pcb_mutex;
pthread_mutex_t planning_mutex;
pthread_mutex_t cpu_mutex;
pthread_mutex_t process_in_memory_mutex;
pthread_mutex_t shared_vars_mutex;

bool planning_running;

t_dictionary * fns;
t_log* logger;

int mem_page_size;
int process_in_memory;

void move_to_list(pcb* pcb, int list_name);
pcb* find_pcb_by_socket(int socket);
pcb* find_pcb_by_pid(int pid);
void remove_pcb_from_socket_pcb_list(pcb* n_pcb);
void remove_cpu_from_cpu_list(t_cpu* cpu);
t_cpu* find_cpu_by_socket(int socket);
t_socket_pcb* find_socket_by_pid(int pid);
void add_process_in_memory();
void substract_process_in_memory();
void short_planning();

#endif /* KERNEL_H_ */
