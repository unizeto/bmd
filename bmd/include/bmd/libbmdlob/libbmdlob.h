#ifndef _LOBZ_H_INCLUDED_
#define _LOBZ_H_INCLUDED_
#include<sys/types.h>
#include<bmd/libbmdnet/libbmdnet.h>
#include<bmd/libbmdpki/libbmdpki.h>

#ifdef WIN32
	#ifdef LIBBMDLOB_EXPORTS
		#define LIBBMDLOB_SCOPE __declspec(dllexport)
	#else
		#ifdef _USRDLL
			#define LIBBMDLOB_SCOPE __declspec(dllimport)
		#else
			#define LIBBMDLOB_SCOPE
		#endif
	#endif
#else
	#define LIBBMDLOB_SCOPE
#endif //ifdef WIN32

#define MAX_LOB_SIZE 5120

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct ts_info {
	char *server_name;
	long port;
} ts_info_t;
	
#define ESC_SEQ_SIZE 6

typedef struct lob_transport {
	bmdnet_handler_ptr handler;	/* polaczenie sieciowe */
	bmd_crypt_ctx_t *enc_ctx;	/* kontekst do szyfrowania symetrycznego */
	bmd_crypt_ctx_t *dec_ctx;	/* kontekst do deszyfrowania kontekst do deszyfrowania symetrycznego */
	bmd_crypt_ctx_t *hash_ctx;	/* kontekst do liczenia skrotu - zawierajacy w sobie informacje o funkcji skrotu */

	ts_info_t *ts_info;		/* struktura do znakowania czasem */
	long options;

        u_int64_t tfs;  /* calosciowy rozmiar pliku */
	u_int64_t fs;	/* rozmiar pliku */
	u_int64_t fl;   /* ile zostalo jeszcze */
	
} lob_transport_t;

enum { CALC_HASH = 1, TS_PLAIN_DATA = 2 };

LIBBMDLOB_SCOPE long new_lob_transport(struct lob_transport **lob,bmdnet_handler_ptr * net_handler,
				      bmd_crypt_ctx_t * enc_ctx,
				      bmd_crypt_ctx_t * dec_ctx,
				      bmd_crypt_ctx_t * hash_ctx,
				      ts_info_t * ts_info,
				      u_int64_t options);


LIBBMDLOB_SCOPE long free_lob_transport(struct lob_transport **lob);

LIBBMDLOB_SCOPE long set_lob_size(struct lob_transport **lob, u_int64_t lob_size);

LIBBMDLOB_SCOPE long get_lob_timestamp(struct lob_transport **lob,GenBuf_t **ts_buf);

LIBBMDLOB_SCOPE long get_lob_signature(struct lob_transport **lob,GenBuf_t **sig_buf);

LIBBMDLOB_SCOPE long get_lob_max_size(struct lob_transport **lob, u_int64_t *lob_size);

LIBBMDLOB_SCOPE long get_lob_in_size(struct lob_transport **lob, u_int64_t *lob_size);

LIBBMDLOB_SCOPE long get_lob_out_size(struct lob_transport **lob, u_int64_t *lob_size);

LIBBMDLOB_SCOPE long send_lob_chunk(struct lob_transport **lob, GenBuf_t **lob_data, long is_last);

LIBBMDLOB_SCOPE long send_lob_cancel(struct lob_transport **lob);

LIBBMDLOB_SCOPE long recv_lob_chunk(struct lob_transport **lob, GenBuf_t **lob_data,long min_bytes_to_get, long max_bytes_to_get);

LIBBMDLOB_SCOPE long get_lob_hash(struct lob_transport **lob,GenBuf_t **hash_buf);

LIBBMDLOB_SCOPE long get_lob_timestamp(struct lob_transport **lob,GenBuf_t **ts_buf);
	
#if defined(__cplusplus)
}
#endif
#endif /* LOBZ_H */
