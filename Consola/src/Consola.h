#ifndef CONSOLA_H_
#define CONSOLA_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <commons/string.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <commons/error.h>
#include <commons/temporal.h>
#include <commons/log.h>
#include <string.h>
#include <socket.h>
#include <config.h>
#include <console.h>
#include <pcb.h>
#include "consola_interface.h"

#define NO_ERRORES 0
#define NO_SE_PUEDEN_RESERVAR_RECURSOS -1
#define ERROR_SIN_DEFINIR -20
#define DESCONEXION_CONSOLA -6

#define IP_KERNEL "IP_KERNEL"
#define PUERTO_KERNEL "PUERTO_KERNEL"
#define CONFIG_FIELDS_N 2

t_dictionary* message_map;

typedef struct {
	int pid;
	int socket;
	int c_message;
	char* time_start;
	char* time_finish;
} t_process;

typedef struct {
	int pid;
	char* time;
	char* message;
} t_message;

t_log* logger;
t_list* messages_list;
t_list* process_list;

t_dictionary * fns;

pthread_mutex_t messages_list_mutex;
pthread_mutex_t process_list_mutex;
pthread_mutex_t p_counter_mutex;
pthread_mutex_t print_menu_mutex;

int p_counter;

void new_message(char* text, int pid);
void abort_program(t_process* process, int exit_code);
t_process* find_process_by_pid(int pid);
t_process* find_process_by_socket(int socket);
void remove_from_process_list(t_process* process);

#endif /* CONSOLA_H_ */
