/*
 * Results.c
 *
 *  Created on: 8/4/2017
 *      Author: utnso
 */

#include <stdbool.h>
#include "Results.h"

Result Ok(){
	Result result;
	result.noError = true;
	strcpy(result.msg,"Ok");

	return result;
}

Result Error(char msg[]){
	Result result;
	result.noError = false;
	strcpy(result.msg, msg);

	return result;
}

ResultWithValue OkWithValue(void* value){
	ResultWithValue result;
	result.result = Ok();
	result.value = value;

	return result;
}

ResultWithValue ErrorWithValue(char msg[], void* value){
	ResultWithValue result;
	result.result = Error(msg);
	result.value = value;

	return result;
}
