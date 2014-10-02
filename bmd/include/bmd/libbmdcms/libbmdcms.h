#ifndef _LIBBMDCMS_H_
#define _LIBBMDCMS_H_

#include <bmd/libbmdasn1_core/linuxwindows.h>
#include <bmd/libbmdasn1_common/ContentInfo.h>   /* CMS ASN.1 type  */
#include <bmd/libbmdasn1_common/SignedData.h>
#include <bmd/libbmdasn1_common/EnvelopedData.h>
#include <bmd/common/bmd-sds.h>
#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/common/bmd-crypto_ds.h>
#include <bmd/common/bmd-os_specific.h>

/*added by WSZ <start>*/
#include <bmd/libbmdasn1_common/SigPolicy/SignaturePolicyIdentifier.h>
#include <bmd/libbmdasn1_common/SigPolicy/SPuri.h>
#include <bmd/libbmdasn1_common/SigPolicy/SPUserNotice.h>
#include <bmd/libbmdasn1_common/SigPolicy/SignaturePolicy.h>

/*atrybuty w podpisie kwalifikowanym*/
#include<bmd/libbmdasn1_common/SigPolicy/Hash.h>
#include<bmd/libbmdasn1_common/SigPolicy/ESSCertID.h>
#include<bmd/libbmdasn1_common/SigPolicy/SigningCertificate.h>
#include<bmd/libbmdasn1_common/IssuerSerial.h>
#include<bmd/libbmdasn1_common/SigningTime.h>
/*#include<bmd/libbmdpki/libbmdpki.h>*/
/*----do timestampa*/
#include <bmd/libbmdasn1_common/TimeStampReq.h>
#include <bmd/libbmdasn1_common/TimeStampResp.h>

#include <bmd/libbmdasn1_common/MessageDigest.h>

/*added by WSZ <end> */


#include <stdio.h>
#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define LIBBMDCMS_SCOPE
#if 0 /*biblioteka linkowania statycznie, wiec export jest niepotrzeby */
#ifdef WIN32
	#ifdef LIBBMDCMS_EXPORTS
		#define LIBBMDCMS_SCOPE __declspec(dllexport)
	#else
		#ifdef _USRDLL
			#define LIBBMDCMS_SCOPE __declspec(dllimport)
		#else
			#define LIBBMDCMS_SCOPE
		#endif
	#endif
#else
	#define LIBBMDCMS_SCOPE
#endif /* ifdef WIN32 */
#endif /* if 0 */


#if defined(__cplusplus)
extern "C" {
#endif

#define ERR_NO_MEMORY 						-98765001
#define ERR_WRONG_ARGUMENT					-98765002
#define ERR_DECODE_CMS_DATA					-98765003
#define ERR_DECODE_SIGNED_DATA				-98765004
#define ERR_NO_CERIFICATES_PRESENT_IN_SIGNED_DATA	-98765005
#define ERR_CERTIFICATE_ENCODING_FAILED			-98765006
#define ERR_OPEN_FILE						-98765007
#define ERR_ENCODE						-98765008
#define ERR_OPERATION_FAILED					-98765009
#define ERR_READ_FILE						-98765010
#define READ_CHUNKSIZE					512

#define OID_CMS_ID_DATA_LONG				1,2,840,113549,1,7,1
#define OID_CMS_ID_SIGNED_DATA_LONG			1,2,840,113549,1,7,2
#define OID_CMS_ID_ENVELOPED_DATA_LONG		1,2,840,113549,1,7,3
#define OID_CMS_ID_MESSAGE_DIGEST_LONG		1,2,840,113549,1,9,4
#define OID_CMS_ID_DIGESTED_DATA_LONG		1,2,840,113549,1,7,5
#define OID_CMS_ID_ENCRYPTED_DATA_LONG		1,2,840,113549,1,7,6
#define OID_CMS_ID_CT_AUTH_DATA_LONG		1,2,840,113549,1,9,16,1,2
#define OID_CMS_ID_COUNTERSIGNATURE_LONG		1,2,840,113549,1,9,6

#define OID_CMS_ID_DATA				"1.2.840.113549.1.7.1"
#define OID_CMS_ID_SIGNED_DATA		"1.2.840.113549.1.7.2"
#define OID_CMS_ID_ENVELOPED_DATA		"1.2.840.113549.1.7.3"
#define OID_CMS_ID_MESSAGE_DIGEST		"1.2.840.113549.1.9.4"
#define OID_CMS_ID_DIGESTED_DATA		"1.2.840.113549.1.7.5"
#define OID_CMS_ID_ENCRYPTED_DATA		"1.2.840.113549.1.7.6"
#define OID_CMS_ID_CT_AUTH_DATA		"1.2.840.113549.1.9.16.1.2"
#define OID_CMS_ID_COUNTERSIGNATURE		"1.2.840.113549.1.9.6"

#define OID_RSAENCRYPTION_LONG			1,2,840,113549,1,1,1
#define OID_MD2WITHRSAENCRYPTION_LONG		1,2,840,113549,1,1,2
#define OID_MD4WITHRSAENCRYPTION_LONG		1,2,840,113549,1,1,3
#define OID_MD5WITHRSAENCRYPTION_LONG		1,2,840,113549,1,1,4
#define OID_SHA1WITHRSAENCRYPTION_LONG		1,2,840,113549,1,1,5
#define OID_ID_DSA_WITH_SHA1_LONG			1,2,840,10040,4,3
#define OID_ID_DSA_LONG					1,2,840,10040,4,1
#define OID_ATTRIBUTE_CONTENTTYPE_LONG		1,2,840,113549,1,9,3
#define OID_ATTRIBUTE_MESSAGEDIGEST_LONG		1,2,840,113549,1,9,4

#define OID_RSAENCRYPTION			"1.2.840.113549.1.1.1"
#define OID_MD2WITHRSAENCRYPTION		"1.2.840.113549.1.1.2"
#define OID_MD4WITHRSAENCRYPTION		"1.2.840.113549.1.1.3"
#define OID_MD5WITHRSAENCRYPTION		"1.2.840.113549.1.1.4"
#define OID_SHA1WITHRSAENCRYPTION		"1.2.840.113549.1.1.5"
#define OID_ID_DSA_WITH_SHA1			"1.2.840.10040.4.3"
#define OID_ID_DSA				"1.2.840.10040.4.1"
#define OID_ATTRIBUTE_CONTENTTYPE		"1.2.840.113549.1.9.3"
#define OID_ATTRIBUTE_MESSAGEDIGEST		"1.2.840.113549.1.9.4"

/*added by WSZ <BEGIN>*/
#define BMDCMS_UNSIGNED_ATTRIBUTE 0
#define BMDCMS_SIGNED_ATTRIBUTE 1
#define BMDCMS_SP_ATTR 2
#define BMDCMS_SP_NULL 3
#define BMDCMS_SP_URI 4
#define BMDCMS_SP_USERNOTICE 5
#define BMD_SP_VISIBLE 	6
#define BMD_SP_BMP 	7
#define BMD_SP_UTF8 	8

#define OID_CMS_SIG_TS_TOKEN 	1, 2, 840, 113549, 1, 9, 16, 2, 14  /*OID SignatureTimeStampToken*/
#define OID_CMS_SIG_TIME 	1, 2, 840, 113549, 1, 9, 5  /*OID SigningTime*/
#define OID_CMS_SIG_POLICY	1, 2, 840, 113549, 1, 9, 16, 2, 15  /*OID SignaturePolicy*/
#define OID_CMS_URI		1, 2, 840, 113549, 1, 9, 16, 5, 1  /*OID URI (id-spq-ets-uri)*/
#define OID_CMS_USER_NOTICE	1, 2, 840, 113549, 1, 9, 16, 5, 2  /*OID USERNOTICE (id-spq-ets-unotice)*/

#define SIGNING_TIME_OID 1, 2, 840, 113549, 1, 9, 5
#define SIGNING_CERT_OID 1, 2, 840, 113549, 1, 9, 16, 2, 12
/*added by WSZ <END>*/

LIBBMDCMS_SCOPE long get_first_certificate_from_SignedData(SignedData_t *SignedData, GenBuf_t **GenBuf);
LIBBMDCMS_SCOPE long GetX509CertListFromCMSSignature(GenBuf_t *FileBuf, GenBufList_t **X509CertificatesList);
LIBBMDCMS_SCOPE long GetX509CertListFromSignedData(SignedData_t *SignedData, GenBufList_t **X509CertificatesList);

LIBBMDCMS_SCOPE long get_CMScontent_from_GenBuf(GenBuf_t *gbuf, ContentInfo_t **cms);
LIBBMDCMS_SCOPE long get_SignedData_from_CMScontent(ANY_t *content, SignedData_t **SignedData);
LIBBMDCMS_SCOPE long der_encode_X509_certificate(Certificate_t *x509cert, GenBuf_t **gbuf);
LIBBMDCMS_SCOPE long der_encode_SignedData(SignedData_t *sigData, GenBuf_t **buf);
LIBBMDCMS_SCOPE long SignedData_set_CMSVersion(SignedData_t *sigData, CMSVersion_e version);
LIBBMDCMS_SCOPE long SignedData_add_DigestAlgorithm(SignedData_t *sigData, DigestAlgorithmIdentifier_t *digest);
LIBBMDCMS_SCOPE long ContentInfo_set_SignedData(ContentInfo_t* content, SignedData_t* sigData);
LIBBMDCMS_SCOPE long der_encode_ContentInfo(ContentInfo_t *contentInfo, GenBuf_t **buf);
LIBBMDCMS_SCOPE long SignedData_add_Certificate(SignedData_t *sigData, Certificate_t *cert);

LIBBMDCMS_SCOPE long AlgorithmIdentifier_set_OID(AlgorithmIdentifier_t *digest,long* oid_table,long oid_size);

LIBBMDCMS_SCOPE long SignedData_set_Content(SignedData_t *sigData,GenBuf_t *buf);

LIBBMDCMS_SCOPE long SignedData_set_ContentType(SignedData_t *sigData,long *oid_tab,long oid_size);

LIBBMDCMS_SCOPE long SignedData_set_Data(SignedData_t *sigData,GenBuf_t *buf);

LIBBMDCMS_SCOPE long AlgorithmIdentifier_set_OID(AlgorithmIdentifier_t *digest,long* oid_table,long oid_size);

LIBBMDCMS_SCOPE long SignerInfo_generate_Signature(SignerInfo_t *signerInfo,bmd_crypt_ctx_t *ctx,long flags,
				SignedAttributes_t *sigattrs,UnsignedAttributes_t *unsigattrs,GenBuf_t *buf);

LIBBMDCMS_SCOPE long SignedData_add_SignerInfo(SignedData_t *sigData,SignerInfo_t *sigInfo);

LIBBMDCMS_SCOPE long SignedData_count_Version(SignedData_t *sigData);

long ContentInfo_get_SignedData(ContentInfo_t *content,SignedData_t **sigData);

long SignedData_get_Content(SignedData_t *sigData,GenBuf_t *data);

long AlgorithmIdentifier_cmp_OID(AlgorithmIdentifier_t *algId,long *oid_tab,long oid_size);

long CertificateSet_is_Certificate(CertificateSet_t *certs, Certificate_t *cert);

long Attribute_generate_ContentType_Data(Attribute_t **attribute1);

long Attribute_generate_MessageDigest(Attribute_t **attribute1,GenBuf_t *buf,long flags);

long isAnyCertOther(CertificateSet_t *certs);

long isAnyCertWithVer(CertificateSet_t *certs, long ver);

long isAnyCRLOther(RevocationInfoChoices_t *crls);

long RecipientInfo_generate(RecipientInfo_t *recInfo,GenBuf_t *certificate,GenBuf_t *cipherKey);

long EnvelopedData_add_RecipientInfo(EnvelopedData_t *envData, RecipientInfo_t *recInfo);

long EnvelopedData_add_RecipientInfo(EnvelopedData_t *envData, RecipientInfo_t *recInfo);

long EnvelopedData_add_Certificate(EnvelopedData_t *envData, Certificate_t *cert);

long EnvelopedData_count_Version(EnvelopedData_t *envData);

long ContentInfo_set_EnvelopedData(ContentInfo_t* content, EnvelopedData_t* envData);

long RecipientInfos_has_RecipientInfo_present(RecipientInfos_t *recInfos,RecipientInfo_PR present);

long RecipientInfos_allVersion(RecipientInfos_t *recInfos,CMSVersion_e version);

long Name_count_hash(Name_t *name, char *hash, long flags);

long SignedAttributes_add_Attribute(SignedAttributes_t *attrs,Attribute_t *attr);

long SignerInfo_add_SignedAttributes(SignerInfo_t *signerInfo,SignedAttributes_t *sigattrs);

long SignerInfo_add_UnsignedAttributes(SignerInfo_t *signerInfo,UnsignedAttributes_t *unsigattrs);

long EnvelopedData_set_EncryptedData(EnvelopedData_t *envData,GenBuf_t *iv,GenBuf_t *data,bmd_encryption_params_t *options);

/*WSZ - opis funkcji w pliku src/libbmdcms/cms_extension.c*/
/*Signing policy identifier-----*/
LIBBMDCMS_SCOPE long Create_DisplayText(char *str, long flag, DisplayText_t **dText);
LIBBMDCMS_SCOPE long Create_NoticeReference(DisplayText_t *dText, NoticeReference_t **NoticeRef);
LIBBMDCMS_SCOPE void Destroy_DisplayText(DisplayText_t **dText);
LIBBMDCMS_SCOPE long AddInt2NoticeReference(long val, NoticeReference_t *NoticeRef);
LIBBMDCMS_SCOPE long Create_SPUserNotice(NoticeReference_t *nRef, DisplayText_t *dText, SPUserNotice_t **SPun);
LIBBMDCMS_SCOPE long Create_SPuri(char *str, SPuri_t **SPuri);
LIBBMDCMS_SCOPE long Create_SigPolicyQualifierInfo(void *item, long flag, SigPolicyQualifierInfo_t **SPqi);
LIBBMDCMS_SCOPE void Destroy_SPUserNotice(SPUserNotice_t **SPun);
LIBBMDCMS_SCOPE void Destroy_SPuri(SPuri_t **destroy_spuri);
LIBBMDCMS_SCOPE long Create_SigPolicyQualifiers(SigPolicyQualifiers_t **SPq);
LIBBMDCMS_SCOPE long Add2SigPolicyQualifiers(SigPolicyQualifierInfo_t *new_item, SigPolicyQualifiers_t *SPq);
LIBBMDCMS_SCOPE long Create_SigPolicyId_char(char *oid, SigPolicyId_t **SPId);
LIBBMDCMS_SCOPE long Create_SigPolicyId(long *oid, long N, SigPolicyId_t **SPId);
LIBBMDCMS_SCOPE long Create_AlgorithmIdentifier_char(char* oid, ANY_t* any, AlgorithmIdentifier_t **algId);
LIBBMDCMS_SCOPE long Create_AlgorithmIdentifier(long *oid, long N, ANY_t* any, AlgorithmIdentifier_t **algId);
LIBBMDCMS_SCOPE long Create_SigPolicyHash(AlgorithmIdentifier_t *algId, OtherHashValue_t* othHashVal, SigPolicyHash_t **SPh);
LIBBMDCMS_SCOPE void Destroy_AlgorithmIdentifier(AlgorithmIdentifier_t **algId);
LIBBMDCMS_SCOPE void Destroy_OtherHashValue_spec(OtherHashValue_t **ohv);
LIBBMDCMS_SCOPE void Destroy_OtherHashValue(OtherHashValue_t **ohv);
LIBBMDCMS_SCOPE long Create_SignaturePolicyId(SigPolicyId_t *sigP_Id, SigPolicyHash_t *sigP_Hash, SigPolicyQualifiers_t *sigP_Qualifiers, SignaturePolicyId_t **SPId);
LIBBMDCMS_SCOPE void Destroy_SigPolicyId(SigPolicyId_t **destroy_SPId);
LIBBMDCMS_SCOPE void Destroy_SigPolicyHash(SigPolicyHash_t **SPh);
LIBBMDCMS_SCOPE long Create_SignaturePolicyIdentifier(long flag, SignaturePolicyId_t *spId, SignaturePolicyIdentifier_t **newSP);
LIBBMDCMS_SCOPE void Destroy_SignaturePolicyId(SignaturePolicyId_t **SPId);
LIBBMDCMS_SCOPE long CreateAttribute_SignaturePolicy(SignaturePolicyIdentifier_t *sigPol, Attribute_t **attribute1);

/*!! FUNKCJE PUBLICZNE*/
/*identyfikator polityki podpisu - atrybut podpisany*/
LIBBMDCMS_SCOPE long SPId_SPIdentifier_Create(char* SP_oid, char* hashalg_oid, ANY_t* params, OtherHashValue_t* ohv, SignaturePolicyIdentifier_t** SPId);
LIBBMDCMS_SCOPE long SPId_Uri_Add(char *uri, SignaturePolicyIdentifier_t *dest);
LIBBMDCMS_SCOPE long SPId_UserNotice_Add(char* text, char* organization, long int* noticeNums, long N, SignaturePolicyIdentifier_t* dest);
LIBBMDCMS_SCOPE long SPId_SPIdentifier_CreateAttribute(SignaturePolicyIdentifier_t *sigPol, Attribute_t **attribute1);
LIBBMDCMS_SCOPE void SPId_SPIdentifier_Destroy(SignaturePolicyIdentifier_t **SP);

LIBBMDCMS_SCOPE long SPId_GetSPIdentifierfromAttribute(Attribute_t *attr, SignaturePolicyIdentifier_t** SPId);
LIBBMDCMS_SCOPE long SPId_GetSPIdentifierfromSignedAttributes(SignedAttributes_t* sigattr, SignaturePolicyIdentifier_t **SPId);
LIBBMDCMS_SCOPE long SPId_SPIdentifierAttribute_Destroy(Attribute_t **attr);
/*polityka podpisu*/
LIBBMDCMS_SCOPE long Decode_spol(GenBuf_t *src_der, SignaturePolicy_t **SP);
LIBBMDCMS_SCOPE void Destroy_spol(SignaturePolicy_t **SP);
/*SigningTime - atrybut podpisany -- nieaktualne*/
/*
LIBBMDCMS_SCOPE long Create_SigningTimeAttribute(TimeStampToken_t *SigTSToken, Attribute_t **attribute);
LIBBMDCMS_SCOPE long Add_Attribute2Attrs(SignerInfo_t *signer, Attribute_t *attribute, long flag);
*/

/**podpis kwalifikowany - atrybuty*/
LIBBMDCMS_SCOPE long SigAttr_Create_SigningTime(Attribute_t **sigtime_attr);
LIBBMDCMS_SCOPE long SigAttr_Create_SigningCertificate(GenBuf_t *cert_hash, Certificate_t *cert, Attribute_t **sigcert_attr);
LIBBMDCMS_SCOPE long SigAttr_Create_SignaturePolicyId(char *spol_oid, char *uri, GenBuf_t *spol_hash, Attribute_t **sigpolid_attr);
LIBBMDCMS_SCOPE long SigAttr_Create_ContentTimestamp(GenBuf_t *ts_resp, GenBuf_t *sent_hash, Attribute_t **attr);
LIBBMDCMS_SCOPE long SignedAtributes_init(SignedAttributes_t **signed_attrs);
LIBBMDCMS_SCOPE long SignedAtributes_add(Attribute_t *new_attr, SignedAttributes_t *signed_attrs);
LIBBMDCMS_SCOPE long UnsignedAtributes_init(UnsignedAttributes_t **unsigned_attrs);
LIBBMDCMS_SCOPE long UnsignedAtributes_add(Attribute_t *new_attr, UnsignedAttributes_t *unsigned_attrs);
LIBBMDCMS_SCOPE long SigningParams_Create(SignedAttributes_t *sig_attrs, UnsignedAttributes_t *unsig_attrs, bmd_signature_params_t **params);
LIBBMDCMS_SCOPE void SigningParams_Destroy(bmd_signature_params_t **params);
LIBBMDCMS_SCOPE long Certificate_from_ctx(bmd_crypt_ctx_t *ctx, Certificate_t **cert, long cert_num);
LIBBMDCMS_SCOPE void Destroy_Certificate(Certificate_t **cert);
LIBBMDCMS_SCOPE long certPtr_from_ctx(bmd_crypt_ctx_t *ctx, GenBuf_t **cert, long cert_num);

LIBBMDCMS_SCOPE long SignedAttributes_destroy(SignedAttributes_t **sig_attrs);
LIBBMDCMS_SCOPE long UnsignedAttributes_destroy(UnsignedAttributes_t **unsig_attrs);
LIBBMDCMS_SCOPE long Attribute_destroy(Attribute_t **attr);

LIBBMDCMS_SCOPE long get_SignedAttribute_MessageDigest(GenBuf_t *signature, GenBuf_t** hash);
/*end WSZ*/



#if defined(__cplusplus)
}
#endif
#endif /* ifndef _LIBBMDCMS_H_*/
