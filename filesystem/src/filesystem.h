#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/bitarray.h>
#include <console.h>
#include <stdbool.h>
#include <config.h>
#include <socket.h>
#include "filesystem_interface.h"

#define PORT "PORT"
#define MOUNT_POINT "MOUNT_POINT"
#define CONFIG_FIELDS_N 2

#define TAMANIO_BLOQUES "TAMANIO_BLOQUES"
#define CANTIDAD_BLOQUES "CANTIDAD_BLOQUES"

#define TAMANIO "TAMANIO"
#define BLOQUES "BLOQUES"

char* mount_point;
int block_size;
int block_quantity;

t_dictionary* fns;
pthread_mutex_t mx_main;
pthread_mutex_t request_mutex;
t_log* logger;
t_bitarray* bitmap;

bool create_file(char* path);
bool delete_file(char* path);
bool validate_file(char* path);
char* get_data(char* path, int offset, int size);
bool save_data(char* path, int offset, int size, char* buffer);

#endif /* FILESYSTEM_H_ */
