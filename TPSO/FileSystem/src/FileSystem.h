/*
 * FileSystem.h
 *
 *
 *      Author: utnso
 */

#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include <SharedLibrary/ServerCommons.h>
#include <SharedLibrary/Hilos.h>

#include "ConfigFileSystem.h"

int activadoKernel;

struct sockaddr_in direccionServidorFileSystem;
int socketServidorFileSystem;

#endif /* FILESYSTEM_H_ */
