#ifndef _BMD_CRYPTO_DS_INCLUDED_
#define _BMD_CRYPTO_DS_INCLUDED_

#include <bmd/common/bmd-sds.h>
#include <bmd/common/bmd-openssl.h>

/* msilewicz 21-09-2007 */
/* #define SAME_CTX	0 */		/*	 opcja konfiguracyjna na razie jako define */
						/* 0 - przy imporcie plików z serwera, każdy z nich ma indywidualnie ustawiany kontekst */
						/* 1 - kontekst jest wspólny dla wszystkich importowanych plików */

typedef struct pkcs11_cert {
	GenBuf_t *gb_cert;			/* certyfikat w DER */
	GenBuf_t *cka_id;			/* identyfikator certyfikatu */
	unsigned long slotID;		/*identyfikator slotu - sigillumowa/kirowa biblioteka pkcs#11, do jednego czytnika
								widzi dwa sloty ... rozroznianie slotow po nazwach dawalo bledne dzilanie*/
} pkcs11_cert_t;

typedef long (*generate_raw_signature_function)(void *,GenBuf_t *input,GenBuf_t **output);
typedef long (*raw_decrypt_function)(void *,GenBuf_t *input,GenBuf_t **output);

typedef long (*ks_exec_function)(void *,GenBuf_t *input,GenBuf_t **output);
typedef long (*ks_prepare_function)(void *);

/* struktura do realizacji operacji poprzez PKCS#11 */
typedef struct pkcs11_crypto_info {
	void * pFunctionList;				/* uchwyt do wskaznika funkcji - dispatch table */
	long hPrivateKey;			/* uchwyt do klucza prywatnego */
	long hSession;			/* uchwyt do sesji */
	long loggedIn;			/* informacja, czy zalogowany do tokena (w sesji) */
	long dontEstimateDecryptBufferSize; /* czy obliczac rozmiar bufora wyjsciowego przed wlasciwym deszyfrowaniem */
	void *pLibrary;					/* wartosc zwrocona przez dlopen/LoadLibrary */
	long hash_alg;				/* nazwa funkcji skrotu */
	GenBuf_t *cert;					/* certyfikat w DER odpowiadajacy danemu kluczowi prywatnemu */

	pkcs11_cert_t **all_certs;

	char *pin;	/*na potrzeby obslugi kart Cryptotech, zapamietywanie pin (by WSZ)*/
	long pinLen;	/*na potrzeby obslugi kart Cryptotech, zapamietywanie pin (by WSZ)*/

} pkcs11_crypto_info_t;

/* struktura do realizacji operacji poprzez plik */
typedef struct file_crypto_info {
	GenBuf_t *cert;					/* certyfikat X509 w DER */
	RSA *publicKey;					/* klucz publiczny */
	RSA *privateKey;				/* klucz prywatny */
	long hash_alg;				/* nazwa funkcji skrotu - tylko w przypadku podpisu */
	/*generate_raw_signature_function sig_function;
	raw_decrypt_function decrypt_function;*/
} file_crypto_info_t;

/* struktura do operacji z kryptografia symetryczna */
typedef struct symmetric_crypto_info {
	long algo_type;				/* typ algorytmu symetrycznego */
	GenBuf_t *IV;					/* wektor poczatkowy */
	GenBuf_t *key;					/* klucz */
	EVP_CIPHER_CTX sym_ctx;			/* tym kontekstem bedzie odbywalo sie szyfrowanie */
	char last_block[EVP_MAX_KEY_LENGTH];	/* miejsce na doppadingowane dane z ostatniego przebiegu */
	long last_block_length;				/* dlugosc danych dopaddingowanych */
	long sym_params;
	long sym_stage;
	long padding_present;
} symmetric_crypto_info_t;

/* struktura do obliczania skrotow */
typedef struct hash_crypto_info {
	long hash_alg;
	EVP_MD *md;
	EVP_MD_CTX *md_ctx;
	long hash_params;
	long hash_stage;
	char hash_value[EVP_MAX_MD_SIZE];
	long hash_size;
} hash_crypto_info_t;

/* msilewicz - definicja nowego typu kontekstu */
typedef struct ks_crypto_info {
	GenBuf_t *sig_cert;				/* certyfikat towarzyszacy kluczowi */
	GenBuf_t *ed_cert;				/* certyfikat do operacji szyfrowania/deszyfrowania */
	char *ks_adr;					/* adres serwera kluczy */
	long ks_port;				/* port serwera kluczy */
	long oper_type;			/* typ operacji - podpis lub deszyfrowanie */
	long ks_resp_status;				/* status operacji */
	GenBuf_t *output;				/* wynikowy bufor - podpis lub plain data */
	ks_exec_function sign;			/* funkcja komunikujaca sie z serwisem kluczy i realizujaca podpis */
	ks_exec_function decrypt;		/* funkcja komunikujaca sie z serwisem kluczy i realizujaca deszyfrowanie */
	ks_exec_function encrypt;		/* funkcja realizujaca szyfrowanie certyfikatem skojarzonym z kluczem pryw. do desz. */
	ks_prepare_function s_prepare;	/* funkcja pobiera certyfikat z serwisu kluczy dla podpisu */
	ks_prepare_function ed_prepare;	/* funkcja pobiera certyfikat z serwisu kluczy dla deszyfrowania/szyfrowania */
} ks_crypto_info_t;

/* kontekst kryptograficzny */
typedef struct bmd_crypt_ctx {
	long ctx_source;			/* typ kontekstu - plik, karta, windows */
	long ctx_type;				/* typ kontekstu - symetryczny, asymetryczny, skrot */
	pkcs11_crypto_info_t *pkcs11;
	file_crypto_info_t *file;
	symmetric_crypto_info_t *sym;
	hash_crypto_info_t *hash;
	ks_crypto_info_t *ks; /* msilewicz - nowy typ kontekstu */
} bmd_crypt_ctx_t;

typedef struct bmd_encryption_params {
	long encryption_type;			/* typ szyfrowania: wewnetrzne, zewnetrzne */
        long encryption_algo;                   /* algorytm symetryczny double zaszyfrowania tresci */
	GenBuf_t *opt_enc_content;			/* miejsce na opcjonalna zaszyfrowana zawartosc */
} bmd_encryption_params_t;

typedef struct bmd_signature_params {
        void *signed_attributes;        /* zestaw atrybutow podpisanych */
        void *unsigned_attributes;      /* zestaw atrybutow niepodpisanych */
        long signature_flags;   /* typ podpisu - zewnetrzny czy wewnetrzny oraz czy podpisujemy Large OBjecty */
		void *signature_policy;			/* wskaznik na strukture polityki podpisu */
		char *tcc;						/* wskaznik na Trusted Cert Store */
} bmd_signature_params_t;

typedef struct bmd_crypto_info {
	bmd_crypt_ctx_t *log_ctx;	/* kontekst na ktory logowano sie do archiwum - klucz prywatny, publiczny uzytkownika */
	bmd_crypt_ctx_t *bmd_ctx;	/* kontekst z certyfikatem archiwum */
	bmd_crypt_ctx_t *sig_ctx;	/* kontekst do podpisywania danych wysylanych do archiwum */
	bmd_crypt_ctx_t *sym_ctx;	/* kontekst do szyfrowania symetrycznego wysylanych danych */
} bmd_crypto_info_t;

typedef struct bmd_info {
	void *conn_handler;
	char *bmd_address;
	long bmd_port;

	bmd_proxy_info_t proxy_info;
	bmd_form_info_t form_info;
	bmd_crypto_info_t crypto_info;

	long request_id;			/* identyfikator requesta */

	GenBuf_t *k0;			/* wartosc "k0" wygenerowana przez klienta przy logowaniu */
	GenBuf_t *i;			/* wartosc "i" od serwera */
	GenBuf_t *n;			/* wartosc "n" od serwera */

	GenBuf_t *cur_ki_mod_n;		/* aktualna wartosc k^i mod n przechowywana przez klienta */
	GenBuf_t *server_ki_mod_n;	/* wartosc k^i mod n przechowywana przez klienta */

	GenBuf_t *hcsb;

	role_selection_callback chooseRole; /* callback double funkcji wybierajacej role */

	GenBufList_t *roleIdList;	/* lista identyfikatorow dostepnych rol */
	GenBufList_t *roleNameList;	/* lista nazw dostepnych rol (kolejnosc elementow zgodna z roleIdList) */
	long roleChosenIndex;		/* indeks wybranej roli w liscie roleIdList oraz roleNameList */
	long defaultRoleIndex;		/* indeks domyslnej roli uzytkownika w liscie roleIdList oraz roleNameList */

	GenBufList_t *groupIdList;		/* lista identyfikatorow dostepnych grup */
	GenBufList_t *groupNameList;	/* lista nazw dostepnych grup (kolejnosc elementow zgodna z groupIdList) */
	long groupChosenIndex;			/* indeks wybranej grupy w liscie groupIdList oraz groupNameList */
	long defaultGroupIndex;			/* indeks domyslnej grupy uzytkownika w liscie groupIdList oraz groupNameList */

	GenBufList_t *classIdList;		/* lista identyfikatorow dostepnych grup */
	GenBufList_t *classNameList;	/* lista nazw dostepnych grup (kolejnosc elementow zgodna z groupIdList) */
	long defaultClassIndex;			/* indeks domyslnej grupy uzytkownika w liscie groupIdList oraz groupNameList */

	GenBuf_t *cert_login_as;

	long *actions;
	long no_of_actions;

	char *server_label;

} bmd_info_t;

typedef struct lob_request_info {
	char **files_to_send;		    /* nazwy wraz z pelnymi sciezkami plikow do wyslania */
	long no_of_files_to_send;		    /* ilosc plikow do wyslania */

	GenBuf_t* buffers_to_send;		/* tablica buforow z danymi do wysylki (zamiast wysylki z plikow) (files_to_send jednoczesnie potrzebna)*/
	long no_of_buffers_to_send;		/* ilosc buforow do wyslania */

	char **files_to_recv;		    /* nazwy wraz z pelnymi sciezkami plikow do pobrania */
	long *recv_files_sizes;    /* lista z rozmiarami plikow do pobrania */
	long no_of_files_to_recv;		    /* ilosc plikow do pobrania */

	double total_file_size;		    /* calkowity rozmiar dodawanych lub pobieranych plikow */
	bmd_crypt_ctx_t **lob_contexts;	    /* konteksty uzywane do szyfrowania lub deszyfrowania LOB */

	long show_send_progress;		    /* pokaz progress wysylki */
	long show_recv_progress;		    /* pokaz progress odbioru */
	struct progress_data send_prog_data;    /* dane do progresu wysylki */
	struct progress_data recv_prog_data;    /* dane do progresu odbioru */
} lob_request_info_t;

#endif
