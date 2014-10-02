#include <bmd/libbmdaa2/wrappers/AC_RDNSequence.h>
#include <bmd/libbmderr/libbmderr.h>

/* Funkcji alokujacej, inicjalizujacej i zwalniajacej nie ma bo z reguly
   RDNSequence_t jest statycznym elementem skladowym Name_t i operacje na
   Name_t powoduja analogiczne efekty w RDNSequence_t. */

int AC_RDNSequenceSetAddValue(DNValueType_t DNValueType, const char *DNValue, RDNSequence_t *RDNSequence)
{
int err = 0;
RelativeDistinguishedName_t *RelativeDistinguishedName = NULL;

	if(DNValueType < 0)
		return BMD_ERR_PARAM1;
	if(DNValue == NULL)
		return BMD_ERR_PARAM2;
	if(RDNSequence == NULL)
		return BMD_ERR_PARAM3;
	
	/* Utworz RelativeDistinguishedName z podanego stringu */
	err = AC_CreateRelativeDistinguishedNameFromString(DNValueType, 
				DNValue, 
				&RelativeDistinguishedName);
	if(err<0){
		PRINT_DEBUG("LIBBMDAAINF[%s:%i:%s] "
				"Error in creating RelativeDistinguishedName from string. "
				"Recieved error code = %i\n",
				__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}

	/* Dodaj powyzszy RelativeDistinguishedName do listy RDNSequence */
	err = asn_set_add(&(RDNSequence->list), RelativeDistinguishedName);
	if(err<0){
		PRINT_DEBUG("LIBBMDAAINF[%s:%i:%s] Error in adding RelativeDistinguishedName to RDNSequence. "
				"Recieved error code = %i\n",
				__FILE__, __LINE__, __FUNCTION__, err);
		return ERR_AA_ASN1_SET_ADD;
	}
	return err;
}

int AC_RDNSequence_clone_pointers(RDNSequence_t *src, RDNSequence_t *dest)
{
int err = 0;
	if(src == NULL)
		return BMD_ERR_PARAM1;
	if(dest == NULL)
		return BMD_ERR_PARAM2;
	dest->list.array = src->list.array;
	dest->list.count = src->list.count;
	dest->list.size  = src->list.size;
	dest->list.free  = src->list.free;
	dest->_asn_ctx.phase = src->_asn_ctx.phase;
	dest->_asn_ctx.step  = src->_asn_ctx.step;
	dest->_asn_ctx.left  = src->_asn_ctx.left;
	dest->_asn_ctx.ptr   = src->_asn_ctx.ptr;
	return err;
}
