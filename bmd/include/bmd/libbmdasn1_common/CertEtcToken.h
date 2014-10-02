/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIXDVCS"
 * 	found in "dvcs.asn1"
 */

#ifndef	_CertEtcToken_H_
#define	_CertEtcToken_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_common/Certificate.h>
#include <bmd/libbmdasn1_common/SigPolicy/ESSCertID.h>
#include <bmd/libbmdasn1_common/PKIStatusInfo.h>
#include <bmd/libbmdasn1_common/ContentInfo.h>
#include <bmd/libbmdasn1_common/CertificateList.h>
#include <bmd/libbmdasn1_common/CertStatus.h>
#include <bmd/libbmdasn1_common/CertID.h>
#include <bmd/libbmdasn1_common/OCSPResponse.h>
#include <bmd/libbmdasn1_common/SMIMECapabilities.h>
#include <bmd/libbmdasn1_common/Extension.h>
#include <bmd/libbmdasn1_core/constr_CHOICE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum CertEtcToken_PR {
	CertEtcToken_PR_NOTHING,	/* No components present */
	CertEtcToken_PR_certificate,
	CertEtcToken_PR_esscertid,
	CertEtcToken_PR_pkistatus,
	CertEtcToken_PR_assertion,
	CertEtcToken_PR_crl,
	CertEtcToken_PR_ocspcertstatus,
	CertEtcToken_PR_oscpcertid,
	CertEtcToken_PR_oscpresponse,
	CertEtcToken_PR_capabilities,
	CertEtcToken_PR_extension,
} CertEtcToken_PR;

/* CertEtcToken */
typedef struct CertEtcToken {
	CertEtcToken_PR present;
	union CertEtcToken_u {
		Certificate_t	 certificate;
		ESSCertID_t	 esscertid;
		PKIStatusInfo_t	 pkistatus;
		ContentInfo_t	 assertion;
		CertificateList_t	 crl;
		CertStatus_t	 ocspcertstatus;
		CertID_t	 oscpcertid;
		OCSPResponse_t	 oscpresponse;
		SMIMECapabilities_t	 capabilities;
		Extension_t	 extension;
	} choice;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} CertEtcToken_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_CertEtcToken;

#ifdef __cplusplus
}
#endif

#endif	/* _CertEtcToken_H_ */
