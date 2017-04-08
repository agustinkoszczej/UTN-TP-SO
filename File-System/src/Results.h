/*
 * Results.h
 *
 *  Created on: 8/4/2017
 *      Author: utnso
 */

#ifndef RESULTS_H_
#define RESULTS_H_

#include <stdbool.h>

typedef struct{
	bool noError;
	char msg[100];
} Result;

typedef struct{
	Result result;
	void* value;
} ResultWithValue;

Result Ok();

Result Error(char msg[]);

ResultWithValue OkWithValue(void* value);

ResultWithValue ErrorWithValue(char msg[], void* value);

#endif /* RESULTS_H_ */
