
#ifndef _bmd_datagram_set_included_
#define _bmd_datagram_set_included_



#include<bmd/libbmdpr/bmd_datagram/bmd_datagram.h>
#include<bmd/libbmdpr/bmd_datagram/asn1clib/BMDDatagram.h>
#include<bmd/libbmdpr/bmd_datagram/asn1clib/BMDDatagramSet.h>

#if defined(__cplusplus)
extern "C" {
#endif
	LIBBMDPR_SCOPE long PR2_bmdDatagramSet_fill_with_datagrams(long number, bmdDatagramSet_t **dtg_set);
	/* [MSil] Funkcja dodaje datagram do zbioru, jesli potrzeba to alokuje zbior (gdy NULL) */
	LIBBMDPR_SCOPE long PR2_bmdDatagramSet_add(bmdDatagram_t *dtg,bmdDatagramSet_t **dtg_set);
	/* [MSil] Funkcja serializuje zbior datagramow i alokuje wyjsciowy bufor */
	LIBBMDPR_SCOPE long PR2_bmdDatagramSet_serialize(	bmdDatagramSet_t *dtg_set,
								GenBuf_t *kn,
								GenBuf_t ***twf_output_buffers,
								long *twf_no_of_buffers,
								long twf_free_datagrams_flag,
								long twf_start_index,
								long twf_transmission_count);

	/* [MSil] Funkcja deserializuje zbior datagramow i alokuje miejsce pod strukture */
	LIBBMDPR_SCOPE long PR2_bmdDatagramSet_deserialize(	GenBuf_t **input,
								long twf_prefix,
								bmdDatagramSet_t **dtg_set,
								long *twf_bmdDatagramsTransmissionCount,
								long *twf_bmdDatagramsAllCount,
								long twf_input_free,
								long *twf_memory_guard,
								long deserialisation_max_memory);

	/* [MSil] Funkcja zwalnia zbior datagramow i zeruje wskaznik */
	LIBBMDPR_SCOPE long PR2_bmdDatagramSet_free(bmdDatagramSet_t **dtg_set);

	/*Dodaje wskazniki na datagramy z drugiego datagramsetu. Tablica wskzanikow na datagramy z dtg_set_source
	jest usuwana. Same datagramy nie sa zwalniane gdyz sa juz powiazane z dtg_set_destiny.*/
	LIBBMDPR_SCOPE long PR2_bmdDatagramSet_merge_pointers(	bmdDatagramSet_t *dtg_set_destiny, \
								bmdDatagramSet_t *dtg_set_source);

	/* [MSil] Tworzy i inicjalizuje bmdDatagramSet */
	long _PR2_create_bmdDatagram_set(bmdDatagramSet_t **bmdDatagramSet);
	/* [MSil] Funkcja dodaje datagram do Set - starsza wersja nowszej */
	long _PR2_add_bmdDatagram_to_set(bmdDatagram_t *bmdDatagram,
					bmdDatagramSet_t *bmdDatagramSet);
	/* [MSil] Starsza wersja funkcji zwalniajacej */
	long _PR_bmdDatagramSet_free(bmdDatagramSet_t *bmdDatagramSet,
				    long with_pointer);
	/* [MSil] zakoduj strukture NP do bufora, alokujac bufor */
	long _PR2_bmdDatagramSet_der_encode(BMDDatagramSet_t *set, GenBuf_t *kn, GenBuf_t **output);
	/* [MSil] zdekoduj bufor do struktury NP, alokujac strukture */
	long _PR2_bmdDatagramSet_der_decode(	GenBuf_t **input,
						long twf_prefix,
						BMDDatagramSet_t **set,
						long twf_input_free);
	/* [MSz] inicjalizacja struktury zbioru w stary sposob */
	long PR_bmdDatagramSet_init(bmdDatagramSet_t * bmdDatagramSet);
	/* [Msz] drukuje strukture bmdDatagramSet_t */
	long PR_bmdDatagramSet_print(bmdDatagramSet_t * bmdDatagramSet,
							WithTime_t WithTime,
							FILE *fd);

#if defined(__cplusplus)
}
#endif
#endif
