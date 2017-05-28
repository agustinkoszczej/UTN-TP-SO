#ifndef PCB_H_
#define PCB_H_

#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>

typedef struct {
	int cycles;
	int op_priviliges;
} t_statistics;

typedef struct {
	int id;
	int pag;
	int off;
	int size;
} t_arg_var;

typedef struct {
	int pag;
	int off;
	int size;
} t_retvar;

typedef struct {
	t_arg_var* args;
	t_list* vars;
	int* retpos;
	t_retvar* retvar;
} t_stack;

typedef struct {
	int pid;
	int pc;
	int page_c;
	t_list* i_code;
	t_dictionary* i_label;
	t_list* i_stack;
	int state;
	t_statistics statistics;
	int exit_code;
} pcb;

typedef struct {
	int offset;
	int length;
} t_i_code;

char* pcb_to_string(pcb* _pcb);

#endif /* PCB_H_ */
