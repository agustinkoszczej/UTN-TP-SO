#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdbool.h>
#include<time.h>
#include<commons/log.h>
#include<commons/string.h>

#include "ServerManager.h"


#include "Kernel.h"

t_log* 	archivo_logger;

void logger (char * accion, char * tipo){

	t_log_level nivel;
	nivel = log_level_from_string(tipo);

	time_t tiempo = time(0);
		struct tm * timeinfo = localtime(&tiempo);
		char fecha [128];
		strftime(fecha, 128, "%d/%m/%y", timeinfo);
	char * archivo_nombre = "Kernel";
	char * archivo_fecha = string_new();
	string_append_with_format(&archivo_fecha, "Logger %s %c%c-%c%c-%c%c.txt", archivo_nombre, fecha[0], fecha[1], fecha[3], fecha[4], fecha[6], fecha[7]);
	printf("%s", archivo_fecha);

	archivo_logger = log_create(archivo_fecha, archivo_nombre, 0, nivel);
	switch(tipo[0]){
	case 'T':
			log_trace(archivo_logger, accion);
			break;
	case 'D':
			log_debug(archivo_logger, accion);
			break;
	case 'I':
			log_info(archivo_logger, accion);
			break;
	case 'W':
			log_warning(archivo_logger, accion);
			break;
	case 'E':
			log_error(archivo_logger, accion);
				break;
	}

}



void cargarConfigKernel() {

	t_config* configKernel;
	configKernel = config_create(path);

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

	printf("Archivo de configuracion de Kernel cargado exitosamente!\n");
	logger("Archivo de configuracion cargado exitosamente", "INFO");
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

int main(void) {
	printf("Iniciando Kernel...\n\n");
	logger("Iniciando Kernel", "INFO");

	//CARGAR ARCHIVO DE CONFIGURACIÓN
	cargarConfigKernel();
	//MOSTRAR ARCHIVO DE CONFIGURACIÓN
	mostrarConfigKernel();

	SetupServer();

	return 0;
}
