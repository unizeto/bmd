#ifndef _AC_DB_COMMON_INCLUDED_
#define _AC_DB_COMMON_INCLUDED_

#include <bmd/libbmdaa2/db/AA_db.h>

#if defined(__cplusplus)
extern "C" {
#endif

	long SplitString(const char *string, const char *separator, 
			SubstringsGenBuf_t **splitted_substrings);
	long AC_ACDistinguishedName2GeneralNames(const ACDistinguishedName_t *ACDistinguishedName, 
			GeneralNames_t **GeneralNames);

	long UnizetoAttrSyntaxInList_free_ptr(UnizetoAttrSyntaxInList_t **UnizetoAttrSyntaxInList);
	long UnizetoAttrSyntaxIn_free_ptr(UnizetoAttrSyntaxIn_t **UnizetoAttrSyntaxIn);
	long SlotList_free_ptr(SlotListIn_t **SlotList);
	long SlotIn_free_ptr(SlotIn_t **SlotIn);
	long AA_db_read_GenBuf_from_file(const char *filename, GenBuf_t **gbuf);

	
#if defined(__cplusplus)
}
#endif /*__cplusplus*/
#endif /*_AC_DB_COMMON_INCLUDED_*/

