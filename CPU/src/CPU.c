/*
 * CPU.c
 *
 *
 *      Author: utnso
 */

#include "CPU.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<stdbool.h>
#include<sys/types.h>
#include<time.h>
#include<commons/log.h>
#include<commons/string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MSJ_DINAMICO 50

int socketCPU;

void cargarConfigCPU() {

  t_config* configCPU;
  configCPU = config_create(path);

  cpu_config.IP_KERNEL = config_get_string_value(configCPU,
      "IP_KERNEL");
  cpu_config.PUERTO_KERNEL = config_get_int_value(configCPU,
      "PUERTO_KERNEL");

  printf("Archivo de configuracion de Consola cargado exitosamente!\n");
  logger("Archivo de configuracion de Consola cargado exitosamente", "INFO");
}

void mostrarConfigCPU() {
  printf("IP_KERNEL=%s\n", cpu_config.IP_KERNEL);
  printf("PUERTO_KERNEL=%d\n", cpu_config.PUERTO_KERNEL);
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

struct sockaddr_in *direccionServidor(){
  struct sockaddr_in *retorno = malloc(sizeof(struct sockaddr_in));

  retorno->sin_family = AF_INET;
  retorno->sin_addr.s_addr = inet_addr(cpu_config.IP_KERNEL);
  retorno->sin_port = htons(cpu_config.PUERTO_KERNEL);

  return retorno;
}

char* intToString(int nro) {
 return string_from_format("%d", nro);
}
int stringToInt(char* string) {
 return atoi(string);
}

int getSocket(){
  return socket(AF_INET, SOCK_STREAM,0);
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

	char* handshake = intToString(QuienIniciaElHandshake);
	enviarMensaje(socket, handshake, 1);		//ENVIO AL SOCKET QUE ME CONECTO EL VALOR DE QUIEN INICIA EL HANDSHAKE (EN ESTE CASO ES UN 4 QUE REPRESENTA A LA CONSOLA)

	if(obtenerHandshake(socket) == QuienDevuelveHandshake) {   //OBTENGO EL HANDSHAKE DEL PROCESO AL QUE LE ENVIE ANTERIORMENTE EL HANDSHAKE (KERNEL) Y LO COMPARO
		printf("Se conecto satisfactoriamente al Kernel\n");
	}

}

void conectarAKernel(){

  socketCPU = getSocket();

  struct sockaddr_in *direccionDelServidor = direccionServidor();
  if(connect(socketCPU,(struct sockaddr*) direccionDelServidor, sizeof(*direccionDelServidor)) != 0){
    perror("Error en el connect");																		//ERROR
  }

  iniciarHandshake(CPU,KERNEL,socketCPU);
}

void esperarMensaje(){
	while(1){
		char* buffer = malloc(4);

			while(1){
				recv(socketCPU, buffer, 4, MSG_WAITALL);
				if(stringToInt(buffer) == HEADER_PASAMANOS){
						char* msg = malloc(50);
						recv(socketCPU, msg, 50, 0);
						puts(msg);
						free(msg);
				}
			}
	}
}


int main (void){
	printf("Iniciando CPU...\n\n");
	logger("Iniciando CPU", "INFO");
	//CARGAR ARCHIVO DE CONFIGURACIÓN
	 cargarConfigCPU();
	//MOSTRAR ARCHIVO DE CONFIGURACIÓN
	 mostrarConfigCPU();


	 conectarAKernel();
	 esperarMensaje();

	return EXIT_SUCCESS;
}
