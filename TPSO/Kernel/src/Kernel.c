#include <stdio.h>
#include <stdlib.h>

#include "Kernel.h"
#include "Procesos.h"


//***************************************************************************************
//* 									C O N S O LA									*
//***************************************************************************************

pthread_mutex_t semConsola;

char* getScript(int socketConsola){

	char* script = recibirMensajeConTamanioDinamico(Conexiones.ListaClientes[buscarCliente(socketConsola)].socket);
	Conexiones.ListaClientes[buscarCliente(socketConsola)].atendido = false;

	if (script==NULL){
		//return "ERROR";
	}
	return script;
}

void atenderConsola(int socketConsola){


	int header = recibirHeader(socketConsola);
	switch (header){
	case HEADER_PROGRAMA:
		 //pthread_mutex_lock(&semConsola);
		 	 enviandoDatosConsola++;
		 //pthread_mutex_lock(&semConsola);
		if(enviandoDatosConsola <= kernel_config.GRADO_MULTIPROG){
			crearHiloConParametro(&Conexiones.ListaClientes[buscarCliente(socketConsola)].hilo, crearProceso ,socketConsola);
		}
	break;

	}

}

void recibirConsolas() {
	configurarServidor(&direccionServidorKernel, &socketServidorKernel,
			kernel_config.PUERTO_PROG, &activadoConsola);
	printf("Esperando Consolas...\n");

	pthread_mutex_init(&semConsola, NULL);

	enviandoDatosConsola = 0;

	while (1) {
		int socketConsola = recibirCliente(socketServidorKernel);
		pthread_t hilo;
		devolverHandshake(socketConsola, KERNEL);
		crearHiloConParametro(&hilo, atenderConsola,
				socketConsola);
	}

	pthread_mutex_destroy(&semConsola);

}

//***************************************************************************************
//* 									M E M O R I A									*
//***************************************************************************************

bool pedirPaginas(t_proceso* proceso, char* codigo) {
	char* paginasProceso = intToString(proceso->PCB->cantidad_paginas);
	char* pidProceso = intToString(proceso->PCB->PID);
	char* respuesta = malloc(1);

		//HEADER
		enviarHeader(socketKernel, HEADER_PROGRAMA);
		//PID
		enviarMensaje(socketKernel, pidProceso, sizeof(int));
		//PAGINAS
		enviarMensaje(socketKernel, paginasProceso, sizeof(int));

		int tamanio = strlen(codigo)+1;
		//TAMAÑO CODIGO
		enviarMensaje(socketKernel, intToString(tamanio), sizeof(int));
		//CODIGO
		enviarMensaje(socketKernel, codigo, tamanio);

		respuesta = recibirMensajeConEspera(socketKernel, 1);
		printf("Rta: %d\n",stringToInt(respuesta));

	free(pidProceso);
	free(paginasProceso);
	return stringToInt(respuesta);
}

void recibirTamanioPagina(){
	tamanio_pagina = stringToInt(recibirMensajeConEspera(socketKernel, sizeof(int)));
}

void conectarMemoria() {
	struct sockaddr_in direccionParaConectarseServidor = crearDireccionCliente(
			kernel_config.PUERTO_MEM, kernel_config.IP_MEMORIA);
	socketKernel = crearSocket();
	conectar(socketKernel, &direccionParaConectarseServidor, MEMORIA);

	iniciarHandshake(KERNEL, MEMORIA, socketKernel);
	recibirTamanioPagina();
}

//***************************************************************************************
//* 									C P U											*
//***************************************************************************************

void recibirCPUs() {

	configurarServidor(&direccionServidorKernel, &socketServidorKernel,
			kernel_config.PUERTO_CPU, &activadoCPU);

	printf("Esperando CPUs...\n");

	while (1) {
			int socketCPU = recibirCliente(socketServidorKernel);
			pthread_t hilo;
			devolverHandshake(socketCPU, KERNEL);
			crearHiloConParametro(&hilo, quedarEsperaRecibirMensajes, socketCPU);
		}
}

//***************************************************************************************
//* 									F I L E  S Y S T E M							*
//***************************************************************************************


void conectarFileSystem() {
	struct sockaddr_in direccionParaConectarseServidor = crearDireccionCliente(
			kernel_config.PUERTO_FS, kernel_config.IP_FS);
	socketKernel = crearSocket();
	conectar(socketKernel, &direccionParaConectarseServidor, FILE_SYSTEM);

	iniciarHandshake(KERNEL, FILE_SYSTEM, socketKernel);
}


//***************************************************************************************
//* 									M A I N											*
//***************************************************************************************


int main(void) {
	printf("Iniciando Kernel...\n\n");
//	logger("Iniciando Kernel", "INFO");
	procesos = 0;
	//CARGAR ARCHIVO DE CONFIGURACIÓN
	cargarConfigKernel();
	//MOSTRAR ARCHIVO DE CONFIGURACIÓN
	mostrarConfigKernel();


	inicializarClientes();
	inicializarServidores();

	conectarFileSystem();
	conectarMemoria();

	pthread_t hiloConsolas;
	crearHilo(&hiloConsolas, recibirConsolas);
	pthread_t hiloCPUs;
	crearHilo(&hiloCPUs, recibirCPUs);

	while (1);

	return 0;
}
