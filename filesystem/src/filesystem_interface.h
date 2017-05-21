#ifndef FILESYSTEM_INTERFACE_H_
#define FILESYSTEM_INTERFACE_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <socket.h>
#include "filesystem.h"

int k_socket;

void newClient(socket_connection * connection);
void connectionClosed(socket_connection * connection);

bool validateFile(char filePath[]);
bool createFile(char filePath[]);
bool deleteFile(char filePath[]);
char* getData(char filePath[],int offSet,int size);
bool saveData(char filePath[],int offSet,int size,int buffer);

#endif /* FILESYSTEM_INTERFACE_H_ */
