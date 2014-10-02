#ifndef __ERR_STACK_INCLUDED__
#define __ERR_STACK_INCLUDED__

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <bmd/common/bmd-const.h>
#include <bmd/libbmd/libbmd.h>
#include <bmd/common/bmd-errors.h>

typedef struct node {
	struct node *next;
	long err_code;
	char *lib_prefix;
	char *additional_desc;
	char *additional_sol;
	char *file;
	long line;
	char *function;
} node_t;

typedef struct err_stack {
	node_t *head;
	long size;
} errs_t;

typedef errs_t * err_t;
typedef enum node_arg {
#ifndef WIN32
LP, 
#endif
AD, AS} node_arg_t;

/****************************************/
/* GENERYCZNY STOS (API prywatne)       */
/****************************************/
int stack_create(errs_t **stack);
int stack_free(errs_t **stack);
int stack_print(const errs_t *stack);
int node_create(const long err_code, node_t **node, const char *file, const long line, const char *function);
int node_set_arg(const node_arg_t ArgType, const char *arg_value, node_t *node);
int node_free(node_t **node);
int node_print(const node_t *node, const int trailing_spaces);
int pts(const int trailing_spaces);
int add_node_2_stack(node_t *node, errs_t *stack);

/****************************************/
/* FUNKCJE DO ZARZADZANIA STOSEM BLEDOW */
/* (API prywatne)			*/
/****************************************/
int bmd_ok(const err_t err);
int bmd_err(const err_t err);
int bmd_set_err(err_t *err, const long errcode, const char *file, const long line, const char *function);
int bmd_seti_err(err_t err, const node_arg_t info_type, const char *info_value);
int bmd_bt_err(const err_t err);
int bmd_free_err(err_t *err);

/****************************************/
/* PUBLICZNE API STOSU BLEDOW		*/
/****************************************/
#define BMD_ERR(x)		bmd_err(x)
/* #define BMD_OK(x)		bmd_ok(x)*/
#define BMD_SETERR(x,y)		bmd_set_err(&x,y,__FILE__,__LINE__,__FUNCTION__);
#define BMD_SETIERR(x,y,z)	bmd_seti_err(x,y,z);
#define BMD_BTERR(x)		bmd_bt_err(x);
#define BMD_FREEERR(x)		bmd_free_err(&x);

#endif /*__ERR_STACK_INCLUDED__*/
