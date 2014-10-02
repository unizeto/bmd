#include <bmd/libbmderr/err_stack.h>

/****************************************/
/* GENERYCZNY STOS		        */
/****************************************/
int stack_create(errs_t **stack)
{
int err = 0;
	if(stack == NULL || *stack != NULL)
		return BMD_ERR_PARAM1;
	*stack = (errs_t *) malloc (sizeof(errs_t));
	if(*stack == NULL) return BMD_ERR_MEMORY;
	memset(*stack, 0, sizeof(errs_t));
	return err;
}

int stack_free(errs_t **stack)
{
int err = 0;
node_t *tmp = NULL, *prev=NULL;

	if(stack == NULL)
		return BMD_ERR_PARAM1;
	if(*stack == NULL)
		return err;
		
	tmp=(*stack)->head;
	while(tmp->next){
		prev = tmp;
		tmp = tmp->next;
		node_free(&prev);
	}
	node_free(&tmp);
	free(*stack); *stack=NULL;
	return err;
}

int stack_print(const errs_t *stack)
{
int err = 0;
node_t *tmp = NULL;
int i=0;

	if(stack == NULL)
		return BMD_ERR_PARAM1;
	
	tmp = stack->head;
	while(tmp->next){
		printf("[===>]");
		node_print(tmp, i);
		tmp = tmp->next;
		i+=1;
	}	
	printf("[===>]");
	node_print(tmp, i);
	return err;
}

int node_create(const long err_code, node_t **node, const char *FILE, const long LINE, const char *FUNCTION)
{
int err = 0;
	if(err_code > 0)
		return BMD_ERR_PARAM1;
	if(node == NULL || *node != NULL)
		return BMD_ERR_PARAM2;
	
	*node = (node_t *) malloc (sizeof(node_t));
	if(*node == NULL) return BMD_ERR_MEMORY;
	memset(*node, 0, sizeof(node_t));

	(*node)->err_code = err_code;
	
	asprintf(&((*node)->file), "%s", FILE);
	if((*node)->file == NULL) return BMD_ERR_MEMORY;

	(*node)->line = LINE;

	asprintf(&((*node)->function), "%s", FUNCTION);
	if((*node)->function == NULL) return BMD_ERR_MEMORY;
	return err;
}

int node_set_arg(const node_arg_t ArgType, const char *arg_value, node_t *node)
{
int err = 0;
	if(ArgType < 0)
		return BMD_ERR_PARAM1;
	if(arg_value == NULL)
		return BMD_ERR_PARAM2;
	if(node == NULL)
		return BMD_ERR_PARAM3;
		
	switch(ArgType){
	case LP:
		asprintf(&(node->lib_prefix), "%s", arg_value);
		if(node->lib_prefix == NULL) return BMD_ERR_MEMORY;
		break;
	case AD:
		asprintf(&(node->additional_desc), "%s", arg_value);
		if(node->additional_desc == NULL) return BMD_ERR_MEMORY;
		break;
	case AS:
		asprintf(&(node->additional_sol), "%s", arg_value);
		if(node->additional_sol == NULL) return BMD_ERR_MEMORY;
		break;

	}
	return err;
}

int node_free(node_t **node)
{
int err = 0;
	if(node == NULL || *node == NULL)
		return BMD_ERR_PARAM1;

	if((*node)->lib_prefix) { free((*node)->lib_prefix); (*node)->lib_prefix = NULL; }
	if((*node)->additional_desc) { free((*node)->additional_desc); (*node)->additional_desc = NULL; }
	if((*node)->additional_sol) { free((*node)->additional_sol); (*node)->additional_sol = NULL; }
	if((*node)->file) { free((*node)->file); (*node)->file = NULL; }
	if((*node)->function) { free((*node)->function); (*node)->function = NULL; }
	free(*node); *node = NULL;
	return err;
}

int node_print(const node_t *node, const int trailing_spaces)
{
int err = 0;
	if(node == NULL)
		return err;

	printf("Plik [%s] Linia [%li] Funkcja [%s]:\n", node->file, node->line, node->function);
	pts(trailing_spaces); printf("EC: %li\n", node->err_code);
	pts(trailing_spaces); printf("LP: %s\n", (node->lib_prefix)?node->lib_prefix:"empty");
	pts(trailing_spaces); printf("AD: %s\n", (node->additional_desc)?node->additional_desc:"empty");
	pts(trailing_spaces); printf("AS: %s\n", (node->additional_sol)?node->additional_sol:"empty");
	return err;
}

int pts(const int trailing_spaces)
{
int i = 0;
	if(trailing_spaces < 0)
		return BMD_ERR_PARAM1;

	for(i=0; i<trailing_spaces*4; i++) printf(" ");
	return 0;
}


int add_node_2_stack(node_t *node, errs_t *stack)
{
int err = 0;
		
	if(node == NULL)
		return BMD_ERR_PARAM1;
	if(stack == NULL)
		return BMD_ERR_PARAM2;
		
	node->next = stack->head;
	stack->head = node;
	stack->size++;
	return err;
}

/****************************************/
/* FUNKCJE DO ZARZADZANIA STOSEM BLEDOW */
/****************************************/
int bmd_ok(const err_t err)
{
	if(err == NULL)
		return BMD_ERR_PARAM1;
	
	if(err->head == NULL) return 1;
	else return 0;
}

int bmd_err(const err_t err)
{
	if(err == NULL)
 	 return 0;
	
	if(err->head != NULL) return 1;
	else return 0;
}

int bmd_set_err(err_t *err, const long errcode, const char *FILE, const long LINE, const char *FUNCTION)
{
int status = 0;
node_t *node = NULL;
	
	/* jesli nie ma stosu to stworz */
	if(err == NULL)
		return BMD_ERR_PARAM1;
	if(*err == NULL){ /* BUG stack create */
				
		status = stack_create(err);
		if(status<0){
			printf("[%s:%i:%s] Error in creating error stack. status = %d\n", 
				__FILE__, __LINE__, __FUNCTION__, status);
			return status;
		}
	}
	/* dopisz wezel */
	status = node_create(errcode, &node, FILE, LINE, FUNCTION);
	if(status<0){
		printf("[%s:%i:%s] Error in creating error node. status = %d\n", 
			__FILE__, __LINE__, __FUNCTION__, status);
		return status;
	}
	status = add_node_2_stack(node, *err);
	if(status<0){
		printf("[%s:%i:%s] Error in adding error node to stack. status = %d\n", 
			__FILE__, __LINE__, __FUNCTION__, status);
		return status;
	}
	return status;
}

int bmd_seti_err(err_t err, const node_arg_t info_type, const char *info_value)
{
int status = 0;
	if(err == NULL)
		return BMD_ERR_PARAM1;
	if(info_type < 0)
		return BMD_ERR_PARAM2;
	if(info_value == NULL)
		return BMD_ERR_PARAM3;
			
	status =  node_set_arg(info_type, info_value, err->head);
	if(status<0){
		printf("[%s:%i:%s] Error in setting error information in stack header\n", 
			__FILE__, __LINE__, __FUNCTION__);
		return status;
	}
	return status;
	
}

int bmd_bt_err(const err_t err)
{
int status = 0;
	if(err == NULL)
		return BMD_ERR_PARAM1;
	status = stack_print(err);
	if(status<0){
		printf("[%s:%i:%s] Error printing stack. status = %d\n", 
			__FILE__, __LINE__, __FUNCTION__, status);
		return status;
	}
	return status;
}

int bmd_free_err(err_t *err)
{
int status = 0;
	if(err == NULL)
		return BMD_ERR_PARAM1;
			
	status = stack_free(err);
	if(status<0){
		printf("[%s:%i:%s] Error clearing stack. status = %d\n", 
			__FILE__, __LINE__, __FUNCTION__, status);
		return status;
	}
	return status;
}


