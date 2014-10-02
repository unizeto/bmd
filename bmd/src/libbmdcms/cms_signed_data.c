#include <bmd/libbmdcms/libbmdcms.h>
#include <bmd/libbmdpki/libbmdpki.h>
#include <bmd/libbmdpkcs11/libbmdpkcs11.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmderr/libbmderr.h>

#ifdef WIN32
#pragma warning(disable:4100)
#endif

// Wygenrowanie sygnatury
long SignerInfo_generate_Signature(	SignerInfo_t *signerInfo,
						bmd_crypt_ctx_t *ctx,
						long flags,
						SignedAttributes_t *sigattrs,
						UnsignedAttributes_t *unsigattrs,
						GenBuf_t *buf)
{
	long oid_rsaencryption[] = {OID_RSAENCRYPTION_LONG};
	/*long oid_md5withrsaencryption[] = {OID_MD5WITHRSAENCRYPTION};*/
	/** !!! WSZ !!! zle ustawianany alogorytm podpisu - chodzi o sam algorytm szyfrowania skrotu danych
	 long oid_sha1withrsaencryption[] = {OID_SHA1WITHRSAENCRYPTION};
	*/
	/*long oid_id_dsa_with_sha1[] = {OID_ID_DSA_WITH_SHA1};
	long oid_id_dsa[] = {OID_ID_DSA};
	long oid_md_5[] = {OID_MD_5};*/
	long oid_sha1[] = {OID_SHA_1_LONG};


	long err = 0;
	Certificate_t *cert = NULL;
	long *oid_sigalg = NULL;
	long oid_sigalgsize = 0;
	long status=0;

	if(ctx->pkcs11!=NULL)
	{
		if(ctx->pkcs11->cert)
			ber_decode(0, &asn_DEF_Certificate, (void **)&cert,
					   ctx->pkcs11->cert->buf, ctx->pkcs11->cert->size);
	}
	else
	{
		if( ctx->file != NULL )
		{
			if(ctx->file->cert)
				ber_decode(0, &asn_DEF_Certificate, (void **)&cert,
						   ctx->file->cert->buf,ctx->file->cert->size );
		}
		else
			if( ctx->ks != NULL )
			{
				ber_decode(0, &asn_DEF_Certificate, (void **)&cert,
						   ctx->ks->sig_cert->buf,ctx->ks->sig_cert->size );
			}
	}

	// SET sid
	{
		Name_t *issuerptr = &(signerInfo->sid.choice.issuerAndSerialNumber.issuer);
		CertificateSerialNumber_t *serialptr = &(signerInfo->sid.choice.issuerAndSerialNumber.serialNumber);

		err = asn_cloneContent( &asn_DEF_Name, &(cert->tbsCertificate.issuer), (void **)&(issuerptr));
		if ( err != 0 )
			return err;
		err = asn_cloneContent( &asn_DEF_INTEGER, &(cert->tbsCertificate.serialNumber), (void **)&(serialptr));
		if ( err != 0 )
			return err;

		signerInfo->sid.present = SignerIdentifier_PR_issuerAndSerialNumber;
	}

	// SET version
	{
		asn_long2INTEGER( &(signerInfo->version), CMSVersion_v1);
	}

	// SET digestAlgorithm
	// AlgorithIdentifier_t .algorithm OBJECT_IDENTIFIER
	{
		AlgorithmIdentifier_t *certkeyalg = &(cert->tbsCertificate.subjectPublicKeyInfo.algorithm);
		if ( !AlgorithmIdentifier_cmp_OID(certkeyalg, oid_rsaencryption, sizeof(oid_rsaencryption)/sizeof(*oid_rsaencryption)) )
		{
			AlgorithmIdentifier_set_OID( &(signerInfo->digestAlgorithm),
                                                     oid_sha1, sizeof(oid_sha1)/sizeof(*oid_sha1) );
			oid_sigalg = oid_rsaencryption;//!WSZ oid_sha1withrsaencryption;
			oid_sigalgsize = sizeof(oid_rsaencryption);//!WSZ sizeof(oid_sha1withrsaencryption);
		}
		else
			return BMD_ERR_UNIMPLEMENTED;
	}

	// SET SignatureAlgorithm
	{
		AlgorithmIdentifier_t *sigalgptr = &(signerInfo->signatureAlgorithm);
		AlgorithmIdentifier_set_OID(sigalgptr,
			oid_sigalg, oid_sigalgsize/sizeof(long));
		if ( err != 0 )
			return err;

		/*!!!! NULL parameter !WSZ*/
		/*
		ANY_t *alg_parameter=(ANY_t*)calloc(1, sizeof(ANY_t));
		alg_parameter->size=2;
		alg_parameter->buf=(uint8_t*)calloc(2, sizeof(uint8_t));
		alg_parameter->buf[0]=0x05;
		alg_parameter->buf[1]=0x00;
		sigalgptr->parameters=alg_parameter;
		*/
		/*!!!!*/


	}
	// SET signedAttrs
	{
        	Attribute_t *attr = NULL;
		Attribute_t *attrmd = NULL;
		if ( signerInfo->signedAttrs == NULL )
			signerInfo->signedAttrs = mallocStructure(sizeof(SignedAttributes_t));
		if ( signerInfo->signedAttrs == NULL )
			return ERR_NO_MEMORY;
		// content type
		BMD_FOK(Attribute_generate_ContentType_Data(&attr));
		BMD_FOK(SignedAttributes_add_Attribute( signerInfo->signedAttrs, attr ));

                // message digest
		BMD_FOK(Attribute_generate_MessageDigest(&attrmd, buf, flags));
		BMD_FOK(SignedAttributes_add_Attribute( signerInfo->signedAttrs, attrmd ));

                if(sigattrs)
                    BMD_FOK(SignerInfo_add_SignedAttributes(signerInfo, sigattrs));
		if(unsigattrs)
                    BMD_FOK(SignerInfo_add_UnsignedAttributes(signerInfo, unsigattrs));
	}

	// SET signature
	{
		GenBuf_t *attrbuf = NULL;
		GenBuf_t *computed_hash=NULL;
		GenBuf_t *computed_sig=NULL;
		GenBuf_t *computed_digestinfo=NULL;
		bmd_crypt_ctx_t *hash_ctx=NULL;

		err=asn1_encode(&asn_DEF_SignedAttributes,signerInfo->signedAttrs, NULL, &attrbuf);
		if( err != BMD_OK )
			return err;

		BMD_FOK(bmd_set_ctx_hash(&hash_ctx,BMD_HASH_ALGO_SHA1));
        BMD_FOK(bmd_hash_data(attrbuf,&hash_ctx,&computed_hash,NULL));
		BMD_FOK(bmd_ctx_destroy(&hash_ctx));

		prepare_digest_info(computed_hash,&computed_digestinfo);
		if(ctx->pkcs11!=NULL) /* podpis PKCS#11 */
		{
			status=bmd_pkcs11_sign(ctx->pkcs11,computed_digestinfo,&computed_sig);
			if(status!=BMD_OK)
				return BMD_ERR_OP_FAILED;
		}
		else
			if(ctx->ks!=NULL) /* podpis przez serwis kluczy */
			{
				status=ctx->ks->sign((void *)ctx->ks,computed_digestinfo,&computed_sig);
				if( status != BMD_OK )
					return BMD_ERR_OP_FAILED;
			}
			else
				if( ctx->file!=NULL)
				{
					status=file_sign_raw(ctx->file,computed_hash,&computed_sig);
					if(status!=BMD_OK)
						return BMD_ERR_OP_FAILED;
				}
				else
				{
					return BMD_ERR_FORMAT;
				}

		OCTET_STRING_fromBuf( &(signerInfo->signature),(char *)computed_sig->buf,computed_sig->size);
		// zwolnij zasoby
		free_gen_buf(&attrbuf);
		free_gen_buf(&computed_sig);
		free_gen_buf(&computed_hash);
		free_gen_buf(&computed_digestinfo);
	}
	asn_DEF_Certificate.free_struct(&asn_DEF_Certificate,cert,0);cert=NULL;
	return 0;
}

