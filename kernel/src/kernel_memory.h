#ifndef KERNEL_MEMORY_H_
#define KERNEL_MEMORY_H_

#include <pthread.h>
#include <stdbool.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <socket.h>
#include "kernel_interface.h"

void kernel_send_program_to_memory();

#endif /* KERNEL_MEMORY_H_ */
