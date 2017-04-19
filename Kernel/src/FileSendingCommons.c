/*
 * FileSendingCommons.c
 *
 *  Created on: 18/4/2017
 *      Author: utnso
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include "SharedLibrary/Results.h"
#include "SharedLibrary/Sender.h"
#include "SharedLibrary/Conversor.h"
#include "SharedLibrary/Headers.h"

ResultWithValue GetFileDescriptor(char fileName[]){
	int fd = open(fileName, O_RDONLY);

	if (fd == -1)
	   return ErrorWithValue(strerror(errno), NULL);

	return OkWithValue(fd);
}

ResultWithValue GetFileLength(int fd){
   int fileSize;
   struct stat file_stat;

	if (fstat(fd, &file_stat) < 0)
		return ErrorWithValue(strerror(errno),NULL);

	fileSize = file_stat.st_size;

	return OkWithValue(fileSize);
}

ResultWithValue SendFileLength(int fd, int socketCliente){
	ResultWithValue r = GetFileLength(fd);

	if(r.result.noError){
		int fileLength = r.value;

		EnviarHeader(HEADER_TAMANIO);
		EnviarMensaje(socketCliente,intToString(fileLength),sizeof(int)); //NO ES INT, HAY QUE REVER

		return OkWithValue(fileLength);
	} else
		return r;
}

Result SendFileBytes(int fd, int fileSize, int socketCliente){
	int offset = 0;
	int remain_data = fileSize;
	int sent_bytes = 0;
	/* Sending file data */
	while (remain_data > 0)
	{
		if ((sent_bytes = sendfile(socketCliente, fd, &offset, BUFSIZ)) <= 0)
			return Error(strerror(errno));

			// fprintf(stdout, "1. Server sent %d bytes from file's data, offset is now : %d and remaining data = %d\n", sent_bytes, offset, remain_data);
			remain_data -= sent_bytes;
			// fprintf(stdout, "2. Server sent %d bytes from file's data, offset is now : %d and remaining data = %d\n", sent_bytes, offset, remain_data);
	}

	return Ok();
}

Result SendFile(char fileName[], int socketCliente){
	ResultWithValue r = GetFileDescriptor(fileName);

	if(r.result.noError){
		int fd = r.value;

		r = SendFileLength(fd, socketCliente);

		if(r.result.noError){
			r.result = SendFileBytes(fd,r.value,socketCliente);

		} else
			return r.result;

	} else
		return r.result;

	return r.result;
}
