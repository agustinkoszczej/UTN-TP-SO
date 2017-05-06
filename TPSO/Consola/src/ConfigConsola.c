/*
 * ConfigConsola.c
 *
 *  Created on: 18/4/2017
 *      Author: utnso
 */

#include "ConfigConsola.h"
#include "Consola.h"

void cargarConfigConsola() {

	t_config* configConsola;
	configConsola = config_create(RUTA_CFG);

	consola_config.IP_KERNEL_CONSOLA = config_get_string_value(configConsola,
			"IP_KERNEL");
	consola_config.PUERTO_KERNEL_CONSOLA = config_get_int_value(configConsola,
			"PUERTO_KERNEL");

	printf("Archivo de configuracion de Consola cargado exitosamente!\n");
}

void mostrarConfigConsola() {
	printf("IP_KERNEL=%s\n", consola_config.IP_KERNEL_CONSOLA);
	printf("PUERTO_KERNEL=%d\n", consola_config.PUERTO_KERNEL_CONSOLA);
}
