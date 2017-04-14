/*
 * Multiplexor.h
 *
 *  Created on: 8/4/2017
 *      Author: utnso
 */

#ifndef MULTIPLEXOR_H_
#define MULTIPLEXOR_H_

void *get_in_addr(struct sockaddr *sa);


void AddClientToMaster(int fd);
void AddListenerToMaster(int fd);

Result Multiplexar(int listener);


#endif /* MULTIPLEXOR_H_ */
