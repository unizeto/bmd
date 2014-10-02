#ifndef _AC_Slot_INCLUDED_
#define _AC_Slot_INCLUDED_

#include <bmd/libbmdasn1_common/Slot.h>
#include <bmd/libbmdaa2/wrappers/AC_wrappers.h>

#if defined(__cplusplus)
extern "C" {
#endif

	int AC_Slot_create(OBJECT_IDENTIFIER_t *Oid, Slot_t *Slot);
	int AC_Slot_create_alloc(OBJECT_IDENTIFIER_t *Oid, Slot_t **Slot);
	int AC_Slot_add_AnyValue(ANY_t	*ANY, Slot_t *Slot );
#if defined(__cplusplus)
}
#endif /*__cplusplus*/
#endif /*_AC_Slot_INCLUDED_*/


