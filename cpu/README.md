## Proceso CPU
Este proceso es el encargado de interpretar y ejecutar las operaciones escritas en código AnSISOP de
un Programa.
Estará en permanente contacto con el Proceso Memoria, tanto para obtener información del
Programa en ejecución, como para actualizar las estructuras requeridas luego de ejecutar una
operación.
Al iniciar, se conectará al Proceso Kernel y quedará a la espera de que este le envíe el PCB de un
Programa para ejecutarlo.
Incrementará el valor del registro Program Counter del PCB y utilizará el índice de código para
solicitar a la Memoria la próxima sentencia a ejecutar. Al recibirla, la parseará, ejecutará las
operaciones requeridas, actualizará los valores del Programa en la Memoria, actualizará el Program
Counter en el PCB y notificará al Kernel que concluyó su ejecución.