/*
 * CPU.h
 *
 *  Created on: 9/4/2017
 *      Author: utnso
 */

#ifndef CPU_H_
#define CPU_H_

#include <commons/config.h>

#include <SharedLibrary/Handshake.h>
#include <SharedLibrary/Headers.h>
#include <SharedLibrary/logger.h>

typedef struct {
  char* IP_KERNEL;	//Después usar esta funcion para transformarlo inet_addr(IP_KERNEL)
  int PUERTO_KERNEL;
}Config_CPU;

Config_CPU cpu_config;
char* path = "CPU.cfg";

#endif /* CPU_H_ */
