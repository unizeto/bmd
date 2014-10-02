#ifndef _bmd_datagram_included_
#define _bmd_datagram_included_
#include <bmd/libbmd/libbmd.h>
#include <bmd/libbmdpr/prlib-common/PR_lib.h>
#include <bmd/common/bmd-common.h>
#include <bmd/libbmdpr/bmd_datagram/asn1clib/BMDDatagram.h>

typedef enum WithTime {WITHOUT_TIME, WITH_TIME} WithTime_t;

#if defined(__cplusplus)
extern "C" {
#endif
	/* [MSil]     Funkcja zwalnia datagram i zeruje wskaznik */
	LIBBMDPR_SCOPE long PR2_bmdDatagram_free(bmdDatagram_t **dtg);

	/* [MSil]     Funkcja serializuje datagram i umieszcza DER w buforze, alokujac miejsce */
	LIBBMDPR_SCOPE long PR2_bmdDatagram_serialize(bmdDatagram_t *dtg,GenBuf_t **output);

	/* [MSil]     Funkcja deserializuje bufor i umieszcza w strukturze, alokujac miejsce */
	LIBBMDPR_SCOPE long PR2_bmdDatagram_deserialize(GenBuf_t *input,bmdDatagram_t **dtg);

	/* [MSil/MSz] Funkcja dodaje metadana do datagramu */
	/*LIBBMDPR_SCOPE by WSZ*/ long PR2_bmdDatagram_add_metadata(char *OIDtable,
							const char *Buf,
							long BufSize,
							EnumMetaData_t KindOfMetaData,
							bmdDatagram_t * bmdDatagram,
							long myId,
							long ownerType,
							long ownerId,
							long twf_copy_ptr);

	/* [MSz/MSil] Funkcja tworzy strukture datagramu i ja inicjalizuje */
	LIBBMDPR_SCOPE long PR2_bmdDatagram_create(bmdDatagram_t ** bmdDatagram);
	/* [MSz/MSil] Funkcja inicjalizuje datagram */
	LIBBMDPR_SCOPE long PR_bmdDatagram_init(bmdDatagram_t * bmdDatagram);
	/* [Msz] Funkcja wyswietla datagram */
	LIBBMDPR_SCOPE long PR_bmdDatagram_print(bmdDatagram_t * bmdDatagram,
						WithTime_t WithTime,
						FILE *fd);
	/* [Msz] Funkcja wyswietla liste metadanych */
	LIBBMDPR_SCOPE
	long PR_bmdDatagram_metadata_print(MetaDataBuf_t **MetaDataArray,
						long no_of_metadata, WithTime_t WithTime,
						FILE *fd);

	long PR2_bmdDatagram_serialize_count_bytes_TW(	bmdDatagram_t *dtg,
							long *twf_size_of_serialized_datagram,
							long *twf_metadata_string_len);
	long PR2_bmdDatagram_serialize_TW(	bmdDatagram_t *dtg,
						char *twf_serialized_datagram_ptr,
						long twf_size_of_serialized_datagram,
						long twf_metadata_string_len);
	long PR2_bmdDatagram_deserialize_TW(	char *twf_input,
						bmdDatagram_t **dtg,
						long *twf_deserialised_len,
						long *twf_memory_guard,
						long twf_max_memory);

	/* [MSil] Funkcja konwertuje (Wysoko Poziomowa) strukture na (Nisko Poziomowa) strukture */
	long _PR2_bmdDatagram2BMDDatagram(bmdDatagram_t *dtg,BMDDatagram_t **asn1_dtg);
	/* [MSil] Funkcja konwertuje NP strukture na WP strukture */
	long _PR2_BMDDatagram2bmdDatagram(BMDDatagram_t *asn1_dtg,bmdDatagram_t **dtg);
	/* [MSil] Funkcja koduje NP strukture do DER */
	long _PR2_BMDDatagram_der_encode(BMDDatagram_t *asn1_dtg,GenBuf_t **output);
	/* [MSil] Funkcje dekoduje bufor do NP struktury */
	long _PR2_BMDDatagram_der_decode(GenBuf_t *input,BMDDatagram_t **asn1_dtg);
	/* [MSil] Funkcja ustawia liste metadanych w NP strukturze */
	long _PR2_BMDDatagram_set_metadata(MetaDataBuf_t **mtds,long no,
					  long type,BMDDatagram_t **asn1_dtg);
	/* [MSil] Funkcja inicjalizuje NP strukture */
	long _PR2_BMDDatagram_init(BMDDatagram_t **bmdDatagramStruct);
	/* [MSil/MSz] Stara funkcja zwalniajaca - wolana w nowej */
	long _PR_bmdDatagram_free(bmdDatagram_t *bmdDatagram, long with_pointer);
	/* [MSil] Funkcja konwertuje NP strukture metadanych na WP strukture */
	long _PR_MetaData2MetaDataBuf(struct MetaDatas *input,
			     MetaDataBuf_t ***mtds,long *no_of_mtds,  bmdDatagram_t *twf_bmdDatagram);
	/* [MSil] Funkcja konwertuje WP strukture na NP strukture metadanych */
	long _PR_MetaDataBuf2MetaData(MetaDataBuf_t **mtds,long no,
				     struct MetaDatas **asn1_mtds);

	long PR_bmdDatagram_PrepareResponse(bmdDatagram_t **bmdDatagram, long dtg_type, long status);
	long AddElementToMetaDataTable(	MetaDataBuf_t ***MetaDataArray,
							long *no_of_MetaData,
							char *OIDtable,
							GenBuf_t *DerEncodedValue,
							long myId,
							long ownerType,
							long ownerId,
							long twf_copy_ptr);

#if defined(__cplusplus)
}
#endif
#endif
