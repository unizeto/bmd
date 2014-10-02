#ifndef _AC_Clearance_INCLUDED_
#define _AC_Clearance_INCLUDED_

#include <bmd/libbmdasn1_common/Clearance.h>
#include <bmd/libbmdaa2/wrappers/AC_wrappers.h>

#define CLASS_LIST_UNMARKED		0
#define CLASS_LIST_UNCLASSIFIED		1
#define CLASS_LIST_RESTRICTED		2
#define CLASS_LIST_CONFIDENTIAL		3
#define CLASS_LIST_SECRET		4
#define CLASS_LIST_TOP_SECRET		5



#if defined(__cplusplus)
extern "C" {
#endif
	int AC_Clearance_create(const OBJECT_IDENTIFIER_t *ClearancePolicyOid,
				ClassList_t *ClassList,
				Clearance_t *Clearance);
	int AC_Clearance_create_alloc(const OBJECT_IDENTIFIER_t *ClearancePolicyOid,
				ClassList_t *ClassList,
				Clearance_t **Clearance);
	int AC_ClearanceSet_add_SecurityCategory(const SecurityCategory_t *SecurityCategory, 
				Clearance_t *Clearance);
#if defined(__cplusplus)
}
#endif /*__cplusplus*/
#endif /*_AC_Clearance_INCLUDED_*/
