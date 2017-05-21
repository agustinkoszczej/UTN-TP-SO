/*
 * FileSystem.c
 *
 *
 *      Author: utnso
 */


#include "FileSystem.h"

void recibirKernel() {

	configurarServidor(&direccionServidorFileSystem, &socketServidorFileSystem,
			file_system_config.PUERTO_FILE_SYSTEM, &activadoKernel);
	//printf("IP = %s\n", inet_ntoa(direccionServidorFileSystem.sin_addr));
	printf("Esperando Kernel...\n");

	while (1) {
		int socketKernel = hacerAccept(socketServidorFileSystem);
		devolverHandshake(socketKernel, KERNEL);
		quedarEsperaRecibirMensajes(socketKernel, KERNEL, FILE_SYSTEM);
	}

}


int main(void) {
	printf("Iniciando File System...\n\n");

	//CARGAR ARCHIVO DE CONFIGURACIÓN
	cargarConfigFileSystem();
	//MOSTRAR ARCHIVO DE CONFIGURACIÓN
	mostrarConfigFileSystem();

	recibirKernel();

	return EXIT_SUCCESS;
}
