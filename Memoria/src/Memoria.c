/*
 * Memoria.c
 *
 *
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>
#include<stdbool.h>
#include<sys/types.h>
#include<time.h>
#include<string.h>
#include<commons/log.h>
#include<commons/string.h>

#include "Memoria.h"
#include "Results.h"
#include "ServerManager.h"

#include <arpa/inet.h>
#include<sys/socket.h>

int socketMemoria;
char * name = "Memoria";

//-----------------------------------------------------------------------------------------------------

struct sockaddr_in *direccionServidor(){ //Kernel
	struct sockaddr_in *retorno = malloc(sizeof(struct sockaddr_in));

	retorno->sin_family = AF_INET;
	retorno->sin_addr.s_addr = inet_addr("127.0.0.1");	//Hardcodeado nomas en este check
	retorno->sin_port = htons(8080);

	return retorno;
}

char* intToString(int nro) {
 return string_from_format("%d", nro);
}
int stringToInt(char* string) {
 return atoi(string);
}

char* recibirMensaje(int socket,int tamanioMsj,int criterio) {
	char* buffer = malloc(tamanioMsj);

	int bytesRecibidos = recv(socket, buffer, tamanioMsj, criterio);

	if(bytesRecibidos <= 0) {
		printf("El Cliente %d se desconecto\n",socket);
	}
	buffer[tamanioMsj] = '\0';
	return buffer;

}

int obtenerHandshake(int socketAlQueMeconecte) {
	char* headerHandshake = recibirMensaje(socketAlQueMeconecte, 4, MSG_WAITALL);

	if(stringToInt(headerHandshake) == HEADER_HANDSHAKE) {
		return stringToInt(recibirMensaje(socketAlQueMeconecte, 1, MSG_WAITALL));
	}
	else {
		return 1; //Error
	}
}

void enviarMensaje(int socketCliente, char* msg, int tamanio) {
 send(socketCliente, msg, tamanio, 0);
}

void enviarHeader(int socket,int header) {
	char* head = intToString(header);

	send(socket, head, 4, 0);
}

void iniciarHandshake(t_handshake QuienIniciaElHandshake, t_handshake QuienDevuelveHandshake, int socket) {

	enviarHeader(socket, HEADER_HANDSHAKE);  //ENVIO EL HEADER DEL HANDSHAKE PARA QUE EL PROCESO QUE LO RECIBE VERIFIQUE SI ES UN HANDSHAKE LO QUE LE ENVIO O NO
	logger("Enviado mensaje de prueba de conexion al Kernel", "TRACE", name);
	char* handshake = intToString(QuienIniciaElHandshake);
	enviarMensaje(socket, handshake, 1);		//ENVIO AL SOCKET QUE ME CONECTO EL VALOR DE QUIEN INICIA EL HANDSHAKE (EN ESTE CASO ES UN 4 QUE REPRESENTA A LA CONSOLA)

	if(obtenerHandshake(socket) == QuienDevuelveHandshake) {   //OBTENGO EL HANDSHAKE DEL PROCESO AL QUE LE ENVIE ANTERIORMENTE EL HANDSHAKE (KERNEL) Y LO COMPARO
		printf("\nSe conecto satisfactoriamente al Kernel\n");
	}

}

void conectarAKernel(){

	socketMemoria = getSocket();

	struct sockaddr_in *VdireccionServidor = direccionServidor();
	if(connect(socketMemoria,(struct sockaddr*) VdireccionServidor, sizeof(*VdireccionServidor)) != 0){
		perror("Error en el connect");																		//ERROR
		logger("Error en el connect", "ERROR", name);
	}

	iniciarHandshake(MEMORIA,KERNEL,socketMemoria); //INICIO EL HANDSHAKE
}

void esperarMensaje(){
	while(1){
		char* buffer = malloc(4);
			while(1){
				if(recv(socketMemoria, buffer, 4, MSG_WAITALL)<=0){
					close(socketMemoria);
					printf("Servidor desconectado\n");
					return;
				}
				else{
					if(stringToInt(buffer) == HEADER_PASAMANOS){
						char* msg = malloc(50);
						recv(socketMemoria, msg, 50, 0);
						puts(msg);
						free(msg);
					}
				}
			}
	}
}


//-----------------------------------------------------------------------------------------------------

void cargarConfigMemoria() {

	t_config* configMemoria;
	configMemoria = config_create(path);

	memoria_config.PUERTO = config_get_int_value(configMemoria, "PUERTO");
	logger("Configurado Puerto", "INFO", name);
	memoria_config.MARCOS = config_get_int_value(configMemoria, "MARCOS");
	logger("Configurado Marcos", "INFO", name);
	memoria_config.MARCO_SIZE = config_get_int_value(configMemoria,
			"MARCO_SIZE");
	logger("Configurado Marco Size", "INFO", name);
	memoria_config.ENTRADAS_CACHE = config_get_int_value(configMemoria,
			"ENTRADAS_CACHE");
	logger("Configurado Entradas Cache", "INFO", name);
	memoria_config.CACHE_X_PROC = config_get_int_value(configMemoria,
			"CACHE_X_PROC");
	logger("Configurado Cache por Proc.", "INFO", name);
	memoria_config.RETARDO_MEMORIA = config_get_int_value(configMemoria,
			"RETARDO_MEMORIA");
	logger("Configurado Retardo Memoria", "INFO", name);

	printf("Archivo de configuracion de Memoria cargado exitosamente!\n");
	logger("Archivo de configuracion cargado exitosamente", "INFO", name);
}

void mostrarConfigMemoria() {
	printf("PUERTO=%d\n", memoria_config.PUERTO);
	printf("MARCOS=%d\n", memoria_config.MARCOS);
	printf("MARCO_SIZE=%d\n", memoria_config.MARCO_SIZE);
	printf("ENTRADAS_CACHE=%d\n", memoria_config.ENTRADAS_CACHE);
	printf("CACHE_X_PROC=%d\n", memoria_config.CACHE_X_PROC);
	printf("RETARDO_MEMORIA=%d\n", memoria_config.RETARDO_MEMORIA);
}

int main(void) {
	printf("Iniciando Memoria...\n\n");
	logger("Iniciando Memoria", "INFO", name);

	//CARGAR ARCHIVO DE CONFIGURACIÓN
	cargarConfigMemoria();
	//MOSTRAR ARCHIVO DE CONFIGURACIÓN
	mostrarConfigMemoria();

	//Result r = SetupServer(); //No se hace para este check

	conectarAKernel();

	printf("\n");

	//puts(r.msg);

	esperarMensaje();


	return EXIT_SUCCESS;
}
