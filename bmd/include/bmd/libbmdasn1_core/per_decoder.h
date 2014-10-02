/*-
 * Copyright (c) 2005 Lev Walkin <vlm@lionet.info>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#ifndef	_PER_DECODER_H_
#define	_PER_DECODER_H_

#include <bmd/libbmdasn1_core/asn_application.h>
#include <bmd/libbmdasn1_core/per_support.h>

#ifdef __cplusplus
extern "C" {
#endif

struct asn_TYPE_descriptor_s;	/* Forward declaration */

/*
 * Unaligned PER decoder of any ASN.1 type. May be invoked by the application.
 */
asn_dec_rval_t uper_decode(struct asn_codec_ctx_s *opt_codec_ctx,
	struct asn_TYPE_descriptor_s *type_descriptor,	/* Type to decode */
	void **struct_ptr,	/* Pointer to a target structure's pointer */
	const void *buffer,	/* Data to be decoded */
	size_t size		/* Size of data buffer */
	);


/*
 * Pre-computed PER constraints.
 */
  enum asn_per_constraint_flags {
	APC_UNCONSTRAINED	= 0x0,	/* No PER visible constraints */
	APC_SEMI_CONSTRAINED	= 0x1,	/* Constrained at "lb" */
	APC_CONSTRAINED		= 0x2,	/* Fully constrained */
	APC_EXTENSIBLE		= 0x4	/* May have extension */
  };
typedef struct asn_per_constraint_s {
	enum asn_per_constraint_flags flags;
	int  range_bits;		/* Full number of bits in the range */
	int  effective_bits;		/* Effective bits */
	long lower_bound;		/* "lb" value */
	long upper_bound;		/* "ub" value */
} asn_per_constraint_t;
typedef struct asn_per_constraints_s {
	asn_per_constraint_t value;
	asn_per_constraint_t size;
} asn_per_constraints_t;

/*
 * Type of the type-specific PER decoder function.
 */
typedef asn_dec_rval_t (per_type_decoder_f)(asn_codec_ctx_t *opt_codec_ctx,
		struct asn_TYPE_descriptor_s *type_descriptor,
		asn_per_constraints_t *constraints,
		void **struct_ptr,
		asn_per_data_t *per_data
	);

#ifdef __cplusplus
}
#endif

#endif	/* _PER_DECODER_H_ */
