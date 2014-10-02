#include <bmd/libbmdpr/prlib-common/PR_OBJECT_IDENTIFIER.h>

/*
 * Funkcja porownuje 2 OIDy niezaleznie czy sa to zakodowane DERy
 * czy tablice przygotowane dopiero do zakodowania.
 * Zwraca 0 - identyczne OIDy
 * != 0	    - rozne OIDy
 */
long PR_OBJECT_IDENTIFIER_cmp(void *oid1, int sizeoid1, void *oid2, int sizeoid2)
{
	if (oid1 == oid2) return 0; /* szybki test wskaznikow */
	if ((oid1 == NULL) || (oid2 == NULL)) return NO_ARGUMENTS;
	if (sizeoid1 != sizeoid2) return UNCOMPARABLE_ARGUMENTS;
	return memcmp (oid1, oid2, sizeoid1);
}

