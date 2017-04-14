/*
 * Conversor.c
 *
 *  Created on: 13/4/2017
 *      Author: utnso
 */


char* intToString(int nro) {
	return string_from_format("%d", nro);
}
int stringToInt(char* string) {
	return atoi(string);
}
