/*
 * ConfigCPU.h
 *
 *  Created on: 19/4/2017
 *      Author: utnso
 */

#ifndef CONFIGCPU_H_
#define CONFIGCPU_H_

typedef struct {
  char* IP_KERNEL_CPU;	//Después usar esta funcion para transformarlo inet_addr(IP_KERNEL)
  int PUERTO_KERNEL_CPU;
}Config_CPU;

Config_CPU cpu_config;

#define RUTA_CFG "CPU.cfg"

void cargarConfigCPU();
void mostrarConfigCPU();

#endif /* CONFIGCPU_H_ */
