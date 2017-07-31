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

### Hot plug
En cualquier momento de la ejecución del sistema pueden conectarse nuevas instancias del proceso
CPU. Será responsabilidad del Kernel aceptar esas nuevas conexiones, y tener en cuenta a las nuevas
CPUs a la hora de planificar.
Mediante la señal SIGUSR1, se le podrá notificar a un CPU que deberá desconectarse una vez
concluida la ejecución del Programa actual, dejando de dar servicio al sistema.

### Fin de la ejecución
Al ejecutar la última sentencia, el CPU deberá notificar al Kernel que el proceso finalizó para que este
se ocupe de solicitar la eliminación de las estructuras utilizadas por el sistema.

### Excepciones
Existe la posibilidad que el Proceso CPU reciba un mensaje de excepción como resultado de una
solicitud a la Memoria o al Kernel. En este caso la CPU deberá mostrar por pantalla un mensaje
aclarando cuál fue el inconveniente que sucedió y como consecuencia del error se deberá finalizar el
programa AnSISOP que se encuentre en ejecución.
