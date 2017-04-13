/*
 * Consola.c
 *
 *
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>

#include "Consola.h"

#include<stdbool.h>
#include<sys/types.h>
#include<time.h>
#include<string.h>
#include<commons/string.h>

#include <arpa/inet.h>
#include<sys/socket.h>


int socketConsola; //Pruebo, no estoy seguro si lo necesito como global


void cargarConfigConsola() {

	t_config* configConsola;
	configConsola = config_create(path);

	consola_config.IP_KERNEL = config_get_string_value(configConsola,
			"IP_KERNEL");
	consola_config.PUERTO_KERNEL = config_get_int_value(configConsola,
			"PUERTO_KERNEL");

	printf("Archivo de configuracion de Consola cargado exitosamente!\n");
	logger("Archivo de configuracion de Consola cargado exitosamente", "INFO");
}

void mostrarConfigConsola() {
	printf("IP_KERNEL=%s\n", consola_config.IP_KERNEL);
	printf("PUERTO_KERNEL=%d\n", consola_config.PUERTO_KERNEL);
}

struct sockaddr_in *direccionServidor(){
	struct sockaddr_in *retorno = malloc(sizeof(struct sockaddr_in));

	retorno->sin_family = AF_INET;
	retorno->sin_addr.s_addr = inet_addr(consola_config.IP_KERNEL);
	retorno->sin_port = htons(consola_config.PUERTO_KERNEL);

	return retorno;
}

int getSocket(){
	return socket(AF_INET, SOCK_STREAM,0);
}

void conectarAKernel(){

	socketConsola = getSocket();

	struct sockaddr_in *VdireccionServidor = direccionServidor();
	if(connect(socketConsola,(struct sockaddr*) VdireccionServidor, sizeof(*VdireccionServidor)) != 0){
		perror("Error en el connect");																		//ERROR
		logger("Error en el connect", "ERROR");
	}

	enviarMensajeDePruebaDeConexion();
}

void enviarMensajeDePruebaDeConexion()
{
	char* mensajeAEnviar = "Hola Kernel"; //Esto va cambiar por el handshake
	int tamanoAEnviar = strlen(mensajeAEnviar);
	send(socketConsola, &tamanoAEnviar , 4, 0);
	send(socketConsola, mensajeAEnviar, strlen(mensajeAEnviar),0);
	logger("Enviado mensaje de prueba de conexion al Kernel", "TRACE");
}

int main() {
	printf("Iniciando Consola...\n\n");
	logger("Iniciando Consola", "INFO");

	//CARGAR ARCHIVO DE CONFIGURACIÓN
	cargarConfigConsola();

	//MOSTRAR ARCHIVO DE CONFIGURACIÓN
	mostrarConfigConsola();

	conectarAKernel();



	return EXIT_SUCCESS;
}
