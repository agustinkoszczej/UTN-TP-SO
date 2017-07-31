#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/string.h>
#include <console.h>

#define CONFIG_CONSOLA "/home/utnso/workspace/tp-2017-1c-Lords-of-the-Strings/consola/config"
#define CONFIG_CPU "/home/utnso/workspace/tp-2017-1c-Lords-of-the-Strings/cpu/config"
#define CONFIG_FILESYSTEM "/home/utnso/workspace/tp-2017-1c-Lords-of-the-Strings/filesystem/config"
#define CONFIG_KERNEL "/home/utnso/workspace/tp-2017-1c-Lords-of-the-Strings/kernel/config"
#define CONFIG_MEMORIA "/home/utnso/workspace/tp-2017-1c-Lords-of-the-Strings/memoria/config"

char ip_vm1[100];
char ip_vm2[100];
char ip_vm3[100];
char ip_vm4[100];

int option;

char* ALGORITMO;
char* QUANTUM;
char* QUANTUM_SLEEP;
char* GRADO_MULTIPROG;
char* SEM_IDS;
char* SEM_INIT;
char* STACK_SIZE;
char* SHARED_VARS;

char* MARCOS;
char* MARCO_SIZE;
char* ENTRADAS_CACHE;
char* CACHE_X_PROC;
char* RETARDO_MEMORIA;

void consola() {
	remove(CONFIG_CONSOLA);
	FILE* f = fopen(CONFIG_CONSOLA, "w");

	fprintf(f, "IP_KERNEL=%s", ip_vm1);
	fprintf(f, "PUERTO_KERNEL=5000");

	fclose(f);
}

void cpu() {
	remove(CONFIG_CPU);
	FILE* f = fopen(CONFIG_CPU, "w");

	fprintf(f, "IP_KERNEL=%s", ip_vm1);
	fprintf(f, "PUERTO_KERNEL=5001\n");
	fprintf(f, "IP_MEMORIA=%s", ip_vm4);
	fprintf(f, "PUERTO_MEMORIA=5002");

	fclose(f);
}

void filesystem() {
	remove(CONFIG_FILESYSTEM);
	FILE* f = fopen(CONFIG_FILESYSTEM, "w");

	fprintf(f, "PORT=5003\n");
	fprintf(f, "MOUNT_POINT=/mnt/Lords-of-the-Strings");

	fclose(f);
}

void kernel() {
	remove(CONFIG_KERNEL);
	FILE* f = fopen(CONFIG_KERNEL, "w");

	fprintf(f, "PUERTO_PROG=5000\n");
	fprintf(f, "PUERTO_CPU=5001\n");
	fprintf(f, "IP_MEMORIA=%s", ip_vm4);
	fprintf(f, "PUERTO_MEMORIA=5002\n");
	fprintf(f, "IP_FS=%s", ip_vm3);
	fprintf(f, "PUERTO_FS=5003\n");
	fprintf(f, "QUANTUM=%s\n", QUANTUM);
	fprintf(f, "QUANTUM_SLEEP=%s\n", QUANTUM_SLEEP);
	fprintf(f, "ALGORITMO=%s\n", ALGORITMO);
	fprintf(f, "GRADO_MULTIPROG=%s\n", GRADO_MULTIPROG);
	fprintf(f, "SEM_IDS=%s\n", SEM_IDS);
	fprintf(f, "SEM_INIT=%s\n", SEM_INIT);
	fprintf(f, "SHARED_VARS=%s\n", SHARED_VARS);
	fprintf(f, "STACK_SIZE=%s", STACK_SIZE);

	fclose(f);
}

void memoria() {
	remove(CONFIG_MEMORIA);
	FILE* f = fopen(CONFIG_MEMORIA, "w");

	fprintf(f, "PUERTO=5002\n");
	fprintf(f, "MARCOS=%s\n", MARCOS);
	fprintf(f, "MARCO_SIZE=%s\n", MARCO_SIZE);
	fprintf(f, "ENTRADAS_CACHE=%s\n", ENTRADAS_CACHE);
	fprintf(f, "CACHE_X_PROC=%s\n", CACHE_X_PROC);
	fprintf(f, "RETARDO_MEMORIA=%s", RETARDO_MEMORIA);

	fclose(f);
}

int main(int argc, char *argv[]) {
	clear_screen();

	printf("IP VM1: ");
	fgets(ip_vm1, 100, stdin);
	printf("IP VM2: ");
	fgets(ip_vm2, 100, stdin);
	printf("IP VM3: ");
	fgets(ip_vm3, 100, stdin);
	printf("IP VM4: ");
	fgets(ip_vm4, 100, stdin);

	println("\n-----\n");
	println("1. Prueba Base");
	println("2. Prueba Completa");
	println("3. Prueba File System");
	println("4. Prueba Heap y Memoria");
	println("5. Prueba Estrés\n");

	remove(CONFIG_CPU);
	remove(CONFIG_FILESYSTEM);
	remove(CONFIG_KERNEL);
	remove(CONFIG_MEMORIA);

	char sel[255];
	fgets(sel, 255, stdin);
	strtok(sel, "\n");
	option = atoi(sel);

	switch (option) {
		case 1:
			ALGORITMO = string_from_format("FIFO");
			QUANTUM = string_from_format("4");
			QUANTUM_SLEEP = string_from_format("500");
			GRADO_MULTIPROG = string_from_format("5");
			SEM_IDS = string_from_format("[mutexArch, b]");
			SEM_INIT = string_from_format("[0, 5]");
			STACK_SIZE = string_from_format("2");
			SHARED_VARS = string_from_format("[!pasadas]");

			MARCOS = string_from_format("500");
			MARCO_SIZE = string_from_format("256");
			ENTRADAS_CACHE = string_from_format("18");
			CACHE_X_PROC = string_from_format("0");
			RETARDO_MEMORIA = string_from_format("100");
			break;
		case 2:
			ALGORITMO = string_from_format("RR");
			QUANTUM = string_from_format("4");
			QUANTUM_SLEEP = string_from_format("500");
			GRADO_MULTIPROG = string_from_format("3");
			SEM_IDS = string_from_format("[m, b, c]");
			SEM_INIT = string_from_format("[1, 1, 0]");
			STACK_SIZE = string_from_format("2");
			SHARED_VARS = string_from_format("[!pasadas, !colas]");

			MARCOS = string_from_format("500");
			MARCO_SIZE = string_from_format("256");
			ENTRADAS_CACHE = string_from_format("18");
			CACHE_X_PROC = string_from_format("3");
			RETARDO_MEMORIA = string_from_format("100");
			break;
		case 3:
			ALGORITMO = string_from_format("RR");
			QUANTUM = string_from_format("4");
			QUANTUM_SLEEP = string_from_format("500");
			GRADO_MULTIPROG = string_from_format("3");
			SEM_IDS = string_from_format("[m, b, c]");
			SEM_INIT = string_from_format("[1, 1, 0]");
			STACK_SIZE = string_from_format("2");
			SHARED_VARS = string_from_format("[!pasadas, !colas]");

			MARCOS = string_from_format("100");
			MARCO_SIZE = string_from_format("256");
			ENTRADAS_CACHE = string_from_format("18");
			CACHE_X_PROC = string_from_format("3");
			RETARDO_MEMORIA = string_from_format("500");
			break;
		case 4:
			ALGORITMO = string_from_format("RR");
			QUANTUM = string_from_format("4");
			QUANTUM_SLEEP = string_from_format("500");
			GRADO_MULTIPROG = string_from_format("5");
			SEM_IDS = string_from_format("[m, b, c]");
			SEM_INIT = string_from_format("[1, 1, 0]");
			STACK_SIZE = string_from_format("2");
			SHARED_VARS = string_from_format("[!pasadas, !colas]");

			MARCOS = string_from_format("100");
			MARCO_SIZE = string_from_format("256");
			ENTRADAS_CACHE = string_from_format("12");
			CACHE_X_PROC = string_from_format("3");
			RETARDO_MEMORIA = string_from_format("500");
			break;
		case 5:
			ALGORITMO = string_from_format("RR");
			QUANTUM = string_from_format("4");
			QUANTUM_SLEEP = string_from_format("100");
			GRADO_MULTIPROG = string_from_format("3");
			SEM_IDS = string_from_format("[m, b, c]");
			SEM_INIT = string_from_format("[1, 1, 0]");
			STACK_SIZE = string_from_format("2");
			SHARED_VARS = string_from_format("[!pasadas, !colas]");

			MARCOS = string_from_format("500");
			MARCO_SIZE = string_from_format("256");
			ENTRADAS_CACHE = string_from_format("18");
			CACHE_X_PROC = string_from_format("3");
			RETARDO_MEMORIA = string_from_format("50");
			break;
		default:
			break;
	}

	consola();
	cpu();
	filesystem();
	kernel();
	memoria();

	clear_screen();
	println("CONFIGS CREADOS EXITOSAMENTE.");

	return 0;
}
