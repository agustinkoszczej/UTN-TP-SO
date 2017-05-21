/*
 * Headers.h
 *
 *
 *      Author: utnso
 */

#ifndef SHAREDLIBRARY_HEADERS_H_
#define SHAREDLIBRARY_HEADERS_H_

#define MAX_HEADER 50

typedef enum{
	HEADER_HANDSHAKE = 0, //0
	HEADER_PROGRAMA = 1, // 1
	HEADER_ERROR = 2,
} t_header;

#endif /* SHAREDLIBRARY_HEADERS_H_ */
