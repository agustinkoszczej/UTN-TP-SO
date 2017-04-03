/*
 * Consola.c
 *
 *
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>

#include "Consola.h"

void cargarConfigConsola() {

	t_config* configConsola;
	configConsola = config_create(path);

	consola_config.IP_KERNEL = config_get_string_value(configConsola,
			"IP_KERNEL");
	consola_config.PUERTO_KERNEL = config_get_int_value(configConsola,
			"PUERTO_KERNEL");

	printf("Archivo de configuracion de Consola cargado exitosamente!\n");
}

void mostrarConfigConsola() {
	printf("IP_KERNEL=%s\n", consola_config.IP_KERNEL);
	printf("PUERTO_KERNEL=%d\n", consola_config.PUERTO_KERNEL);
}

int main() {
	printf("Iniciando Consola...\n\n");

	//CARGAR ARCHIVO DE CONFIGURACIÓN
	cargarConfigConsola();
	//MOSTRAR ARCHIVO DE CONFIGURACIÓN
	mostrarConfigConsola();

	return EXIT_SUCCESS;
}
