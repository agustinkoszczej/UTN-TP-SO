#ifndef MEMORIA_H_
#define MEMORIA_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <console.h>
#include <config.h>
#include <socket.h>
#include <pthread.h>
#include <pcb.h>
#include <exit_codes.h>

#define PUERTO "PUERTO"
#define MARCOS "MARCOS"
#define MARCO_SIZE "MARCO_SIZE"
#define ENTRADAS_CACHE "ENTRADAS_CACHE"
#define CACHE_X_PROC "CACHE_X_PROC"
#define RETARDO_MEMORIA "RETARDO_MEMORIA"
#define CONFIG_FIELDS_N 6

#define PID_ADM_STRUCT 0 //si le pongo un numero < 0 es alta paja cambiar todos los ifs que preguntan por pid < 0 en vez si es igual a -1

typedef struct {
	int frame;
	int pid;
	int pag;
} t_adm_table;

typedef struct {
	t_adm_table* adm_table;
	int lru;
} t_cache;

t_list* adm_list;
t_list* cache_list;


t_dictionary* fns;

pthread_mutex_t frames_mutex;
pthread_mutex_t frames_cache_mutex;

t_log* logger;

char* frames_cache;
char* frames;
int frames_count;
int frame_size;
int mem_delay;
int cache_size;
int cache_x_proc;
int stack_size;

void start_program();
char* read_bytes(int pid, int page, int offset, int size);
void store_bytes(int pid, int page, int offset, int size, char* buffer);
void add_pages(int pid, int n_frames);
void finish_program(int pid);
void free_page(int pid, int page);
//char* parse_t_adm_table(int frame, int pid, int pag);

bool has_available_frames(int n_frames);

#endif /* MEMORIA_H_ */
