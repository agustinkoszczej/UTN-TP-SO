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
#include <time.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/log.h>

#include "Kernel.h"
#include "./Utilities/Multiplexor.h"

int main(void) {
	printf("Iniciando Kernel...\n\n");
	logger("Iniciando Kernel", "INFO", NOMBRE_PROCESO);

	//CARGAR ARCHIVO DE CONFIGURACIÓN
	cargarConfigKernel();
	//MOSTRAR ARCHIVO DE CONFIGURACIÓN
	mostrarConfigKernel();

	Result r = SetupServer(kernel_config.PUERTO_CPU,Multiplexar);
	printf("\n");
	//logger por ok o error total
	if(r.noError != true)
		logger(r.msg, "ERROR", NOMBRE_PROCESO);
	else
		logger(r.msg, "INFO", NOMBRE_PROCESO);


	puts(r.msg);

	return 0;
}
