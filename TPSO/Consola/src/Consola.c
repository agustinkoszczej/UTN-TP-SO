/*
 * Consola.c
 *
 *
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>

#include "Consola.h"

void cargarYEnviarArchivo() {
	size_t length = 0;
	int size = 0;
	char* line = NULL;
	char* contenido = string_new();
	ssize_t read = getline(&line, &length, programa);

	while (read != -1) {
		string_append(&contenido, line);
		size += strcspn(line, "\n") + 1;
		free(line);
		length = 0;
		read = getline(&line, &length, programa);
	}
	free(line);
	//string_append(&contenido, "\0");
	size += 1;

	printf("\n%s\n", contenido);

	enviarHeader(socketConsola, HEADER_PROGRAMA);
	enviarMensaje(socketConsola, intToString(size), sizeof(int));
	enviarMensaje(socketConsola, contenido, size);
	free(contenido);
	fclose(programa);
}

void conectarKernel() {
	struct sockaddr_in direccionParaConectarseServidor = crearDireccionCliente(
			consola_config.PUERTO_KERNEL_CONSOLA,
			consola_config.IP_KERNEL_CONSOLA);
	socketConsola = crearSocket();
	conectar(socketConsola, &direccionParaConectarseServidor, KERNEL);

	iniciarHandshake(CONSOLA, KERNEL, socketConsola);
}

void escucharPedidos() {
	char* header;
	while (1) {
		header = recibirHeader(socketConsola);
		//procesarHeader(header);
		free(header);
	}
}

void iniciarPrograma(){
	while (1) {
				char* path;
				puts("Consola: Ingrese ruta archivo ansisop");
				//scanf("%s", path);
				//Hardcodeado por paja de andar escribiendolo siempre
				path = "facil.ansisop";
				programa = fopen(path, "r");
				if (programa == NULL) {
					perror("No se encontro el archivo.");
				}
				cargarYEnviarArchivo();
				scanf("%s", path);
			}
}

void finalizarPrograma(){

}

void desconectarConsola(){

}

void limpiarMensajes(){

}

int main() {
	printf("Iniciando Consola...\n\n");
	cargarConfigConsola();
	mostrarConfigConsola();

	conectarKernel();
	pthread_t hiloEscuchador;
	crearHilo(&hiloEscuchador, escucharPedidos);

	int opc;

	while(1){
	printf("SELECCIONE UNA OPCION:\n");

	printf("1. Iniciar Programa\n");
	printf("2. Finalizar Programa\n");
	printf("3. Desconectar Consola\n");
	printf("4. Limpiar Mensajes\n");
	printf("5. Salir\n");

	scanf("%d", &opc);

	switch(opc){
	case 1:
		iniciarPrograma();
	break;

	case 2:
		finalizarPrograma();
	break;

	case 3:
		desconectarConsola();
	break;

	case 4:
		limpiarMensajes();
	break;

	case 5:
		return EXIT_SUCCESS;
	break;

	default:
		printf("Opcion invalida!\n");
	break;
	}
}
}

