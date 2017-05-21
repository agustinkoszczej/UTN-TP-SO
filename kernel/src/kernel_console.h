#ifndef KERNEL_CONSOLE_H_
#define KERNEL_CONSOLE_H_

#include <pthread.h>
#include <stdbool.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <socket.h>
#include "kernel.h"
#include "kernel_interface.h"
#include "kernel_memory.h"

void* kernel_create_pcb();
void kernel_send_response_to_console(bool response);

#endif /* KERNEL_CONSOLE_H_ */
