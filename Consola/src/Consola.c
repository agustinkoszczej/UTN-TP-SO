/*
 * Consola.c
 *
 *
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>

#include "Consola.h"
#include "ServerCommons.h"

#include<stdbool.h>
#include<sys/types.h>
#include<time.h>
#include<string.h>
#include<commons/log.h>
#include<commons/string.h>

t_log* archivo_logger;

void logger(char * accion, char * tipo) {

	t_log_level nivel;
	nivel = log_level_from_string(tipo);

	time_t tiempo = time(0);
	struct tm * timeinfo = localtime(&tiempo);
	char fecha[128];
	strftime(fecha, 128, "%d/%m/%y", timeinfo);
	char * archivo_nombre = "Consola";
	char * archivo_fecha = string_new();
	string_append_with_format(&archivo_fecha, "Logger %s %c%c-%c%c-%c%c.txt",
			archivo_nombre, fecha[0], fecha[1], fecha[3], fecha[4], fecha[6],
			fecha[7]);
	printf("%s\n", archivo_fecha);

	archivo_logger = log_create(archivo_fecha, archivo_nombre, 0, nivel);
	switch (tipo[0]) {
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

void cargarConfigConsola() {

	t_config* configConsola;
	configConsola = config_create(path);

	consola_config.IP_KERNEL = config_get_string_value(configConsola,
			"IP_KERNEL");
	consola_config.PUERTO_KERNEL = config_get_int_value(configConsola,
			"PUERTO_KERNEL");

	logger("Archivo de configuracion de Consola cargado exitosamente", "INFO");
	printf("\nArchivo de configuracion de Consola cargado exitosamente!\n");
}

void mostrarConfigConsola() {
	printf("IP_KERNEL=%s\n", consola_config.IP_KERNEL);
	printf("PUERTO_KERNEL=%d\n", consola_config.PUERTO_KERNEL);
}


int getHandshake() {
	char* handshake = recibirMensajeSinEspera(socketConsola, 1);
	return charToInt(handshake);
}

void handshakear() {
	char *head = string_from_format("%c", HEADER_HANDSHAKE);
	char *hand = string_from_format("%c", HANDSHAKE_CONSOLA);
	enviarMensaje(socketConsola, head, 1);
	enviarMensaje(socketConsola, hand, 1);

	if (getHandshake() == HANDSHAKE_KERNEL) {
		printf("Se conecto satisfactoriamente con el Kernel!");
	}
}

void conectarKernel(){
	struct sockaddr_in direccionParaConectarseServidor = crearDireccionCliente(consola_config.PUERTO_KERNEL, consola_config.IP_KERNEL);
	int cliente = crearSocket();
	conectar(cliente, &direccionParaConectarseServidor);
	handshakear();
}

int main() {
	printf("Iniciando Consola...\n\n");
	logger("Iniciando Consola", "INFO");
	//CARGAR ARCHIVO DE CONFIGURACIÓN
	cargarConfigConsola();
	//MOSTRAR ARCHIVO DE CONFIGURACIÓN
	mostrarConfigConsola();

	conectarKernel();

	return EXIT_SUCCESS;
}
