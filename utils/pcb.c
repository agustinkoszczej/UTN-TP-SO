#include "pcb.h"

pcb* string_to_pcb(char* str) {
	pcb* n_pcb = malloc(sizeof(pcb));
	n_pcb->i_code = list_create();
	n_pcb->i_label = dictionary_create();
	n_pcb->i_stack = list_create();
	cJSON* root = cJSON_Parse(str);

	n_pcb->pid = cJSON_GetObjectItem(root, "pid")->valueint;
	n_pcb->pc = cJSON_GetObjectItem(root, "pc")->valueint;
	n_pcb->page_c = cJSON_GetObjectItem(root, "page_c")->valueint;
	n_pcb->exit_code = cJSON_GetObjectItem(root, "exit_code")->valueint;
	n_pcb->state = cJSON_GetObjectItem(root, "state")->valueint;
	n_pcb->statistics.cycles = cJSON_GetObjectItem(root, "cycles")->valueint;
	n_pcb->statistics.op_priviliges = cJSON_GetObjectItem(root, "op_priviliges")->valueint;

	cJSON* i_code_arr = cJSON_GetObjectItem(root, "i_code");
	cJSON* i_label_arr = cJSON_GetObjectItem(root, "i_label");
	cJSON* i_stack_arr = cJSON_GetObjectItem(root, "i_stack");

	int i;
	for (i = 0; i < cJSON_GetArraySize(i_code_arr); i++) {
		t_i_code* i_code = malloc(sizeof(t_i_code));
		cJSON* i_code_o = cJSON_GetArrayItem(i_code_arr, i);
		i_code->length = cJSON_GetObjectItem(i_code_o, "length")->valueint;
		i_code->offset = cJSON_GetObjectItem(i_code_o, "offset")->valueint;

		list_add(n_pcb->i_code, i_code);
	}

	for (i = 0; i < cJSON_GetArraySize(i_label_arr); i++) {
		cJSON* i_label_o = cJSON_GetArrayItem(i_label_arr, i);
		char* label = cJSON_GetObjectItem(i_label_o, "label")->valuestring;
		int* pos = malloc(sizeof(int));
		*pos = cJSON_GetObjectItem(i_label_o, "pos")->valueint;

		dictionary_put(n_pcb->i_label, label, pos);
	}

	for (i = 0; i < cJSON_GetArraySize(i_stack_arr); i++) {
		cJSON* i_stack_o = cJSON_GetArrayItem(i_stack_arr, i);
		t_stack* stack = malloc(sizeof(t_stack));

		if (cJSON_HasObjectItem(i_stack_o, "args")) {
			cJSON* args_o = cJSON_GetObjectItem(i_stack_o, "args");
			t_arg_var* args = malloc(sizeof(t_arg_var));
			args->id = cJSON_GetObjectItem(args_o, "id")->valueint;
			args->off = cJSON_GetObjectItem(args_o, "off")->valueint;
			args->pag = cJSON_GetObjectItem(args_o, "pag")->valueint;
			args->size = cJSON_GetObjectItem(args_o, "size")->valueint;
			stack->args = args;
		} else
			stack->args = NULL;

		if (cJSON_HasObjectItem(i_stack_o, "vars")) {
			cJSON* vars_arr = cJSON_GetObjectItem(i_stack_o, "vars");
			int j;
			for (j = 0; j < cJSON_GetArraySize(vars_arr); j++) {
				cJSON* vars_o = cJSON_GetArrayItem(vars_arr, j);
				t_arg_var* var = malloc(sizeof(t_arg_var));
				var->id = cJSON_GetObjectItem(vars_o, "id")->valueint;
				var->off = cJSON_GetObjectItem(vars_o, "off")->valueint;
				var->pag = cJSON_GetObjectItem(vars_o, "pag")->valueint;
				var->size = cJSON_GetObjectItem(vars_o, "size")->valueint;
				list_add(n_pcb->i_stack, var);
			}
		} else
			stack->vars = NULL;

		if (cJSON_HasObjectItem(i_stack_o, "retpos")) {
			cJSON* retpos_o = cJSON_GetObjectItem(i_stack_o, "retpos");
			int* retpos = malloc(sizeof(int));
			*retpos = retpos_o->valueint;
			stack->retpos = retpos;
		} else
			stack->retpos = NULL;

		if (cJSON_HasObjectItem(i_stack_o, "retvar")) {
			cJSON* retvar_o = cJSON_GetObjectItem(i_stack_o, "retvar");
			t_retvar* retvar = malloc(sizeof(t_retvar));
			retvar->off = cJSON_GetObjectItem(retvar_o, "off")->valueint;
			retvar->pag = cJSON_GetObjectItem(retvar_o, "pag")->valueint;
			retvar->size = cJSON_GetObjectItem(retvar_o, "size")->valueint;
			stack->retvar = retvar;
		} else
			stack->retvar = NULL;
	}

	return n_pcb;
}

char* pcb_to_string(pcb* n_pcb) {
	cJSON* root = cJSON_CreateObject();
	cJSON* stack_arr = cJSON_CreateArray();
	cJSON* i_code_arr = cJSON_CreateArray();
	cJSON* i_label_arr = cJSON_CreateArray();

	cJSON_AddItemToObject(root, "pid", cJSON_CreateNumber(n_pcb->pid));
	cJSON_AddItemToObject(root, "pc", cJSON_CreateNumber(n_pcb->pc));
	cJSON_AddItemToObject(root, "page_c", cJSON_CreateNumber(n_pcb->page_c));
	cJSON_AddItemToObject(root, "state", cJSON_CreateNumber(n_pcb->state));
	cJSON_AddItemToObject(root, "cycles", cJSON_CreateNumber(n_pcb->statistics.cycles));
	cJSON_AddItemToObject(root, "op_priviliges", cJSON_CreateNumber(n_pcb->statistics.op_priviliges));
	cJSON_AddItemToObject(root, "exit_code", cJSON_CreateNumber(n_pcb->exit_code));

	void it_stack(void* s) {
		t_stack* stack = s;
		cJSON* stack_o = cJSON_CreateObject();

		if (stack->args != NULL) {
			t_arg_var* args = stack->args;
			cJSON* args_o = cJSON_CreateObject();
			cJSON_AddItemToObject(args_o, "id", cJSON_CreateNumber(args->id));
			cJSON_AddItemToObject(args_o, "pag", cJSON_CreateNumber(args->pag));
			cJSON_AddItemToObject(args_o, "off", cJSON_CreateNumber(args->off));
			cJSON_AddItemToObject(args_o, "size", cJSON_CreateNumber(args->size));

			cJSON_AddItemToObject(stack_o, "args", args_o);
		}

		if (stack->vars != NULL) {
			t_list* vars = stack->vars;
			cJSON* vars_arr = cJSON_CreateArray();
			void it_vars(void* v) {
				t_arg_var* var = v;
				cJSON* var_o = cJSON_CreateObject();
				cJSON_AddItemToObject(var_o, "id", cJSON_CreateNumber(var->id));
				cJSON_AddItemToObject(var_o, "pag", cJSON_CreateNumber(var->pag));
				cJSON_AddItemToObject(var_o, "off", cJSON_CreateNumber(var->off));
				cJSON_AddItemToObject(var_o, "size", cJSON_CreateNumber(var->size));

				cJSON_AddItemToArray(vars_arr, var_o);
			}
			list_iterate(vars, &it_vars);
			cJSON_AddItemToObject(stack_o, "vars", vars_arr);
		}

		if (stack->retpos != NULL) {
			int retpos = *stack->retpos;
			cJSON_AddItemToObject(stack_o, "retpos", cJSON_CreateNumber(retpos));
		}

		if (stack->retvar != NULL) {
			t_retvar* retvar = stack->retvar;
			cJSON* retvar_o = cJSON_CreateObject();
			cJSON_AddItemToObject(retvar_o, "pag", cJSON_CreateNumber(retvar->pag));
			cJSON_AddItemToObject(retvar_o, "off", cJSON_CreateNumber(retvar->off));
			cJSON_AddItemToObject(retvar_o, "size", cJSON_CreateNumber(retvar->size));

			cJSON_AddItemToObject(stack_o, "retvar", retvar_o);
		}

		cJSON_AddItemToArray(stack_arr, stack_o);
	}

	void it_i_code(void* c) {
		t_i_code* i_code = c;
		cJSON* i_code_o = cJSON_CreateObject();

		cJSON_AddItemToObject(i_code_o, "offset", cJSON_CreateNumber(i_code->offset));
		cJSON_AddItemToObject(i_code_o, "length", cJSON_CreateNumber(i_code->length));

		cJSON_AddItemToArray(i_code_arr, i_code_o);
	}

	void it_label(char* key, void* value) {
		int* pos = value;
		cJSON* i_label_o = cJSON_CreateObject();

		cJSON_AddItemToObject(i_label_o, "label", cJSON_CreateString(key));
		cJSON_AddItemToObject(i_label_o, "pos", cJSON_CreateNumber(*pos));

		cJSON_AddItemToArray(i_label_arr, i_label_o);
	}

	list_iterate(n_pcb->i_code, &it_i_code);
	cJSON_AddItemToObject(root, "i_code", i_code_arr);

	dictionary_iterator(n_pcb->i_label, &it_label);
	cJSON_AddItemToObject(root, "i_label", i_label_arr);

	if (n_pcb->i_stack != NULL) {
		list_iterate(n_pcb->i_stack, &it_stack);
		cJSON_AddItemToObject(root, "i_stack", stack_arr);
	}

	return cJSON_Print(root);
}
