/*
 * FileSystem.c
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

#include "FileSystem.h"
#include "ServerManager.h"
#include "Results.h"

#include <arpa/inet.h>
#include<sys/socket.h>

int socketFileSystem;

//-----------------------------------------------------------------------------------

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
	logger("Enviado mensaje de prueba de conexion al Kernel", "TRACE");
	char* handshake = intToString(QuienIniciaElHandshake);
	enviarMensaje(socket, handshake, 1);		//ENVIO AL SOCKET QUE ME CONECTO EL VALOR DE QUIEN INICIA EL HANDSHAKE (EN ESTE CASO ES UN 4 QUE REPRESENTA A LA CONSOLA)

	if(obtenerHandshake(socket) == QuienDevuelveHandshake) {   //OBTENGO EL HANDSHAKE DEL PROCESO AL QUE LE ENVIE ANTERIORMENTE EL HANDSHAKE (KERNEL) Y LO COMPARO
		printf("\nSe conecto satisfactoriamente al Kernel\n");
	}

}

void conectarAKernel(){

	socketFileSystem = getSocket();

	struct sockaddr_in *VdireccionServidor = direccionServidor();
	if(connect(socketFileSystem,(struct sockaddr*) VdireccionServidor, sizeof(*VdireccionServidor)) != 0){
		perror("Error en el connect");																		//ERROR
		logger("Error en el connect", "ERROR");
	}

	iniciarHandshake(FILE_SYSTEM,KERNEL,socketFileSystem); //INICIO EL HANDSHAKE
}

//-----------------------------------------------------------------------------------
void cargarConfigFileSystem() {

	t_config* configFileSystem;
	configFileSystem = config_create(path);

	file_system_config.PUERTO = config_get_int_value(configFileSystem,
			"PUERTO");
	file_system_config.PUNTO_MONTAJE = config_get_string_value(configFileSystem,
			"PUNTO_MONTAJE");

	printf("Archivo de configuracion de File System cargado exitosamente!\n");
	logger("Archivo de configuracion cargado exitosamente", "INFO");
}

void mostrarConfigFileSystem() {
	printf("PUERTO=%d\n", file_system_config.PUERTO);
	printf("PUNTO_MONTAJE=%s\n", file_system_config.PUNTO_MONTAJE);
}

void esperarMensaje(){
	while(1){
		char* buffer = malloc(4);

			while(1){
				recv(socketFileSystem, buffer, 4, MSG_WAITALL);
				if(stringToInt(buffer) == HEADER_PASAMANOS){
						char* msg = malloc(50);
						recv(socketFileSystem, msg, 50, 0);
						puts(msg);
						free(msg);
				}
			}
	}
}



/*Result AlLevantarServer(int servidor){
	//------------------------------

		 ResultWithValue r = AceptarConexion(servidor);

		//------------------------------

		 if(r.value != NULL)
			 RecibirMensaje(r.value);
		 else
			 return r.result;
}

Falta codear AceptarConexion y RecibirMensaje

*/


int main(void) {
	printf("Iniciando File System...\n\n");
	logger("Inicinado File System", "INFO");

	//CARGAR ARCHIVO DE CONFIGURACIÓN
	cargarConfigFileSystem();
	//MOSTRAR ARCHIVO DE CONFIGURACIÓN
	mostrarConfigFileSystem();

	//Result r = SetupServer(); No lo hacemos en este check

	conectarAKernel();

	printf("\n");


	//Result r = SetupServer(8080,AlLevantarServer);
	//puts(r.msg);

	esperarMensaje();

	return EXIT_SUCCESS;
}
