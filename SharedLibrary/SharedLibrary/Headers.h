/*
 * Headers.h
 *
 *
 *      Author: utnso
 */

#ifndef SHAREDLIBRARY_HEADERS_H_
#define SHAREDLIBRARY_HEADERS_H_

#define HEADER_LENGTH 50

typedef enum {
	HEADER_HANDSHAKE, //0
	HEADER_PASAMANOS,
	HEADER_TAMANIO,
	HEADER_ARCHIVO
} t_header;

#endif /* SHAREDLIBRARY_HEADERS_H_ */
