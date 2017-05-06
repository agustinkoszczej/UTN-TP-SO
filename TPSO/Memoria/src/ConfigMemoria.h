/*
 * ConfigMemoria.h
 *
 *
 *      Author: utnso
 */

#ifndef CONFIGMEMORIA_H_
#define CONFIGMEMORIA_H_

typedef struct {
	int PUERTO_MEMORIA;
	int MARCOS;
	int MARCO_SIZE;
	int ENTRADAS_CACHE;
	int CACHE_X_PROC;
	int RETARDO_MEMORIA;
}Config_Memoria;

Config_Memoria memoria_config;

#define RUTA_CFG "Memoria.cfg"

void cargarConfigMemoria();
void mostrarConfigMemoria();


#endif /* CONFIGMEMORIA_H_ */
