# **Sistemas Operativos UTN FRBA - TP 2017 1C:**
# **Lords of the Strings**
 
![GitHub ESTher](/ESTher.png)
 
## **ESTheR: El Stack The Revenge**
El trabajo práctico consiste en simular ciertos aspectos de un sistema multiprocesador, con la
capacidad de interpretar la ejecución de scripts escritos en un lenguaje diseñado para el trabajo
práctico. Este sistema planificará y ejecutará estos scripts (en adelante “Programas”) controlando
sus solicitudes de memoria, administrando los accesos a recursos tanto propios como compartidos.
Los scripts utilizados en el trabajo práctico estarán escritos en el lenguaje AnSISOP, el cual fue
inventado y diseñado por la cátedra para fines didácticos.

## **Arquitectura del sistema**
El sistema simulará un sistema distribuido para el procesamiento de los Programas escritos en
lenguaje de scripting AnSISOP. El mismo contará con un proceso central o Kernel, el cual se
encargará de la planificación de dichos programas en la diferentes CPUs.
La información relacionada al contexto de ejecución de cada programa y las regiones dedicadas al
sistema operativo para su gestión se almacenarán en un proceso denominado Memoria, que simula
la memoria principal de una computadora.
Un proceso llamado Filesystem otorgará al sistema la capacidad de almacenar y gestionar datos de
manera persistente.
Por último los procesos consola serán los encargados de enviar a ejecutar los programas AnSISOP al
Kernel.

![GitHub SystemArchitecture](/SystemArchitecture.png)

## **Integrantes:**

| Apellido | Nombre | Github |
| -------- | -------- | -------- |
| Koszczej | Agustín | [agustinkoszczej](https://github.com/agustinkoszczej) |
| Plawner | Sebastián | [sebaplawner](https://github.com/sebaplawner) |

## **Procesos:**

* [Consola](https://github.com/agustinkoszczej/UTN-TP-SO/tree/master/consola)
* [CPU](https://github.com/agustinkoszczej/UTN-TP-SO/tree/master/cpu)
* [File System](https://github.com/agustinkoszczej/UTN-TP-SO/tree/master/filesystem)
* [Kernel](https://github.com/agustinkoszczej/UTN-TP-SO/tree/master/kernel)
* [Memoria](https://github.com/agustinkoszczej/UTN-TP-SO/tree/master/memoria)

## **Checkpoints:**

* ~~Checkpoint 1 - Obligatorio~~
  * ~~Fecha: 15/04/2017~~
  
* ~~Checkpoint 2~~
  * ~~Fecha: 06/05/2017~~
  
* ~~Checkpoint 3~~
  * ~~Fecha: 20/05/2017~~
  
* ~~Checkpoint 4 - Obligatorio (En Laboratorio)~~
  * ~~Fecha: 03/06/2017~~
  
* ~~Checkpoint 5~~
  * ~~Fecha: 24/06/2017~~
  
* Entrega Final
  * ~~Fecha: 08/07/2017~~
    * ~~Fecha de primer recuperatorio: 15/07/2017~~
    
    * ~~Fecha de segundo recuperatorio: 29/07/2017~~

## **Instalación:**
#### 1. Crear la carpeta workspace si no existe

    mkdir workspace   
    cd workspace
    
#### 2. Clonar nuestro repositorio [Lords-of-the-Strings](https://github.com/sisoputnfrba/tp-2017-1c-Lords-of-the-Strings)

    git clone https://github.com/sisoputnfrba/tp-2017-1c-Lords-of-the-Strings
    
#### 3. Clonar e instalar las [so-commons-library](https://github.com/sisoputnfrba/so-commons-library/tree/1dd52d058e5c468f7dbe83ad8579a059d5c5ff38) y el [ansisop-parser](https://github.com/sisoputnfrba/ansisop-parser/tree/f390532f020c3b158932df8e745c75fa0ca10764)

     git submodule init 
     git submodule update
        
#### 4. Compilar todos los módulos

     sh compile.sh
    
#### 5. Importar para cada módulo las [librerías comunes](https://github.com/agustinkoszczej/UTN-TP-SO/tree/master/utils)

     source import_libraries.sh

#### 6. Ejecutar módulo

     cd NombreMódulo
     Debug/./NombreMódulo RutaConfig
    
*RutaConfig: `config` por default*

#### Importante:
Recordar que algunos módulos requieren que otros estén en ejecución para iniciar, este es el órden para arrancar los módulos:
      
     1. Memoria/File System
     2. Kernel
     3. CPU/s
     4. Consola/s

### **Comandos Útiles**

`export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/workspace/tp-2017-1c-Lords-of-the-Strings/utils/Debug` 
    
`sudo chmod +x path.sh`
   
`source path.sh`
    
`ifconfig`
    
## **Github:**
* https://github.com/sisoputnfrba/tp-2017-1c-Lords-of-the-Strings
