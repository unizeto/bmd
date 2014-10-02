#ifndef _LIBBMDPKI_INCLUDED_
#define _LIBBMDPKI_INCLUDED_

#include <bmd/common/bmd-common.h>
#include <bmd/common/bmd-crypto_ds.h>
#include <bmd/libbmdasn1_core/OCTET_STRING.h>
#include <bmd/libbmdasn1_common/TSTInfo.h>
#include <bmd/libbmdcms/libbmdcms.h>

typedef struct progress_data progress_data_t;

#ifdef WIN32
	#ifdef LIBBMDPKI_EXPORTS
		#define LIBBMDPKI_SCOPE __declspec(dllexport)
	#else
		#ifdef _USRDLL
			#define LIBBMDPKI_SCOPE __declspec(dllimport)
		#else
			#define LIBBMDPKI_SCOPE
		#endif 
	#endif
#else
	#define LIBBMDPKI_SCOPE
#endif /* ifdef _WIN32 */

#define BMD_HASH_SHA1_SIZE	20

#define BMD_CTX_SOURCE_FILE	0x01
#define BMD_CTX_SOURCE_PKCS11	0x02
#define BMD_CTX_SOURCE_WIN	0x03

/** @note ustawienie ktore ustawia kontekst do uzycia z szyfrowaniem badz deszyfrowaniem symetrycznym */
#define BMD_CTX_SOURCE_NONE	0x04

#define BMD_CTX_SOURCE_KS	0x05

/* ogolny kontekst do operacji asymetrycznych - zarowno kluczem publicznym jak i prywatnym oraz podpis rowniez */
#define BMD_CTX_TYPE_ASYM	0x01
/* kontekst do operacji symetrycznych */
#define BMD_CTX_TYPE_SYM	0x02
/* kontekst do liczenia skrotow */
#define BMD_CTX_TYPE_HASH	0x03

/* kontekst do podpisow */
#define BMD_CTX_TYPE_SIG	0x04

/* kontekst do szyfrowania asymetrycznego */
#define BMD_CTX_TYPE_ASYM_E	0x05

/* kontekst do deszyfrowania asymetrycznego */
#define BMD_CTX_TYPE_ASYM_D	0x06

/*
* opcje kontekstu PKCS#11
* DEDBS - dont estimate decryption buffer size (wplywa tylko na deszyfrowanie asymetryczne)
* BMD_CTX_PKCS11_OPTION_DEFAULT oznacza, ze deszyfrowanie bedzie wykonane w dwoch wywolaniach (pierwsze zwracajacego rozmiar potrzebnego bufora wyjsciowego, drugie wypelniajace bufor)
* BMD_CTX_PKCS11_OPTION_DEDBS oznacza, ze deszyfrowanie bedzie wykonane pojedynczym wywolaniem (od razu wypelnia dostarczony bufor)
*/
typedef enum 
{
	BMD_CTX_PKCS11_OPTION_DEFAULT=0x01,
	BMD_CTX_PKCS11_OPTION_DEDBS=0x02
} BmdCtxPkcs11Option_e;


#define BMD_HASH_ALGO_SHA1	0x01
#define BMD_HASH_ALGO_DEFAULT	BMD_HASH_ALGO_SHA1
#define BMD_HASH_ALGO_MD5		0x02

#define BMD_CRYPT_ALGO_DES3	0x01
#define BMD_CRYPT_ALGO_AES	0x02

#define BMD_CRYPT_ALGO_SYM_DEFAULT BMD_CRYPT_ALGO_DES3

#define BMD_CRYPT_ALGO_RSA	0x03
#define BMD_CRYPT_ALGO_ASYM_DEFAULT BMD_CRYPT_ALGO_RSA

#define BMD_CMS_ENV_INTERNAL	0x0001
#define BMD_CMS_ENV_EXTERNAL	0x0002

#define BMD_CMS_SIG_INTERNAL	0x0001
#define BMD_CMS_SIG_EXTERNAL	0x0002
#define BMD_CMS_SIG_LOB         0x0004

/* liczenie hasha strumieniowo */
#define BMD_HASH_SINGLE_ROUND	0x0000
#define BMD_HASH_MULTI_ROUND	0x0001

/* liczenie szyfrogramu symetrycznego strumieniowo */
#define BMD_SYM_SINGLE_ROUND	0x0000
#define BMD_SYM_MULTI_ROUND	0x0001

#define BMD_HASH_STAGE_FIRST	0x0001
#define BMD_HASH_STAGE_NORMAL	0x0002
#define BMD_HASH_STAGE_LAST	0x0004
#define BMD_HASH_STAGE_COMPLETE	0x0008

#define BMD_SYM_STAGE_FIRST	0x0001
#define BMD_SYM_STAGE_NORMAL	0x0002
#define BMD_SYM_STAGE_LAST	0x0004
#define BMD_SYM_STAGE_COMPLETE	0x0008

#define BMD_SYM_PADDING_ABSENT	0x0000
#define BMD_SYM_PADDING_PRESENT	0x0001

#define BMD_PKCS11_AUTO_LOAD_CERT	0x0001


/*added by WSZ*/
#define TSTINFO_OID "1.2.840.113549.1.9.16.1.4"
/*!!!! poniżej FALSZYWY OID polityki TSA !!! nalezy poprawic na prawdziwy*/
#define TSA_POLICY_OID "1.2.3.4.5.6.7.8.9"   /*!!! UWAGA !!! FAKE !!!*/

#define TSA_POLICY_OID_LONG 1,2,3,4,5,6,7,8,9   /*!!! UWAGA !!! FAKE !!!*/
#define TSTINFO_OID_LONG 1,2,840,113549,1,9,16,1,4

#define BMDPKI_ERR_LOAD_SIG_POLICY -77
#define BMDPKI_ERR_FIND_CERT_STORE -78
/*/added by WSZ*/



#if defined(__cplusplus)
extern "C" {
#endif
	/* wskaznik na funkcje obrazujaca progress - np. podpisywania duzego pliku lub szyfrowania */
	typedef long (*progress_function)(void *,double,double,double,double,void *);

	/* szyfrowanie i deszyfrowanie asymetryczne */
	LIBBMDPKI_SCOPE long bmd_encrypt_data(GenBuf_t *input,bmd_crypt_ctx_t *ctx,GenBuf_t **output,void *opt);
	LIBBMDPKI_SCOPE long bmd_decrypt_data(	GenBuf_t **input,
						long twf_offset,
						bmd_crypt_ctx_t *ctx,
						GenBuf_t **output,
						void *opt,
						long twf_input_free);

	/* szyfrowanie i deszyfrowanie symetryczne */
	LIBBMDPKI_SCOPE long bmd_symmetric_encrypt(GenBuf_t *input,bmd_crypt_ctx_t **ctx,GenBuf_t **output,void *opt);
	LIBBMDPKI_SCOPE long bmd_symmetric_decrypt(GenBuf_t *input, long twf_offset, bmd_crypt_ctx_t **ctx, \
	GenBuf_t **output,void *opt);

	/* podpis */
	LIBBMDPKI_SCOPE long bmd_sign_data(GenBuf_t *input,bmd_crypt_ctx_t *ctx,GenBuf_t **output,void *opt);
	LIBBMDPKI_SCOPE long bmd_sign_file(const char *filename,bmd_crypt_ctx_t *sig_ctx,GenBuf_t **output,void *opt,
									  progress_data_t *pd);
	LIBBMDPKI_SCOPE long bmd_sign_file_wv(const char *filename,bmd_crypt_ctx_t *sig_ctx,GenBuf_t **output,void *opt,
									  progress_data_t *pd, GenBuf_t *verhash, long vrf_keys_corr, long vrf_sig_hash);								/* with verification */

	LIBBMDPKI_SCOPE long bmd_create_signature_params(bmd_signature_params_t **sig_params);
	LIBBMDPKI_SCOPE long bmd_set_signature_params_flag(bmd_signature_params_t *sig_params,long flag);
	LIBBMDPKI_SCOPE long bmd_set_signature_params_policy(bmd_signature_params_t *sig_params,char *sig_policy,
														long auto_params_create,GenBuf_t *cert,CertExtConfig_t *certCheckParams);
	LIBBMDPKI_SCOPE long bmd_set_signature_params_cert_store(bmd_signature_params_t *sig_params,char *tcc);


	LIBBMDPKI_SCOPE long bmd_destroy_signature_params(bmd_signature_params_t **sig_params);
	/* weryfikacja sciezki */
	LIBBMDPKI_SCOPE long bmd_verify_cert_path(GenBuf_t *cert,char *tcc,GenBuf_t ***cert_path,void *opt);
#ifdef WIN32
	long _find_CA_in_store(char *store_name, GenBuf_t* checkCert, GenBuf_t **found_CAcert);
	LIBBMDPKI_SCOPE long bmd_verify_cert_path_sysStore(GenBuf_t *checkCert, GenBuf_t ***cert_path);
#endif


	/* weryfikacja certyfikatu */
	LIBBMDPKI_SCOPE long bmd_verify_cert_with_crl(GenBuf_t *cert, char* crlURL, char* crlFilePath);
	LIBBMDPKI_SCOPE long bmd_verify_cert_with_cert(GenBuf_t *cert,GenBuf_t *issuer_cert);
	LIBBMDPKI_SCOPE long bmd_get_validity_time_from_cert(GenBuf_t *cert,time_t *notBefore,time_t *notAfter);
	LIBBMDPKI_SCOPE long bmd_cert_get_serial(GenBuf_t *cert,char **serial);

	/* skrot */
	LIBBMDPKI_SCOPE long bmd_hash_data(GenBuf_t *input,bmd_crypt_ctx_t **ctx,GenBuf_t **output,void *opt);
	LIBBMDPKI_SCOPE long bmd_hash_as_string(GenBuf_t *input,char **output,char sep,long uppercase);
	LIBBMDPKI_SCOPE long bmd_string_as_sha1(const char *strhash, GenBuf_t **outhash);
	LIBBMDPKI_SCOPE long bmd_hash_SignaturePolicy(GenBuf_t *sigPolicy, GenBuf_t** sigPolicyHash);

	/* znakowanie czasem */
	LIBBMDPKI_SCOPE long bmd_timestamp_data(GenBuf_t *input,long hash_algo,char *ts_server,long ts_port,
					       long transport_type,char *proxy_addr,long proxy_port,char *proxy_username,
					       char *proxy_password,long retry_count, long timeout, GenBuf_t **output);

	/* utworzenie kontekstu wraz z inicjalizacja wszyskich jego skladnikow */
	LIBBMDPKI_SCOPE long bmd_create_ctx(bmd_crypt_ctx_t **ctx,long ctx_source,long ctx_type);

	/* utworzenie kontekstu bmd_crypt_ctx z pliku - obecnie moze byc to PFX lub PEM */
	LIBBMDPKI_SCOPE long bmd_set_ctx_file(char *filename,char *pass,long passlen,bmd_crypt_ctx_t **ctx);
	/* utworzenie kontekstu bmd_crypt_ctx z pliku pfx wczytanego wczesniej do pamieci- obecnie moze byc to PFX lub PEM */
	LIBBMDPKI_SCOPE long bmd_set_ctx_fileInMem(GenBuf_t *file_buf,char *pass,long passlen,bmd_crypt_ctx_t **ctx);
	/* utworzenie kontekstu bmd_crypt_ctx z bufora przechowujacego zakodowana w base64 zawartosc pliku pfx*/
	LIBBMDPKI_SCOPE long bmd_set_ctx_from_base64content(char *encoded_content, char *pass, long passlen, bmd_crypt_ctx_t **set_ctx);

	/* reinicjalizacja kontekstu */
	LIBBMDPKI_SCOPE long bmd_ctx_reinit(bmd_crypt_ctx_t **ctx);

	/* ustawienie konteksu z PKCS#11 i pobranie uchwytu do klucza prywatnego */
	LIBBMDPKI_SCOPE long bmd_set_ctx_pkcs11(bmd_crypt_ctx_t **ctx,char *pkcs11_library,long ctx_type,long auto_load, BmdCtxPkcs11Option_e ctx_option);
	LIBBMDPKI_SCOPE long bmd_set_ctx_pkcs11_privkey(bmd_crypt_ctx_t **ctx,pkcs11_cert_t *sel_cert,char *pin,long pin_length);

	/* sprawdzenie kontekstu dla konkretnego typu - czy PKI, SYM czy HASH */
	LIBBMDPKI_SCOPE long bmd_check_ctx(bmd_crypt_ctx_t *ctx,long ctx_source,long ctx_type);
	LIBBMDPKI_SCOPE long bmd_ctx_destroy(bmd_crypt_ctx_t **ctx);
	LIBBMDPKI_SCOPE long bmd_set_ctx_sym(bmd_crypt_ctx_t **ctx,long sym_algo,GenBuf_t *key,GenBuf_t *IV);
	LIBBMDPKI_SCOPE long bmd_set_ctx_hash(bmd_crypt_ctx_t **ctx,long hash_algo);
	LIBBMDPKI_SCOPE long bmd_set_ctx_cert(bmd_crypt_ctx_t **ctx,GenBuf_t *cert);
	LIBBMDPKI_SCOPE long bmd_gen_sym_ctx_with_pass(		long crypt_algo,long hash_algo, char *pass, long pass_len, \
								bmd_crypt_ctx_t **ctx);

	LIBBMDPKI_SCOPE long bmd_get_cert_and_content_from_signature(GenBuf_t *input,GenBuf_t **output_cert,GenBuf_t **content);
	LIBBMDPKI_SCOPE long bmd_verify_sig_integrity(GenBuf_t *input,GenBuf_t *opt_content);
	LIBBMDPKI_SCOPE long bmd_check_keys_correspondence(GenBuf_t *signature, GenBuf_t *certificate);
	LIBBMDPKI_SCOPE long bmd_check_sign_with_hash(GenBuf_t *signature, GenBuf_t *hash);

	LIBBMDPKI_SCOPE long bmd_create_der_bmdKeyIV(long keyEncAlgo,long symAlgo,bmd_crypt_ctx_t *enc_ctx,GenBuf_t *sym_key,
						    GenBuf_t *sym_IV,GenBuf_t *enc_IV,GenBuf_t **output);

	LIBBMDPKI_SCOPE long bmd_decode_bmdKeyIV(GenBuf_t *input,GenBuf_t *dec_sym_key,bmd_crypt_ctx_t *dec_ctx,bmd_crypt_ctx_t **sym_ctx);

	LIBBMDPKI_SCOPE long bmd_generate_number(long byte_size,GenBuf_t **k);
	LIBBMDPKI_SCOPE long bmd_compute_ki_mod_n(GenBuf_t *k,GenBuf_t *i,GenBuf_t *n,GenBuf_t **result);
	LIBBMDPKI_SCOPE long bmd_compare_bn(GenBuf_t *a,GenBuf_t *b);

/* Funkcje nieeskportowalne */
	long file_sign_raw(void *info,GenBuf_t *input,GenBuf_t **output);
	long file_decrypt_raw(void *info,GenBuf_t *input,GenBuf_t **output);
	long file_encrypt_raw(void *info,GenBuf_t *input,GenBuf_t **output);
	long ks_sign_raw(void *info,GenBuf_t *input,GenBuf_t **output);
	long ks_decrypt_raw(void *info,GenBuf_t *input,GenBuf_t **output);
	long ks_s_prepare_ctx(void *info);
	long ks_ed_prepare_ctx(void *info);
	long ks_encrypt_raw(void *info,GenBuf_t *input,GenBuf_t **output);
	long __bmd_add_recipient(GenBuf_t *cert,bmd_encryption_params_t *options,GenBuf_t *symmetric_key, \
	EnvelopedData_t **envData);
	long _bmd_pkcs11_crypto_info_init(pkcs11_crypto_info_t **out);
	long _bmd_file_crypto_info_init(file_crypto_info_t **out);
	long _bmd_sym_crypto_info_init(symmetric_crypto_info_t **out);
	long _bmd_hash_crypto_info_init(hash_crypto_info_t **out);
	long _bmd_ks_crypto_info_init(ks_crypto_info_t **ks_info);
	long _bmd_gen_key_iv(GenBuf_t **sym_key,GenBuf_t **iv);
	long __bmd_check_ctx_source_and_type(long ctx_source,long ctx_type);
	long __bmd_cert_to_pubkey(GenBuf_t *cert,RSA **publicKey);
	long __bmd_create_signed_data(GenBuf_t *input,long flags,SignedData_t **sigData);
	long __bmd_create_signer_info(SignerInfo_t **signerInfo,SignedData_t **sigData,bmd_signature_params_t *opt,
                             GenBuf_t *input,bmd_crypt_ctx_t *ctx);
	long __bmd_add_signer_cert(GenBuf_t *cert,SignedData_t **sigData);
	long __bmd_decode_timestampresp(GenBuf_t *input,GenBuf_t **buf);
	long __bmd_get_enveloped_data(	GenBuf_t **input,
					long twf_offset,
					EnvelopedData_t **enveloped_data,
					long twf_input_free);
	long __bmd_check_and_get_encrypted_key(EnvelopedData_t *enveloped_data,GenBuf_t **output);
	long __bmd_check_and_get_IV_and_enc_content(EnvelopedData_t *enveloped_data,GenBuf_t **IV,GenBuf_t **enc_content);
	long __bmd_get_signed_data(GenBuf_t *input,SignedData_t **signed_data);
	long __bmd_get_signing_pubkey(SignedData_t *signed_data,RSA **public_key);
	long __bmd_compute_hash_for_sig_check(SignedData_t *signed_data,GenBuf_t *opt_content, \
	GenBuf_t **hash,GenBuf_t **sig_value);
	long _bmd_verify_RsaWithSHA1_signature(GenBuf_t *signature,GenBuf_t *der,GenBuf_t *pub_key);
	long _bmd_verify_RsaWithMD5_signature(GenBuf_t *signature,GenBuf_t *der,GenBuf_t *pub_key);
	long __pki_bmd_genbuf_hex_dump(GenBuf_t *input,char **output,char sep,long uppercase);

/*!by WSZ*/

/**nowy szyfrator i deszyfrator symetryczny*/
#define BMDPKI_SYM_NOT_LAST_CHUNK 0
#define BMDPKI_SYM_LAST_CHUNK 1

/*z bmdpki_sym.c*/
LIBBMDPKI_SCOPE long bmdpki_symmetric_encrypt(bmd_crypt_ctx_t *ctx_enc, GenBuf_t *data, long is_last, GenBuf_t **output);
LIBBMDPKI_SCOPE long bmdpki_symmetric_decrypt(bmd_crypt_ctx_t *ctx_dec, GenBuf_t *enc_buf, long is_last, GenBuf_t **output);


#define DVCS_RESPONSE_OID	"1.2.840.113549.1.9.16.1.8"
#define DVCS_REQUEST_OID	"1.2.840.113549.1.9.16.1.7"

#define DVCS_RESPONSE_OID_LONG 1, 2, 840, 113549, 1, 9, 16, 1, 8
#define DVCS_REQUEST_OID_LONG 1, 2, 840, 113549, 1, 9, 16, 1, 7


/*z sig_api.c*/
long bmd_sign_EncapsulatedContentInfo(EncapsulatedContentInfo_t *encap, bmd_crypt_ctx_t *ctx, ContentInfo_t **output,void *opt);
long bmd_sign_EncapsulatedContentInfo_DVCSReq(EncapsulatedContentInfo_t *encap, bmd_crypt_ctx_t *ctx, ContentInfo_t **output,void *opt);
/*z ts_api.c*/
long _read_ts_serial(char *file_name, long *sn, FILE **file);
long _write_ts_serial(char *file_name, long sn, FILE **file);
long _Create_PKIStatus(PKIStatus_e flag, PKIStatus_t *new_status);
long _Create_PKIFailureInfo(PKIFailureInfo_e flag, PKIFailureInfo_t **new_failureinfo);
long _encode_TSTInfo_to_eContent(TSTInfo_t *tst_info, OCTET_STRING_t **econtent);
long _Create_encapContentInfo(TSTInfo_t *tst_info, EncapsulatedContentInfo_t** encap);
long _encode_TimeStampResp(TimeStampResp_t* ts_resp, GenBuf_t** result);
long Create_TSTInfo(long oid_tsapolicy_tab[], long oid_N, TimeStampReq_t *ts_req, long serial, Certificate_t *cert, TSTInfo_t **tst_info);
long _Create_negativeResponse(PKIStatus_e status_enum, PKIFailureInfo_e failInfo_enum, GenBuf_t **negativeResp);
/*cmp_OID_with_longTab chwilowo w libbmdpki*/
long cmp_OID_with_longTab(OBJECT_IDENTIFIER_t *oid, long *tab, long N);
LIBBMDPKI_SCOPE long cmp_OID_with_charTab(	OBJECT_IDENTIFIER_t *oid, char *tab);

LIBBMDPKI_SCOPE long bmd_generate_timestamp(GenBuf_t *req, bmd_crypt_ctx_t *context, GenBuf_t **resp, void* opt);
/*!by WSZ*/

LIBBMDPKI_SCOPE long PKCS15toPKCS12(GenBuf_t *p15,const char *pass,long pass_len,GenBuf_t **p12,long debug);

LIBBMDPKI_SCOPE long bmd_set_ctx_ks(bmd_crypt_ctx_t **ctx,const char *adr, const long port,long ks_type);
#if defined(__cplusplus)
}
#endif
#endif
