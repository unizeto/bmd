#include <bmd/libbmdaa2/wrappers/AC_ClassList.h>
#include <bmd/libbmderr/libbmderr.h>

#define CLASS_LIST_SIZE 1

int AC_ClassList_create(int level, ClassList_t *ClassList)
{
int err = 0;
	if(level < 0)
		return BMD_ERR_PARAM1;
	if(ClassList == NULL)
		return BMD_ERR_PARAM2;
	
	memset(ClassList,0,sizeof(ClassList_t));
		
	ClassList->buf = (uint8_t *) malloc (sizeof (uint8_t) * 1);
	if (ClassList->buf == NULL) return NO_MEMORY;
	/* Zawartosc Bufora:				*/
	/* unmarked    	= 1		length = 1	*/
	/* unclassified = 01		length = 2	*/
	/* restricted   = 001		length = 3	*/
	/* confidential = 0001		length = 4	*/
	/* secret       = 00001		length = 5	*/
	/* topsecret    = 000001	length = 6	*/
	*(ClassList->buf)      = (uint8_t)(1 << (7-level));
	ClassList->size        = CLASS_LIST_SIZE;	
	ClassList->bits_unused = CLASS_LIST_SIZE * 8 - (level+1);
	return err;
}

int AC_ClassList_create_alloc(int level, ClassList_t **ClassList)
{
int err = 0;
	if(level < 0)
		return BMD_ERR_PARAM1;
	if(ClassList == NULL || *ClassList != NULL)
		return BMD_ERR_PARAM2;
		
	*ClassList = (ClassList_t *) malloc (sizeof(ClassList_t));
	if(*ClassList == NULL) return NO_MEMORY;
		
	err=AC_ClassList_create(level, *ClassList);
	if(err<0){
		PRINT_DEBUG("LIBBMDAAERR[%s:%i:%s] Error in creating ClassList_t. "
			"Recieved error code = %i.\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	return err;
}
