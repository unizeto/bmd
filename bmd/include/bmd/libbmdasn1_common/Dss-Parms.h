/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIXAttributeCertificate"
 * 	found in "ac1.asn1"
 */

#ifndef	_Dss_Parms_H_
#define	_Dss_Parms_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_core/INTEGER.h>
#include <bmd/libbmdasn1_core/constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dss-Parms */
typedef struct Dss_Parms {
	INTEGER_t	 p;
	INTEGER_t	 q;
	INTEGER_t	 g;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} Dss_Parms_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_Dss_Parms;

#ifdef __cplusplus
}
#endif

#endif	/* _Dss_Parms_H_ */