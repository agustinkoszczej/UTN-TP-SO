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
#include<commons/log.h>
#include<commons/string.h>

#include <arpa/inet.h>
#include<sys/socket.h>

int activar_logger = 1; //Cambiar a 1 cuando no lo estemos probando. Esto es para que no nos llenemos de archivos cada vez que lo probamos.
t_log* 	archivo_logger;

int socketConsola; //Pruebo, no estoy seguro si lo necesito como global

void logger (char * accion, char * tipo){
	if(activar_logger == 1){
	t_log_level nivel;
	nivel = log_level_from_string(tipo);

	time_t tiempo = time(0);
		struct tm * timeinfo = localtime(&tiempo);
		char fecha [128];
		strftime(fecha, 128, "%d/%m/%y", timeinfo);
	char * archivo_nombre = "Consola";
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
}

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
	}

	enviarMensajeDePruebaDeConexion();
}

void enviarMensajeDePruebaDeConexion()
{
	char* mensajeAEnviar = "Hola Kernel"; //Esto va cambiar por el handshake
	int tamanoAEnviar = strlen(mensajeAEnviar);
	send(socketConsola, &tamanoAEnviar , 4, 0);

	send(socketConsola, mensajeAEnviar, strlen(mensajeAEnviar),0);
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
