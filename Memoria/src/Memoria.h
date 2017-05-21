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
#include <md5.h>
#include <pcb.h>

#define NO_ERRORES 0
#define NO_SE_PUEDEN_RESERVAR_RECURSOS -1
#define ERROR_SIN_DEFINIR -20
#define DESCONEXION_CONSOLA -6

#define PUERTO "PUERTO"
#define MARCOS "MARCOS"
#define MARCO_SIZE "MARCO_SIZE"
#define ENTRADAS_CACHE "ENTRADAS_CACHE"
#define CACHE_X_PROC "CACHE_X_PROC"
#define RETARDO_MEMORIA "RETARDO_MEMORIA"
#define CONFIG_FIELDS_N 6

/*
 typedef struct {
 int frame;
 int pos;
 } curPos;
 */

MD5_CTX context;

typedef struct {
	unsigned char hash[16];
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

t_config *config;
t_dictionary* fns;

pthread_mutex_t mx_main;
pthread_mutex_t frames_mutex;
pthread_mutex_t frames_cache_mutex;

t_log* logger;

char* frames_cache;
char* frames;
int cur_pos;
int frames_count;
int frame_size;
int mem_delay;
int cache_size;
int cache_x_proc;

void start_program();
char* read_bytes(int pid, int page, int offset, int size);
void store_bytes(int pid, int page, int offset, int size, char* buffer);
void add_pages(int pid, int n_frames);
void finish_program(int pid);

bool has_available_frames(int n_frames);

#endif /* MEMORIA_H_ */
