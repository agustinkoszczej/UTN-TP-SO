/*
 * ConfigKernel.c
 *
 *  Created on: 19/4/2017
 *      Author: utnso
 */

#include "ConfigKernel.h"
#include "Kernel.h"
#define NOMBRE_PROCESO "Kernel"

void cargarConfigKernel() {

	t_config* configKernel;
	configKernel = config_create(RUTA_CFG);

	kernel_config.PUERTO_PROG = config_get_int_value(configKernel,
			"PUERTO_PROG");
	kernel_config.PUERTO_CPU = config_get_int_value(configKernel, "PUERTO_CPU");
	kernel_config.IP_MEMORIA = config_get_string_value(configKernel,
			"IP_MEMORIA");
	kernel_config.PUERTO_MEMORIA = config_get_int_value(configKernel,
			"PUERTO_MEMORIA");
	kernel_config.IP_FS = config_get_string_value(configKernel, "IP_FS");
	kernel_config.PUERTO_FS = config_get_int_value(configKernel, "PUERTO_FS");
	kernel_config.QUANTUM = config_get_int_value(configKernel, "QUANTUM");
	kernel_config.QUANTUM_SLEEP = config_get_int_value(configKernel,
			"QUANTUM_SLEEP");
	kernel_config.ALGORITMO = config_get_string_value(configKernel,
			"ALGORITMO");
	kernel_config.GRADO_MULTIPROG = config_get_int_value(configKernel,
			"GRADO_MULTIPROG");
	kernel_config.SEM_IDS = config_get_array_value(configKernel, "SEM_IDS");
	kernel_config.SEM_INIT = config_get_array_value(configKernel, "SEM_INIT");
	kernel_config.SHARED_VARS = config_get_array_value(configKernel,
			"SHARED_VARS");
	kernel_config.STACK_SIZE = config_get_int_value(configKernel, "STACK_SIZE");
	logger("Archivo de configuracion cargado exitosamente", "INFO", NOMBRE_PROCESO);
}

void mostrarArrayDinamico(char** array) {
	int i = 0;
	printf("[");
	while (array[i] != '\0') {
		if (array[i + 1] != '\0') //PREGUNTO ESTO PARA VER SI ES ULTIMO ELEMENTO O NO, ASI HAGO UNA COMA O NO
			printf("%s, ", array[i]);
		else
			//SI ES ULTIMO ELEMENTO
			printf("%s]\n", array[i]);
		i++;
	}
}
void mostrarConfigKernel() {
	printf("PUERTO_PROG=%d\n", kernel_config.PUERTO_PROG);
	printf("PUERTO_CPU=%d\n", kernel_config.PUERTO_CPU);
	printf("IP_MEMORIA=%s\n", kernel_config.IP_MEMORIA);
	printf("PUERTO_MEMORIA=%d\n", kernel_config.PUERTO_MEMORIA);
	printf("IP_FS=%s\n", kernel_config.IP_FS);
	printf("PUERTO_FS=%d\n", kernel_config.PUERTO_FS);
	printf("QUANTUM=%d\n", kernel_config.QUANTUM);
	printf("QUANTUM_SLEEP=%d\n", kernel_config.QUANTUM_SLEEP);
	printf("ALGORITMO=%s\n", kernel_config.ALGORITMO);
	printf("GRADO_MULTIPROG=%d\n", kernel_config.GRADO_MULTIPROG);
	printf("SEM_IDS=");
	mostrarArrayDinamico(kernel_config.SEM_IDS);
	printf("SEM_INIT=");
	mostrarArrayDinamico(kernel_config.SEM_INIT);
	printf("SHARED_VARS=");
	mostrarArrayDinamico(kernel_config.SHARED_VARS);
	printf("STACK_SIZE=%d\n", kernel_config.STACK_SIZE);
}
