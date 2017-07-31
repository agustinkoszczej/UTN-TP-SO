## Proceso Memoria
El Proceso Memoria es el responsable en el sistema de brindar a los Programas espacio en memoria
para que estos realicen sus operaciones. Para ello simulará un mecanismo de tabla de páginas
invertida y la utilización de una Memoria Caché. Es importante tener en cuenta que la memoria
deberá tener una serie de estructuras administrativas las cuales deberán estar administradas dentro
del mismo espacio de direcciones que los datos de los usuarios.

### Arquitectura de la Memoria
Al iniciar, solicitará un bloque de memoria contigua de tamaño configurable por archivo de
configuración, para simular la memoria principal del sistema. Luego creará las estructuras
administrativas necesarias para poder gestionar dicho espacio, permitiendo a cada Programa en
funcionamiento utilizar un conjunto de páginas de tamaño fijo.

A la Memoria se conectarán, mediante sockets, el proceso Kernel y los diversos CPUs. Por cada
conexión, la Memoria creará un hilo dedicado a atenderlo, que quedará a la espera de solicitudes de
operaciones. La Memoria deberá validar cada pedido recibido, y responder en consecuencia.
* Ante cualquier solicitud de acceso a la memoria principal, el Proceso Memoria deberá
esperar una cantidad de tiempo configurable (en milisegundos), simulando el tiempo de
acceso a memoria. En caso de que la solicitud sea resuelta por la Memoria Caché, no se
deberá esperar.
* Es importante destacar la naturaleza multi-hilo del proceso, por lo que será parte del
desarrollo del trabajo atacar los problemas de concurrencia que surjan.
* A modo de simular el funcionamiento de un Sistema Operativo real, las estructuras
administrativas de la Memoria deberán estar contenidas dentro del espacio del bloque de
memoria contigua.
* En caso de que un proceso solicite memoria y no se le pueda asignar por falta de espacio, se
deberá informar al proceso que lo solicite (CPU o Kernel) de que no hay más espacio
disponible.

### Diagrama de páginas de la memoria:
Las estructuras Administrativas deben guardarse en los primeros N bloques de la memoria, mientras
que los bloques de los procesos se deberán ir asignando a medida que sean necesarios, pudiendo
quedar de forma continua o no como se ve en el siguiente diagrama:

![GitHub MemoryPages](/memoria/MemoryPages.png)

Para hacer que las estructuras administrativas ocupen la menor cantidad posible de espacio en el
sistema se utilizará una tabla de páginas invertida, y se recomienda la siguiente estructura de datos
pudiendo alterarse la misma según las necesidades de cada grupo siempre buscando reducir el
tamaño de la estructura a su mínimo tamaño:

| #Frame | PID | #Pag |
| -------- | -------- | -------- |
| 0 | -1 | 0 |
| 1 | -1 | 1 |
| ... | ... | ... |
| 21 | 1 | 0 |
| 22 | 1 | 1 |
| 23 | 1 | 2 |
| 2 | 4 | 1 | 3 |

### Frame Lookup
Potencialmente la tabla de páginas podría llegar a tener una cantidad muy alta de entradas,
haciendo de la búsqueda secuencial un recurso muy lento. Para subsanar dicho inconveniente, se
suele utilizar una Función de Hashing para encontrar rápidamente un subset de la tabla que
contenga el valor a buscar.

### Memoria Caché
El proceso Memoria utilizará una estructura en que emulará una única caché del sistema, la cual
permitirá tener un acceso más rápido a los datos de la memoria. A modo de simplificar el desarrollo
de la Caché, la misma será una estructura de tamaño configurable por archivo de configuración, así
como también la cantidad máxima de entradas que un proceso podrá tener en la misma (pudiendo
no alcanzarse el máximo establecido por proceso, hasta incluso no tener ninguna entrada por
encontrarse la caché totalmente llena). La caché deberá eliminarlas páginas cacheadas de un
proceso cuando este sea finalizado.
La estructura de las entradas de la caché deberá ser la siguiente
* Identificador del Proceso
* Número de Página
* Contenido de la Página
Las entradas en la memoria caché se deberán reemplazar siguiendo el algoritmo LRU. El alcance del
reemplazo es global, es decir, un proceso puede quitarle páginas a otro en caso de ser necesario.
