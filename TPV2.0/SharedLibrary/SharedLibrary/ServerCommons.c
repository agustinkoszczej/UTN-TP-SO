/*
 * ServerCommons.c
 *
 *
 *      Author: utnso
 */
#include<stdio.h>

#define CANT_CONEXIONES 50 //Esto es del listen

#include "ServerCommons.h"

int crearSocket() {
	return socket(AF_INET, SOCK_STREAM, 0);
}

struct sockaddr_in crearDireccionCliente(unsigned short PUERTO, char* IP) {
	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = inet_addr(IP);
	direccionServidor.sin_port = htons(PUERTO);
	return direccionServidor;
}

struct sockaddr_in crearDireccionServidor(unsigned short PUERTO) {
	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = INADDR_ANY;
	direccionServidor.sin_port = htons(PUERTO);
	return direccionServidor;
}

void conectar(int socket, struct sockaddr_in* direccionServidor, int tipo) {
	if (connect(socket, (void*) direccionServidor, sizeof(*direccionServidor))
			!= 0) {
		perror("No se pudo conectar");
		exit(1);
	}
	t_cliente Servidor;
	Servidor.direccion = *direccionServidor;
	Servidor.socket = socket;
	Servidor.tipoProceso = tipo;
	agregarServidor(Servidor);
}

void enviarMensaje(int socketCliente, char* msg, int tamanio) {
	send(socketCliente, msg, tamanio, 0);
}

//VER VIDEO SOCKETS DE UTN.SO PARTE 2 PARA ENTENDER ESTA FUNCION
void permitirReutilizar(int servidor, int* activado) {
	(*activado) = 1;
	setsockopt(servidor, SOL_SOCKET, SO_REUSEADDR, activado, sizeof(*activado));
}

char* recibirMensaje(int socketCliente, int tamanioMsj, int criterio) {
	char* buffer = malloc(tamanioMsj);
	//Criterio = 0 -> No-Bloqueante (ES DECIR QUE PUEDE AVANZAR DE LA FUNCION SIN RECIBIR UN MENSAJE)
	//Criterio = MSG_WAITALL -> Bloqueante (ES DECIR QUE NO AVANZA DE LA FUNCION HASTA QUE RECIBA UN MENSAJE)

	int bytesRecibidos = recv(socketCliente, buffer, tamanioMsj, criterio);
	if (bytesRecibidos <= 0) { //SI ESTO SE CUMPLE, EL CLIENTE SE DESCONECTO
		printf("\nEl Cliente %d se desconectó\n", socketCliente);
		quitarCliente(buscarCliente(socketCliente)); //QUITO EL CLIENTE DE LA LISTA
		perror("Causa"); //PERROR ME MUESTRA EL MOTIVO
		printf("\n"); //DEJO UN ESPACIO
		close(socketCliente);
	}
	buffer[tamanioMsj] = '\0';
	return buffer;
}

char* recibirMensajeSinEspera(int socketCliente, int tamanioMsj) {
	return recibirMensaje(socketCliente, tamanioMsj, 0); //NO BLOQUEANTE
}

char* recibirMensajeConEspera(int socketCliente, int tamanioMsj) {
	return recibirMensaje(socketCliente, tamanioMsj, MSG_WAITALL); //BLOQUEANTE
}

void hacerListen(int servidor) {
	listen(servidor, CANT_CONEXIONES); //DEFINIDO ARRIBA DE TODO
}

void hacerBindeo(int servidor, struct sockaddr_in* direccionServidor) {
	if (bind(servidor, (void*) direccionServidor, sizeof(*direccionServidor))
			!= 0) {
		perror("Fallo el bind");
		exit(1);
	}
}

void configurarServidor(struct sockaddr_in* direccion, int* socketServidor,
		unsigned short PUERTO, int* activado) {
	(*direccion) = crearDireccionServidor(PUERTO); //LOS TRATO COMO PUNTEROS PARA PODER MODIFICARLOS DE LA FUNCION QUE ME LO LLAMA
	(*socketServidor) = crearSocket();
	permitirReutilizar((*socketServidor), &activado);
	//Recordar regla memotecnica
	//BLAB: Bind-Listen-Accept-Begin to Communicate

	hacerBindeo((*socketServidor), direccion);
	hacerListen((*socketServidor));
}

int agregarCliente(t_cliente Cliente) {
	// AGREGO UN CLIENTE EN LA PRIMERA POSICION LIBRE DEL ARRAY Y RETORNO LA POSICION DONDE LO AGREGUE
	int i = 0;
	for (i = 0; i < MAX_CLIENTES; i++) {
		if (Conexiones.ListaClientes[i].socket == 0) {
			Conexiones.ListaClientes[i].socket = Cliente.socket;
			Conexiones.ListaClientes[i].direccion = Cliente.direccion;
			printf("El Cliente: Socket %d fue agregado satisfactoriamente!\n",
					Cliente.socket);
			return i;
		}
	}
	printf("Se superó el numero máximo de clientes!\n");

	close(Cliente.socket);
	return -1; //RETORNO ESTO EN CASO DE QUE ESTE LLENO
}

int agregarServidor(t_cliente Servidor) {
	//AGREGO UN SERVIDOR EN LA PRIMERA POSICION LIBRE DEL ARRAY Y RETORNO LA POSICION DONDE LO AGREGUE
	int i = 0;
	for (i = 0; i < MAX_SERVIDORES; i++) {
		if (Conexiones.ListaServidores[i].socket == 0
				&& buscarCliente(Servidor.socket) == -1) {
			Conexiones.ListaServidores[i].socket = Servidor.socket;
			Conexiones.ListaServidores[i].direccion = Servidor.direccion;
			Conexiones.ListaServidores[i].tipoProceso = Servidor.tipoProceso;
			printf("El Servidor: Socket %d fue agregado satisfactoriamente!\n",
					Servidor.socket);
			return i;
		}
	}
	printf("Se superó el numero máximo de servidores!\n");

	close(Servidor.socket);
	return -1; //RETORNO ESTO EN CASO DE QUE ESTE LLENO
}

void quitarCliente(int indice) {
	// Liberamos del array al cliente y cerramos su socket;
	printf("Cliente quitado , IP = %s , Puerto =  %d \n",
			inet_ntoa(Conexiones.ListaClientes[indice].direccion.sin_addr),
			ntohs(Conexiones.ListaClientes[indice].direccion.sin_port));
	close(Conexiones.ListaClientes[indice].socket);
	Conexiones.ListaClientes[indice].socket = 0;
}

int buscarCliente(int nroSocket) {
	int i;
	for (i = 0; i < MAX_CLIENTES; i++) {
		//SI EL NRO DE SOCKET Y EL TIPO DE PROCESO (0:KERNEL, 1:FS, 2:MEMORIA, 3:CPU, 4:CONSOLA) COINCIDEN, LO ENCONTRÉ
		//UN MISMO PROCESO NO PUEDE TENER LOS MISMOS SOCKETS, YA QUE SE CONECTA AL MISMO PUERTO Y NO SE PUEDEN REPETIR LOS SOCKETS EN UN MISMO PUERTO
		if (Conexiones.ListaClientes[i].socket == nroSocket) {
			return i;
		}
	}
	return -1; //NO EXISTE CLIENTE
}

int buscarServidor(int nroSocket) {
	int i;
	for (i = 0; i < MAX_SERVIDORES; i++) {
		//SI EL NRO DE SOCKET Y EL TIPO DE PROCESO (0:KERNEL, 1:FS, 2:MEMORIA, 3:CPU, 4:CONSOLA) COINCIDEN, LO ENCONTRÉ
		//UN MISMO PROCESO NO PUEDE TENER LOS MISMOS SOCKETS, YA QUE SE CONECTA AL MISMO PUERTO Y NO SE PUEDEN REPETIR LOS SOCKETS EN UN MISMO PUERTO
		if (Conexiones.ListaServidores[i].socket == nroSocket) {
			return i;
		}
	}
	return -1; //NO EXISTE SERVIDOR
}

int hacerAccept(int socketServidor) {
	t_cliente Cliente;
	int socketNuevoCliente;
	struct sockaddr_in direccionCliente;
	unsigned int tamanioDireccion = sizeof(direccionCliente);

	socketNuevoCliente = accept(socketServidor,
			(struct sockaddr *) &direccionCliente, &tamanioDireccion);

	Cliente.socket = socketNuevoCliente;
	Cliente.direccion = direccionCliente;

	printf("\nSe conectó Cliente: Socket = %d | IP = %s | Puerto = %d \n",
			Cliente.socket, inet_ntoa(Cliente.direccion.sin_addr),
			ntohs(Cliente.direccion.sin_port));

	agregarCliente(Cliente);

	return Cliente.socket;
}

void quedarEsperaRecibirMensajes(int socketCliente, t_handshake quienReciboMsjs,
		t_handshake quienSoy) {
	while (1) {
		char* msj = recibirMensajeConTamanioDinamico(socketCliente);
		if (msj != "ERROR") { //SI EL CLIENTE NO SE DESCONECTO
			char* nameProceso = nombreProceso(quienReciboMsjs);
			printf("%s socket %d dice =  %s\n", nameProceso, socketCliente,
					msj);
		} else {
			break;
		}
	}
}

void recibirMensajesYReplicar(int socketCliente) {
	while (1) {
		char* msj = recibirMensajeConTamanioDinamico(socketCliente);
		if (msj != "ERROR") { //SI EL CLIENTE NO SE DESCONECTO
			int posCliente = buscarCliente(socketCliente);
			char* nameProceso = nombreProceso(
					Conexiones.ListaClientes[posCliente].tipoProceso);
			printf("%s socket %d dice =  %s\n", nameProceso, socketCliente,
					msj);
			replicarMensajeClientes(CPU, msj);
			replicarMensajeServidores(FILE_SYSTEM, msj);
			replicarMensajeServidores(MEMORIA, msj);
		} else {
			break;
		}
	}
}
int recibirCliente(int socketServidor) {
	int socketCliente = hacerAccept(socketServidor);
	return socketCliente;
}

int getHandshake(int socketAConectar) {

	char* headerHandshake = recibirMensajeConEspera(socketAConectar,
			sizeof(int));
	if (stringToInt(headerHandshake) == HEADER_HANDSHAKE) { //RECIBO HEADER, COMO ES 0 ENTONCES ES UN HANDSHAKE
		return stringToInt(recibirMensajeConEspera(socketAConectar, 1)); //RECIBO EL HANDSHAKE (0|KERNEL, 1|FILESYSTEM, 2|MEMORIA, 3|CPU, 4|CONSOLA)

	} else {
		return -1; //RETORNO ERROR
	}
}

void enviarHeader(int socketCliente, int header) {
	char* head = intToString(header);
	enviarMensaje(socketCliente, head, sizeof(int));
}
int recibirHeader(int socketCliente) {
	return stringToInt(recibirMensajeConEspera(socketCliente, sizeof(int)));
}

void iniciarHandshake(t_handshake quienIniciaHandshake,
		t_handshake quienDevuelveHandshake, int socket) {
	enviarHeader(socket, HEADER_HANDSHAKE); //ME ARMO EL HEADER DE QUE VA SER UN HANSHAKE

	char* handshake = intToString(quienIniciaHandshake); //ME TRANSFORMO A STRING ESE HANDSHAKE
	enviarMensaje(socket, handshake, 1);

	if (getHandshake(socket) == quienDevuelveHandshake) { //EL GET HANDSHAKE SE QUEDA A LA ESPERA DE QUE LE DEVUELVAN EL HANDSHAKE
		printf("Se conecto satisfactoriamente!\n");
		Conexiones.ListaServidores[buscarServidor(socket)].tipoProceso =
				quienDevuelveHandshake;
	}
}

void devolverHandshake(int socketCliente, t_handshake quienSoyYo) {
	int posicionCliente = buscarCliente(socketCliente);
	int handshake = getHandshake(socketCliente);

	if (handshake == CONSOLA) {
		printf("Se conectó una Consola!\n");
		Conexiones.ListaClientes[posicionCliente].tipoProceso = CONSOLA;
	} else if (handshake == CPU) {
		printf("Se conectó una CPU!\n");
		Conexiones.ListaClientes[posicionCliente].tipoProceso = CPU;
	} else if (handshake == KERNEL) {
		printf("Se conectó un Kernel!\n");
		Conexiones.ListaClientes[posicionCliente].tipoProceso = KERNEL;
	}

	enviarHeader(socketCliente, HEADER_HANDSHAKE);
	enviarMensaje(socketCliente, intToString(quienSoyYo), 1);
}

void enviarMensajeConTamanioDinamico(int socketCliente, char* msj) {
	char* longitudMensaje = string_new();
	longitudMensaje = intToString(strlen(msj));
	enviarMensaje(socketCliente, longitudMensaje, sizeof(int)); //LE ENVIO PRIMERO EL TAMAÑO DEL MENSAJE A ENVIAR
	enviarMensaje(socketCliente, msj, strlen(msj)); //LE ENVIO EL MENSAJE DE ESE TAMAÑO
}

int recibirTamanioMensaje(int socketCliente) {
	return stringToInt(recibirMensajeConEspera(socketCliente, sizeof(int)));
}

char* recibirMensajeConTamanioDinamico(int socket) {
	int tamanioProxMsj = recibirTamanioMensaje(socket);

	char* msj = malloc(tamanioProxMsj);
	if (tamanioProxMsj != 0) {
		return recibirMensajeConEspera(socket, tamanioProxMsj);
		return msj;
	}
	return "ERROR";
}

int stringToInt(char* string) {
	return atoi(string);
}

char* intToString(int nro) {
	return string_from_format("%d", nro);
}

void inicializarClientes() {
	// Inicializamos en desconectado = 0
	int i;
	for (i = 0; i < MAX_CLIENTES; i++) {
		Conexiones.ListaClientes[i].socket = 0;
	}
}

void inicializarServidores() {
	// Inicializamos en desconectado = 0
	int i;
	for (i = 0; i < MAX_SERVIDORES; i++) {
		Conexiones.ListaServidores[i].socket = 0;
	}
}

void mostrarClientes() {
	int i;
	for (i = 0; i < MAX_CLIENTES; i++) {
		if (Conexiones.ListaClientes[i].socket != 0) {
			printf("%s ",
					nombreProceso(Conexiones.ListaClientes[i].tipoProceso));
			printf("Socket = %d\n", Conexiones.ListaClientes[i].socket);
		}
	}
}

int cantidadClientes() {
	int i, cant = 0;
	for (i = 0; i < MAX_CLIENTES; i++) {
		if (Conexiones.ListaClientes[i].socket != 0) {
			cant++;
		}
	}
	return cant;
}

int cantidadServidores() {
	int i, cant = 0;
	for (i = 0; i < MAX_SERVIDORES; i++) {
		if (Conexiones.ListaServidores[i].socket != 0) {
			cant++;
		}
	}
	return cant;
}

char* nombreProceso(int tipoProceso) {
	if (tipoProceso == 0) {
		return ("Kernel");
	}
	if (tipoProceso == 1) {
		return ("File System");
	}
	if (tipoProceso == 2) {
		return ("Memoria");
	}
	if (tipoProceso == 3) {
		return ("CPU");
	}
	if (tipoProceso == 4) {
		return ("Consola");
	} else {
		return ("No existe el proceso");
	}
}

void replicarMensajeClientes(int idProcesoAReplicar, char* msj) {
	int i;
	for (i = 0; i < MAX_CLIENTES; i++) {
		if (Conexiones.ListaClientes[i].tipoProceso == idProcesoAReplicar
				&& Conexiones.ListaClientes[i].socket != 0) {
			enviarMensajeConTamanioDinamico(Conexiones.ListaClientes[i].socket,
					msj);
		}
	}
}

void replicarMensajeServidores(int idProcesoAReplicar, char* msj) {
	int i;
	for (i = 0; i < MAX_SERVIDORES; i++) {
		if (Conexiones.ListaServidores[i].tipoProceso == idProcesoAReplicar
				&& Conexiones.ListaServidores[i].socket != 0) {
			enviarMensajeConTamanioDinamico(
					Conexiones.ListaServidores[i].socket, msj);
		}
	}
}
