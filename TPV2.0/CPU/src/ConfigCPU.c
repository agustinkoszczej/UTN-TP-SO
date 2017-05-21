/*
 * ConfigCPU.c
 *
 *
 *      Author: utnso
 */

#include "ConfigCPU.h"
#include "CPU.h"

void cargarConfigCPU() {
  t_config* configCPU;
  configCPU = config_create(RUTA_CFG);

  cpu_config.IP_KERNEL_CPU = config_get_string_value(configCPU,
      "IP_KERNEL");
  cpu_config.PUERTO_KERNEL_CPU = config_get_int_value(configCPU,
      "PUERTO_KERNEL");

  printf("Archivo de configuracion de Consola cargado exitosamente!\n");
}

void mostrarConfigCPU() {
  printf("IP_KERNEL=%s\n", cpu_config.IP_KERNEL_CPU);
  printf("PUERTO_KERNEL=%d\n", cpu_config.PUERTO_KERNEL_CPU);
}
