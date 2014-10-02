#include<bmd/libbmdaa2/wrappers/AC_Slot.h>
#include <bmd/libbmderr/libbmderr.h>

/* Wywolywac przy pierwszym wstawieniu do  SEQUENCE OF. */
int AC_Slot_create(OBJECT_IDENTIFIER_t *Oid, Slot_t *Slot)
{
int err = 0;
	if(Oid == NULL)
		return BMD_ERR_PARAM1;
	if(Slot == NULL)
		return BMD_ERR_PARAM2;

	memset(Slot,0,sizeof(Slot_t));
	/* typ slotu */
	Slot->slotType.buf  = Oid->buf;
	Slot->slotType.size = Oid->size;
	return err;
}

int AC_Slot_create_alloc(OBJECT_IDENTIFIER_t *Oid, Slot_t **Slot)
{
int err = 0;
	if(Oid == NULL)
		return BMD_ERR_PARAM1;
	if(Slot == NULL || *Slot != NULL)
		return BMD_ERR_PARAM2;
	
	*Slot = (Slot_t *) malloc (sizeof(Slot_t));
	if(*Slot == NULL) return NO_MEMORY;

	err = AC_Slot_create(Oid, *Slot);
	if(err<0){
		PRINT_DEBUG("LIBBMDAAERR[%s:%i:%s] Error in creating Slot_t. "
			"Recieved error code = %i.\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	return err;
}

int AC_Slot_add_AnyValue(ANY_t *ANY, Slot_t *Slot)
{
int err = 0;
	if(ANY == NULL)
		return BMD_ERR_PARAM1;
	if(Slot == NULL)
		return BMD_ERR_PARAM2;

	/* W przypadku gdyby ktos chcial dodac ANY, a tablica 			*/
	/* wskaznikow nie bylaby zaalokowana, wowczas zalokuj pamiec na nia	*/
	/* i podepnij do 1 wskaznika 1 element.					*/
	if (Slot->slotValues == NULL) {
		Slot->slotValues = (struct Slot__slotValues *) malloc ( sizeof(struct Slot__slotValues) );
		if(Slot->slotValues == NULL) return NO_MEMORY;
		memset(Slot->slotValues, 0, sizeof(struct Slot__slotValues));
	}
	asn_set_add(&(Slot->slotValues->list), ANY);
	return err;
}
