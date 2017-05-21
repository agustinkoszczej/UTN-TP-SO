/*
 * Kernel.h
 *
 *
 *      Author: utnso
 */

#ifndef KERNEL_H_
#define KERNEL_H_

#include <SharedLibrary/ServerCommons.h>
#include <SharedLibrary/Hilos.h>
#include <SharedLibrary/Handshake.h>
#include <SharedLibrary/Headers.h>

#include "ConfigKernel.h"
#include "Procesos.h"


struct sockaddr_in direccionServidorKernel;
int socketServidorKernel;

int activadoConsola;
int activadoCPU;

int socketKernel;

int procesos;

unsigned int tamanio_pagina;

int enviandoDatosConsola;

#endif /* KERNEL_H_ */
