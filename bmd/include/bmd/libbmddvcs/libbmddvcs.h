
/*Autor: Włodzimierz Szczygieł ( Dział Programowania, Unizeto Technologies SA )*/

#ifndef LIBBMDDVCS_H_
#define LIBBMDDVCS_H_


#include <bmd/libbmdcms/libbmdcms.h>
#include <bmd/libbmdasn1_common/DVCSResponse.h>
#include <bmd/libbmdasn1_common/Data.h>
#include <bmd/libbmdasn1_common/DVCSRequest.h>
#include <bmd/libbmdasn1_common/EncapsulatedContentInfo.h>
#include <bmd/libbmdpki/libbmdpki.h>
/*
 * #include <bmd/libbmdutils/libbmdutils.h>
//!! #define DVCS_RESPONSE_OID 1, 2, 840, 113549, 1, 9, 16, 1, 8  ---- przeniosione do libbmdpki.h
//!! #define DVCS_REQUEST_OID 1, 2, 840, 113549, 1, 9, 16, 1, 7 ---- przeniesione do libbmdpki.h
*/

/** ponizsze define dla funkcji _create_DVCSRequest */
#define SERVICE_TYPE_CPD	ServiceType_cpd		/*Certification of Possession of Data*/
#define SERVICE_TYPE_CCPD	ServiceType_ccpd	/*Certification of Claim of Possession of Data*/
#define SERVICE_TYPE_VSD	ServiceType_vsd		/*Validation of Digitally Signed Documents*/
#define SERVICE_TYPE_VPKC	ServiceType_cpkc	/*Validation of Public Key Certificates*/

/** dodatkowe definicje tylko dla SERVICE_TYPE_CPD */
#define BMDDVCS_HASH_AT_INPUT	0 
#define BMDDVCS_COUNT_HASH	1

/** dopuszczalne wartosci externalDocumentType (document_type jako parametr funkcji API)*/
#define BMDDVCS_EXTERNAL_DOCUMENT_CONTENT	1
#define BMDDVCS_EXTERNAL_DOCUMENT_HASH		2


struct bmd_dvcs
{
	char *url;
	GenBuf_t * signedData; //ContentInfo_t* signedData; // dla VSD
	char* hashAlgorithmOid; // na potrzeby weryfikacji PAdES - oid funkcji skrotu wykorzystanej do wyliczenia skrotu z czesci podpisanej PDF // dla VSD
	DVCSTime_t* tsTime; // znacznik czasu dla weryfikowanego podpisu badz czas wyciagniety ze znacznika // dla VSD
	GenBuf_t** tsAttachments; // tablica zalaczanych znacznikow czasu // dla VSD (podpis zewnetrzny)
	long tsAttachmentsCount; // ilosc znacznikow w tsAttachments // dla VSD (podpis zewnetrzny)
	GenBuf_t* data; //dla CPD oraz CCPD (dane)
	Certificate_t *certificate; // dla VPKC
	GenBuf_t *dvcsCert; //wystawione poswiadczenie
	char *errorString; //string z bledem przetwarzania zadania
	long connectionTimeout; //timeout na polaczenie z serwerem DVCS
	long externalDocumentType; //BMDDVCS_EXTERNAL_DOCUMENT_CONTENT lub BMDDVCS_EXTERNAL_DOCUMENT_HASH
};
typedef struct bmd_dvcs bmdDVCS_t;


/*public API*/
/*dvcs_api.c*/
long bmdDVCS_load_Sig(GenBuf_t *signature, bmdDVCS_t **input_data);
long bmdDVCS_load_fileSig(char *file_name, bmdDVCS_t **input_data);
long bmdDVCS_set_server(bmdDVCS_t *input_data, char *url);
long bmdDVCS_set_connectionTimeout(bmdDVCS_t *input_data, long timeout);
long bmdDVCS_set_proof_Time(bmdDVCS_t* input_data, GenBuf_t* binaryTimestampToken);
long bmdDVCS_set_proof_TimestampToken(bmdDVCS_t* input_data, GenBuf_t* binaryTimestampToken);
long bmdDVCS_clear_proof(bmdDVCS_t* input_data);
long bmdDVCS_add_timestamp_attachment(bmdDVCS_t* input_data, GenBuf_t* timestampBuf);
long bmdDVCS_clear_timestamp_attachments(bmdDVCS_t* input_data);
long bmdDVCS_set_hash_algorithm(bmdDVCS_t* input_data, char* hashAlgorithmOid);
long bmdDVCS_validate_signature(bmdDVCS_t *input_data, bmd_crypt_ctx_t *context);
long bmdDVCS_validate_externalSignature(bmdDVCS_t *input_data, GenBuf_t *document, long document_type, bmd_crypt_ctx_t *context);

long bmdDVCS_load_Cert(char *pkcs11_library_path, bmdDVCS_t **input_data, long cert_num);
long bmdDVCS_load_fileCert(char *pfx_path, char *password, bmdDVCS_t **input_data);
long bmdDVCS_load_memCert(GenBuf_t *cert, bmdDVCS_t **input_data);
long bmdDVCS_verify_cert(bmdDVCS_t *input_data, bmd_crypt_ctx_t *context);

long bmdDVCS_load_data(GenBuf_t *data, bmdDVCS_t **input_data);
long bmdDVCS_load_fileData(char *file_name, bmdDVCS_t **input_data);
long bmdDVCS_certify_possession_of_data(bmdDVCS_t *input_data, bmd_crypt_ctx_t *context);
long bmdDVCS_certify_existence_of_data(bmdDVCS_t *input_data, bmd_crypt_ctx_t *context, long countHashFlag);

long bmdDVCS_get_dvcsCert(bmdDVCS_t *input_data, GenBuf_t **dvcsCert);
long bmdDVCS_get_errString(bmdDVCS_t *input_data, char** errString);
long bmdDVCS_destroy(bmdDVCS_t **input_data);


/* Private API */

/*dvcs.c*/
long _create_DVCSRequestInformation(bmdDVCS_t *input, ServiceType_e serviceType, DVCSRequestInformation_t **reqInformation);
long _create_Data(Data_t **data);
long _add_to_Data_VSD(GenBuf_t *signedData, Data_t *data);
/*long _add_to_Data_VSD(ContentInfo_t *signedData, Data_t *data); XADES!! */
long _add_to_Data_CPD(GenBuf_t *der, Data_t *data);
long _add_to_Data__CCPD(GenBuf_t *data_for_hash, Data_t *data, long countHashFlag);
long _create_CertEtcToken(CertEtcToken_PR flag, Certificate_t *certificate, CertEtcToken_t **cert_token);

/* poniższe dwa definy dedykowane dla funkcji _create_TargetEtcChain() oraz _add_to_Data_CPKC() */
#define CERT_CONTENT CertEtcToken_PR_certificate
#define CERT_HASH CertEtcToken_PR_esscertid
long _create_TargetEtcChain(Certificate_t *certificate, long flag, TargetEtcChain_t **target_chain);
long _add_to_Data_VPKC(Certificate_t *certificate, long flag, Data_t *data);
void _destroy_DVCSRequest(DVCSRequest_t **req);
long _create_DVCSRequest(bmdDVCS_t *input, long flag, DVCSRequest_t **request, long countHashFlag);
long _encode_DVCSRequest_to_eContent(DVCSRequest_t *req, OCTET_STRING_t **econtent);
long _Create_encapContentInfo_DVCSReq(DVCSRequest_t *req, EncapsulatedContentInfo_t** encap);
long _encode_DVCSRequest_to_genbuf(DVCSRequest_t *req, GenBuf_t **bufor);
long _get_respStatus(DVCSResponse_t *response, long *status);
long _prepare_DVCSReq_to_send(bmdDVCS_t *input , long service_type ,bmd_crypt_ctx_t *context, GenBuf_t **req_buf, long countHashFlag);
void _destroy_DVCSResponse(DVCSResponse_t **resp);
long _get_dvcsCert(DVCSResponse_t *resp, GenBuf_t **dvcsCert);


long call_DVCSservice_signed(long service_type, bmd_crypt_ctx_t *context, bmdDVCS_t *input, long *result, long countHashFlag);

/*long call_DVCSservice(char *dvcs_host, long dvcs_port, long service_type, bmdDVCS_t *input);*/
//long test_external(long service_type, bmd_crypt_ctx_t *context, bmdDVCS_t *input, long *result, DVCSResponse_t **server_resp);

long decode_DVCSResponse(GenBuf_t *resp_buf, DVCSResponse_t **resp);
long _create_Encapsulated_DVCSRequest(bmdDVCS_t *input, long flag, EncapsulatedContentInfo_t **encap, long countHashFlag);

/*dvcs_externalsig.c*/
long call_DVCSservice_externalSig(GenBuf_t* document, bmd_crypt_ctx_t *context, bmdDVCS_t *input, long *result);
long _create_HTTPbody_externalSig(GenBuf_t* DVCSrequest, GenBuf_t *document, GenBuf_t** tsAttachments, long tsAttachmentsCount, char *boundary, GenBuf_t **output);

long _add_tsAttachment(bmdDVCS_t *input, GenBuf_t *timestampAttachment);
long _destroy_tsAttachments(bmdDVCS_t *input);


#endif /*LIBBMDDVCS_H_*/
