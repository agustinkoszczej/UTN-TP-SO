/*
 * ServerManager.h
 *
 *  Created on: 2/4/2017
 *      Author: utnso
 */

#ifndef SERVERMANAGER_H_
#define SERVERMANAGER_H_

#include "Results.h"

Result SetupServer(int, Result AlLevantarServidor(int));
void permitirReutilizarPuerto(int);
struct sockaddr_in CrearDireccionServer(int);
int crearSocket(void);

#endif /* SERVERMANAGER_H_ */
