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

  cpu_config.IP_KERNEL = config_get_string_value(configCPU,
      "IP_KERNEL");
  cpu_config.PUERTO_KERNEL = config_get_int_value(configCPU,
      "PUERTO_KERNEL");

  printf("Archivo de configuracion de Consola cargado exitosamente!\n");
  logger("Archivo de configuracion de Consola cargado exitosamente", "INFO", NOMBRE_PROCESO);
}

void mostrarConfigCPU() {
  printf("IP_KERNEL=%s\n", cpu_config.IP_KERNEL);
  printf("PUERTO_KERNEL=%d\n", cpu_config.PUERTO_KERNEL);
}
