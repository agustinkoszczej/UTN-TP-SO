/*
 * ServerCommons.h
 *
 *
 *      Author: utnso
 */

#ifndef SERVERCOMMONS_H_
#define SERVERCOMMONS_H_


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include<time.h>

#include<commons/log.h>
#include<commons/string.h>
#include <commons/config.h>
#include <commons/collections/list.h>

#include <pthread.h>

#include "Handshake.h"
#include "Headers.h"
#include "Hilos.h"

#define MAX_CLIENTES 300
#define MAX_SERVIDORES 300
//struct sockaddr_in direccion;
//int socketServidor, tamanioDireccion;

int activado;

typedef struct {
	//UN MISMO PROCESO NO PUEDE TENER LOS MISMOS SOCKETS, YA QUE SE CONECTA AL MISMO PUERTO Y NO SE PUEDEN REPETIR LOS SOCKETS EN UN MISMO PUERTO
	int tipoProceso; //SON LOS MISMOS QUE LOS HANDSHAKE: 0|KERNEL, 1|FILESYSTEM, 2|MEMORIA, 3|CPU, 4|CONSOLA
	int socket; //SI ES 0 EL SOCKET SIGNIFICA QUE NO HAY NADIE, POR ESO INICIALIZAR CLIENTES SETTEO TODO A 0
	struct sockaddr_in direccion; //GUARDO LOS DATOS DE LA DIRECCION DE ESE CLIENTE: IP, PUERTO, ETC
	pthread_t hilo;
	void* proceso;
	bool atendido;
} t_cliente;

typedef struct {
	t_cliente ListaClientes [MAX_CLIENTES]; //LA LISTA DE CLIENTES QUE TENGO CONECTADO
	t_cliente ListaServidores [MAX_SERVIDORES]; //LA LISTA DE SERVIDORES A LOS QUE ESTOY CONECTADO
}t_conexiones;

t_conexiones Conexiones;

int crearSocket();
struct sockaddr_in crearDireccionCliente(unsigned short PUERTO, char* IP);
struct sockaddr_in crearDireccionServidor(unsigned short PUERTO);
void conectar(int socketCliente, struct sockaddr_in* direccionServidor, int tipo);
void enviarMensaje(int socketCliente, char* msg, int tamanio);
void permitirReutilizar(int servidor, int* activado);
int cantidadClientes();
char* recibirMensaje(int cliente, int tamanioMsj, int criterio);
char* recibirMensajeSinEspera(int socketCliente, int tamanioMsj);
char* recibirMensajeConEspera(int socketCliente, int tamanioMsj);
void hacerListen(int servidor);
void hacerBindeo(int servidor, struct sockaddr_in* direccionServidor);
void configurarServidor(struct sockaddr_in* direccion, int* socketServidor, unsigned short PUERTO, int* activado);
int agregarCliente(t_cliente Cliente);
int agregarServidor(t_cliente Servidor);
void quitarCliente(int indice);
int buscarCliente(int nroSocket);
int buscarServidor(int nroSocket);
int hacerAccept(int socketServidor);
void quedarEsperaRecibirMensajes(int socketCliente, t_handshake quienReciboMsjs, t_handshake quienSoy);
void recibirMensajesYReplicar(int socketCliente);
//void recibirClientes(int socketServidor);
int recibirCliente(int socketServidor);
int getHandshake(int socketAConectar);
void enviarHeader(int socketCliente, int header);
int recibirHeader(int socketCliente);
void iniciarHandshake(t_handshake quienIniciaHandshake, t_handshake quienDevuelveHandshake, int socket);
void devolverHandshake(int socketCliente, t_handshake quienSoyYo);
void enviarMensajeConTamanioDinamico(int socketCliente, char* msj);
int recibirTamanioMensaje(int socketCliente);
char* recibirMensajeConTamanioDinamico(int socket);
int stringToInt(char* string);
char* intToString(int nro);
void inicializarClientes();
void inicializarServidores();
void mostrarClientes();
char* nombreProceso(int tipoProceso);
void replicarMensajeClientes(int idProcesoAReplicar, char* msj);
void replicarMensajeServidores(int idProcesoAReplicar, char* msj);

#endif /* SERVERCOMMONS_H_ */
