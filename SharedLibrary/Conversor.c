/*
 * Conversor.c
 *
 *  Created on: 13/4/2017
 *      Author: utnso
 */

#include "Handshake.h"

char* intToString(int nro) {
	return string_from_format("%d", nro);
}
int stringToInt(char* string) {
	return atoi(string);
}

char* textonombreProceso(t_handshake Id_Proceso){
	switch(Id_Proceso){
		case KERNEL:
			return "Kernel";
		break;
		case FILE_SYSTEM:
			return "File System";
		break;
		case MEMORIA:
			return "Memoria";
		break;
		case CPU:
			return "CPU";
		break;
		case CONSOLA:
			return "Consola";
		break;
		default:
			return "?";
		break;
	}
}
