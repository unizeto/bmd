/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "BMDDatagram"
 * 	found in "bmd_datagram.asn1"
 * 	`asn1c -fcompound-names -S`
 */

#ifndef	_BMDDatagram_H_
#define	_BMDDatagram_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_core/INTEGER.h>
#include <bmd/libbmdasn1_core/OCTET_STRING.h>
#include <bmd/libbmdasn1_core/constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct MetaDatas;

/* BMDDatagram */
typedef struct BMDDatagram {
	INTEGER_t	 protocolVersion;
	INTEGER_t	 datagramType;
	struct MetaDatas	*actionMetaDatas	/* OPTIONAL */;
	struct MetaDatas	*userMetaDatas	/* OPTIONAL */;
	struct MetaDatas	*sysMetaDatas	/* OPTIONAL */;
	struct MetaDatas	*pkiMetaDatas	/* OPTIONAL */;
	struct MetaDatas	*additionalMetaDatas	/* OPTIONAL */;
	OCTET_STRING_t	*protocolData	/* OPTIONAL */;
	OCTET_STRING_t	*protocolDataFilename	/* OPTIONAL */;
	OCTET_STRING_t	*protocolDataFileLocationId	/* OPTIONAL */;
	OCTET_STRING_t	*protocolDataFileId	/* OPTIONAL */;
	OCTET_STRING_t	*protocolDataOwner	/* OPTIONAL */;
	INTEGER_t	 randId;
	INTEGER_t	 filesRemaining;
	INTEGER_t	 datagramStatus;
	INTEGER_t	 datagramNumber;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} BMDDatagram_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_BMDDatagram;

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include "MetaDatas.h"

#endif	/* _BMDDatagram_H_ */