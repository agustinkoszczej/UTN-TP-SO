#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/log.h>
#include <console.h>
#include <stdbool.h>
#include <config.h>
#include <socket.h>
#include "filesystem_interface.h"

#define PORT "PORT"
#define MOUNT_POINT "MOUNT_POINT"
#define CONFIG_FIELDS_N 2

char* mount_point;

t_dictionary* fns;
pthread_mutex_t mx_main;
pthread_mutex_t request_mutex;
t_log* logger;

#endif /* FILESYSTEM_H_ */
