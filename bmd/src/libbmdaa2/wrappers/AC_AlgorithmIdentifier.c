#include <bmd/libbmdaa2/wrappers/AC_AlgorithmIdentifier.h>
#include <bmd/libbmderr/libbmderr.h>

int AC_AlgorithmIdentifier_set_AlgorithmOid(OBJECT_IDENTIFIER_t *Oid, AlgorithmIdentifier_t *AlgorithmIdentifier)
{
int err = 0;
	if(Oid == NULL)
		return BMD_ERR_PARAM1;
	if(AlgorithmIdentifier == NULL)
		return BMD_ERR_PARAM2;

	err = AC_OBJECT_IDENTIFIER_clone_pointers(Oid, &(AlgorithmIdentifier->algorithm));
	if(err<0){
		PRINT_DEBUG("LIBBMDAAERR[%s:%i:%s] Error in setting algorithm Oid in AlgorithmIdentifier_t. "
			"Recieved error code = %i.\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	return err;
}

int AC_AlgorithmIdentifier_set_AlgorithmParameters(ANY_t *Parameters, AlgorithmIdentifier_t *AlgorithmIdentifier)
{
int err = 0;
	if(Parameters == NULL)
		return BMD_ERR_PARAM1;
	if(AlgorithmIdentifier == NULL)
		return BMD_ERR_PARAM2;

	AlgorithmIdentifier->parameters = Parameters;
	return err;
}
