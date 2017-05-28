#ifndef TEXTOS_H_
#define TEXTOS_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/string.h>
#define CONFIG_TITLE "Configuracion inicial"

/**
	* @NAME: show_title
	* @DESC: muestra por pantalla un título decorado
	*/
void show_title(char*);
/**
	* @NAME: wait_any_key
	* @DESC: la consola queda a la espera que el usuario ingrese cualquier letra.
	*/
void wait_any_key();
/**
	* @NAME: clear_screen
	* @DESC: limpia la consola.
	* AVISO: Sólo funciona en la consola de Linux.
	* En Eclipse dice "TERM environment variable not set."
	*/
void clear_screen();
/**
	* @NAME: println
	* @DESC: imprime por pantalla un texto y agrega al final un salto de línea.
	*
	* @PARAMS:
	* 		text - el texto a imprimir
	*/
void println(char* text);
/**
	* @NAME: println_with_clean
	* @DESC: limpia la pantalla y luego imprime un texto.
	*
	* @PARAMS:
	* 		text - el texto a imprimir
	*/
void println_with_clean(char* text);

#endif /* TEXTOS_H_ */
