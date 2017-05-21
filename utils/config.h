#ifndef CONFIG2_H_
#define CONFIG2_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/config.h>
#include <commons/error.h>
#include "console.h"

/**
	* @NAME: print_config
	* @DESC: imprime por pantalla el archivo de configuracion.
	*
	* @PARAMS:
	* 		config - estructura cargada del archivo
	* 		config_fields - un array con los nombres de los campos definidos
	* 		total_fields - el total de campos a recorrer
	*/
void print_config(t_config *config, const char *config_fields[], int total_fields);
/**
	* @NAME: load_config
	* @DESC: carga el archivo de configuracion o muestra errores.
	*
	* @PARAMS:
	* 		config - puntero a la estructura donde se guardará la configuración
	* 		argc - total de parámetros pasados al proceso
	* 		dir - la dirección donde ir a buscar el archivo de configuración
	*/
void load_config(t_config **config, int argc, char* dir);

#endif /* CONFIG2_H_ */
