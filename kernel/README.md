## Proceso Kernel
El proceso Kernel es el proceso principal del sistema. Recibirá los programas, almacenará los scripts
AnSISOP en la Memoria y planificará su ejecución en los distintos procesos CPUs del sistema según
alguno de los algoritmos definidos. Además, será el encargado de resolver las llamadas a sistema
(syscalls) realizadas por los programas.

### Arquitectura del Proceso Kernel
El proceso Kernel al ser iniciado se conectará con el proceso Memoria y el proceso FS, obtendrá el
tamaño de página de la memoria y quedará a la espera de conexiones por parte de las CPUs y/o
Consolas.
Al contar con al menos un Proceso CPU, comenzará a planificar los diversos programas en función
del algoritmo de planificación.
* En cualquier momento, instancias de los Procesos CPU y Procesos Consola pueden ingresar o
desconectarse del sistema, debiendo el Kernel responder de forma favorable e informar del
cambio en la configuración del sistema.
* La falta de Procesos CPU en el sistema es posible. En ese caso, los programas quedarán a la
espera de una CPU para ser planificados.
* Es posible que mediante la consola del Kernel se solicite la finalización de un programa. En
ese caso, se deberán realizar las acciones solicitadas con el PCB correspondiente, cargando
el código de error en el campo Exit Code, e informando al Proceso Consola correspondiente.

### Creación del PCB
Al recibir la conexión de una nueva Consola, el Kernel intercambiará unos mensajes iniciales con la
misma (handshake), para luego recibir la totalidad del código fuente del script que se deberá
ejecutar.
El Kernel creará la estructura PCB con al menos los siguientes campos:
* Un identificador único (PID), el cual deberá informar a la consola.
* Program Counter (PC)
* Referencia a la tabla de Archivos del Proceso
* Posición del Stack (SP)
* El Exit Code (EC) del proceso

A partir de esta información, el Proceso Kernel deberá solicitarle al Proceso Memoria que le asigne
lás páginas necesarias para almacenar el código del programa y el stack. Para simular el
comportamiento de un sistema real, también le enviará el código completo del Programa.
Si no se pudiera obtener espacio suficiente para algunas de las estructuras necesarias del proceso,
entonces se le rechazará el acceso al sistema, informándose oportunamente en el Proceso Consola
correspondiente el motivo por el cual no pudo aceptarse el programa en el sistema.

### Planificación de Procesos
El sistema planificará los procesos según el siguiente esquema:

![GitHub ProcessScheduler](/kernel/ProcessScheduler.png)

Algunos de los aspectos principales del esquema a tener en cuenta son:
* Los procesos no se crearán en la memoria hasta que no pasen del estado NEW al estado
READY. Esto quedará determinado por el grado de multiprogramación del sistema.
* Los PCB que lleguen a la cola de EXIT se quedarán en dicha cola para poder mantener una
traza de ejecución del sistema.
* Es posible que en algún momento de la ejecución el sistema no cuente con CPUs conectadas,
con lo cual no será posible tener procesos en el estado EXEC.
* Cualquier programa podrá ser finalizado desde la consola del Kernel

#### *Algoritmos de Planificación*
El sistema utilizará los algoritmos configurables FIFO y Round Robin. Para este último, debe ser
configurable el valor del Quantum por archivo de configuración.

### System Calls - Operaciones Privilegiadas
Existen operaciones en el sistema que no pueden ser resueltas únicamente por el proceso CPU, ya
que estas requieren información privilegiada del sistema o acceso a elementos de la arquitectura
sobre las cuales tan solo el Kernel tiene permisos. Dichas instrucciones son denominadas System
Calls y deberán ser procesadas por el Kernel, a pedido expreso de una CPU.
Se registran dos módulos en el sistema que permiten operar System Calls: La Capa de Memoria,
encargada de crear y reservar memoria para el sistema, y administrar las variables compartidas; y la
Capa de FileSystem, encargada de proveer el acceso a archivos.

### Terminación irregular de procesos
Existe la posibilidad de que un proceso finalice su ejecución de forma irregular. Para poder
administrar la finalización de los procesos, el Kernel mantendrá en cada PCB un Exit Code que
identificará el motivo por el cuál finalizó dicho proceso.
El Exit Code es un valor entero signado, que en caso de ser positivo, representará una finalización
normal de un proceso. Por default, el valor de finalización de un programa que ejecutó todas sus
instrucciones es 0.
Por otro lado, en caso de una finalización irregular, el Exit Code de un proceso será un entero
negativo. El valor a guardar en el EC dependerá del motivo por el cual un programa finalizó
inesperadamente
