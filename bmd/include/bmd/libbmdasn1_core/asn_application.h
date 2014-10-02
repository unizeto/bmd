/*-
 * Copyright (c) 2004 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
/*
 * Application-level ASN.1 API.
 */
#ifndef	_ASN_APPLICATION_H_
#define	_ASN_APPLICATION_H_

#include <bmd/libbmdasn1_core/asn_system.h>		/* for platform-dependent types */
#include <bmd/libbmdasn1_core/asn_codecs.h>		/* for ASN.1 codecs specifics */

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Generic type of an application-defined callback to return various
 * types of data to the application.
 * EXPECTED RETURN VALUES:
 *  -1: Failed to consume bytes. Abort the mission.
 * Non-negative return values indicate success, and ignored.
 */
typedef int (asn_app_consume_bytes_f)(const void *buffer, size_t size,
	void *application_specific_key);

#include <bmd/libbmdasn1_core/constr_TYPE.h>	/* for asn_TYPE_descriptor_t */

#ifdef __cplusplus
}
#endif

#endif	/* _ASN_APPLICATION_H_ */
