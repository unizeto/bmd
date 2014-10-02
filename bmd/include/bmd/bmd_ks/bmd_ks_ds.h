#ifndef _BMD_KS_DS_INCLUDED_
#define _BMD_KS_DS_INCLUDED_

#ifdef WIN32
#include <pkcs11/cryptoki.h>
#else
	#define CK_PTR *
	#define CK_DEFINE_FUNCTION(returnType, name) returnType name
	#define CK_DECLARE_FUNCTION(returnType, name) returnType name
	#define CK_DECLARE_FUNCTION_POINTER(returnType, name) returnType (* name)
	#define CK_CALLBACK_FUNCTION(returnType, name) returnType (* name)
	#ifndef NULL_PTR
		#define NULL_PTR 0
	#endif
	#include<pkcs11.h>
#endif

#include <bmd/bmd_ks/bmd_ks_const.h>
#include <bmd/libbmdks/bmd_ks_cli.h>
#include <bmd/bmd_ks/msg.h>

typedef struct svc_conf {
        char *adr;
        long port;
        char *sig_key_id;
        char *dec_key_id;
        char *pin;
        char *pkcs11_lib;
        long max_threads;
	long log_level;
	long timeout_sec;
	char *log_dir;
} svc_conf_t;

typedef struct svc_p11 {
        HMODULE lHandle;
        CK_C_GetFunctionList pC_GetFunctionList;
        CK_FUNCTION_LIST_PTR pFunctionList;
        CK_SLOT_ID_PTR slots;
        CK_SESSION_HANDLE *hSigSes;
	char *sig_cert;
	long sig_cert_length;
	char *ed_cert;
	long ed_cert_length;
        CK_SESSION_HANDLE *hDecSes;
        CK_OBJECT_HANDLE *hSigKey;
        CK_OBJECT_HANDLE *hDecKey;
        long _sig_slot;
        long _dec_slot;
} svc_p11_t;

typedef struct ks_thread_data {
	long thread_number;
	SOCKET s;
} ks_thread_data_t;

#endif
