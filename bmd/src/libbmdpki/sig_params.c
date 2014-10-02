#include <bmd/common/bmd-common.h>

#include <bmd/libbmdpki/libbmdpki.h>
#include <bmd/libbmdpkcs12/libbmdpkcs12.h>
//#include <bmd/libbmddialogs/libbmddialogs.h>
#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmderr/libbmderr.h>

#include <bmd/libbmdts/libbmdts.h>
#include <bmd/libbmdpkcs11/libbmdpkcs11.h>
#include <bmd/common/bmd-crypto_ds.h>

#include <bmd/common/bmd-const.h>

#include <bmd/libbmdcms/libbmdcms.h>

#include <bmd/libbmdasn1_common/SigPolicy/SignerAndVerifierRules.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>

#define BMD_CMS_ATTR_CONTENT_TYPE		1
#define BMD_CMS_ATTR_MESSAGE_DIGEST		2
#define BMD_CMS_ATTR_SIGNING_TIME		3
#define BMD_CMS_ATTR_SIGNING_CERT		4
#define BMD_CMS_ATTR_SIG_POLICY_ID		5


long bmd_create_signature_params(bmd_signature_params_t **sig_params)
{
	if(sig_params==NULL)
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if((*sig_params)!=NULL)
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}

	(*sig_params)=(bmd_signature_params_t *)calloc(1, sizeof(bmd_signature_params_t));
	if(*sig_params == NULL)
		{ return BMD_ERR_MEMORY; }
	else
		{ return BMD_OK; }
}

long bmd_set_signature_params_flag(bmd_signature_params_t *sig_params,long flag)
{
	if(sig_params==NULL)
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}

	sig_params->signature_flags=flag;

	return BMD_OK;
}

long bmd_set_signature_params_cert_store(bmd_signature_params_t *sig_params,char *tcc)
{
	if(sig_params==NULL)
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(tcc==NULL)
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	asprintf(&(sig_params->tcc),"%s",tcc);
	return BMD_OK;
}

long _bmd_load_signature_policy(char *sig_policy,GenBuf_t **output)
{
	long status;
	GenBuf_t *tmp=NULL;

#ifdef WIN32
	wchar_t *wideFilename=NULL;
#endif

	if(sig_policy==NULL)
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(output==NULL)
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
	if((*output)!=NULL)
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	if(strstr(sig_policy,"http://")!=NULL)
	{
		tmp=get_from_url(sig_policy);
		if(tmp)
		{
			set_gen_buf2(tmp->buf,tmp->size,output);
		}
		else
		{
			PRINT_DEBUG("LIBBMDPKIERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
			return BMD_ERR_OP_FAILED;
		}
	}
	else
	{
#ifdef WIN32
		UTF8StringToWindowsString(sig_policy, &wideFilename, NULL);
		status=bmd_load_binary_content_wide(wideFilename, output);
		free(wideFilename); wideFilename=NULL;
#else
		status=bmd_load_binary_content(sig_policy,output);
#endif
		if(status!=BMD_OK)
		{
			PRINT_DEBUG("LIBBMDPKIERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
			return BMD_ERR_OP_FAILED;
		}
	}

	return BMD_OK;
}

long __bmd_determine_signed_attr_type(	OBJECT_IDENTIFIER_t *oid,
							long *type)
{
char contentType_OID[]	={"1.2.840.113549.1.9.3"};
char messageDigest_OID[]={"1.2.840.113549.1.9.4"};
char signingTime_OID[]	={"1.2.840.113549.1.9.5"};
char signingCert_OID[]	={"1.2.840.113549.1.9.16.2.12"};
char sigPolicyID_OID[]	={"1.2.840.113549.1.9.16.2.15"};

	if(OBJECT_IDENTIFIER_cmp(oid,contentType_OID)==0)
	{
		*type=BMD_CMS_ATTR_CONTENT_TYPE;
		return BMD_OK;
	}

	if(OBJECT_IDENTIFIER_cmp(oid,messageDigest_OID)==0)
	{
		*type=BMD_CMS_ATTR_MESSAGE_DIGEST;
		return BMD_OK;
	}

	if(OBJECT_IDENTIFIER_cmp(oid,signingTime_OID)==0)
	{
		*type=BMD_CMS_ATTR_SIGNING_TIME;
		return BMD_OK;
	}

	if(OBJECT_IDENTIFIER_cmp(oid,signingCert_OID)==0)
	{
		*type=BMD_CMS_ATTR_SIGNING_CERT;
		return BMD_OK;
	}

	if(OBJECT_IDENTIFIER_cmp(oid,sigPolicyID_OID)==0)
	{
		*type=BMD_CMS_ATTR_SIG_POLICY_ID;
		return BMD_OK;
	}

	return BMD_ERR_OP_FAILED;
}

long _bmd_policy_CMSAttrs_to_SignedAttrs(CMSAttrs_t *policy_attrs,SignedAttributes_t **attrs)
{
long i=0;
long type=0;

	if(policy_attrs==NULL)
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(attrs==NULL)
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	if((*attrs)==NULL)
	{
		(*attrs)=(SignedAttributes_t *)malloc(sizeof(SignedAttributes_t));
		memset(*attrs,0,sizeof(SignedAttributes_t));
	}

	for(i=0;i<policy_attrs->list.count;i++)
		__bmd_determine_signed_attr_type(policy_attrs->list.array[i],&type);

	return BMD_OK;
}

long bmd_destroy_signature_params(bmd_signature_params_t **sig_params)
{

	if(sig_params==NULL)
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if((*sig_params)==NULL)
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_OK;
	}

	free((*sig_params)->tcc); (*sig_params)->tcc=NULL;

	if((*sig_params)->signature_policy != NULL)
	{
		asn_DEF_SignaturePolicy.free_struct(&asn_DEF_SignaturePolicy,(*sig_params)->signature_policy, 1);
		free((*sig_params)->signature_policy);
		(*sig_params)->signature_policy=NULL;
	}
	if((*sig_params)->signed_attributes != NULL)
	{
		asn_DEF_SignedAttributes.free_struct(&asn_DEF_SignedAttributes,(*sig_params)->signed_attributes, 1);
		free((*sig_params)->signed_attributes);
		(*sig_params)->signed_attributes=NULL;
	}
	if((*sig_params)->unsigned_attributes != NULL)
	{
		asn_DEF_UnsignedAttributes.free_struct(&asn_DEF_UnsignedAttributes,(*sig_params)->unsigned_attributes, 1);
		free((*sig_params)->unsigned_attributes);
		(*sig_params)->unsigned_attributes=NULL;
	}

	free(*sig_params); *sig_params=NULL;

	return BMD_OK;
}

/*
WSZ porzdki - BMD_ERR_LOAD_SIG_POLICY (-77)
@return BMD_ERR_LOAD_SIG_POLICY (-77) jesli nie mona zaladowac badz zdekodowac wskazanej polityki podpisu
@return BMDPKI_ERR_FIND_CERT_STORE (-78) jesli nie mozna odnalezc magazynu certyfikatow zaufanych
z funkcji bmd_verify_cert_with_crl():
@return BMD_ERR_PARAM1				- niewlasciwe wywolanie
@return BMD_ERR_OP_FAILED			- wiele bledow nie okreslonych blizej
@return BMD_ERR_PKI_NO_CRL (-56)	- nie moze wczytac listy CRL
@return BMD_ERR_PKI_VERIFY			- blad weryfikacji certyfikatu
*/
long bmd_set_signature_params_policy(bmd_signature_params_t *sig_params,char *sig_policy,long auto_params_create,GenBuf_t *cert, CertExtConfig_t *certCheckParams)
{
long status;
GenBuf_t *buf=NULL;
SignaturePolicy_t *policy=NULL;
asn_dec_rval_t rval;
CMSAttrs_t *policy_signed_attrs=NULL;
SignerAndVerifierRules_t *SigVerRules=NULL;
long i=0;
long j=0;
long type;
char *policy_oid=NULL;
bmd_crypt_ctx_t *hash_ctx=NULL;
GenBuf_t *policy_hash_buf=NULL;
GenBuf_t *cert_hash_buf=NULL;
Attribute_t *sig_pol_attr=NULL;
Attribute_t *cert_attr=NULL;
Attribute_t *time_attr=NULL;
Certificate_t *asn1_cert=NULL;
CertificateTrustPoint_t *trust_point=NULL;
CertificateTrustTrees_t *trust_trees=NULL;
SigningCertTrustCondition_t *sig_cert_trust=NULL;
GeneralNames_t *g_names=NULL;
GeneralName_t *g_name=NULL;
char *policy_url=NULL;
long trust_condition=0;

GenBuf_t **trusted_certs=NULL;
GenBuf_t **constructed_cert_path=NULL;
long cert_path_length=0;

long flag_trust_found=0;

	if(sig_params==NULL)
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(sig_policy==NULL)
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	status=_bmd_load_signature_policy(sig_policy,&buf);
	if(status!=BMD_OK)
	{
		PRINT_DEBUG("LIBBMDPKIERR Error loading signature policy. Error=%i\n",BMDPKI_ERR_LOAD_SIG_POLICY);
		return BMDPKI_ERR_LOAD_SIG_POLICY;
	}
	rval=ber_decode(NULL,&asn_DEF_SignaturePolicy,&(sig_params->signature_policy),buf->buf,buf->size);
	if(rval.code!=RC_OK)
	{
		PRINT_DEBUG("LIBBMDPKIERR Error loading signature policy. Error=%i\n",BMDPKI_ERR_LOAD_SIG_POLICY);
		return BMDPKI_ERR_LOAD_SIG_POLICY;
	}

	policy=(SignaturePolicy_t *)sig_params->signature_policy;
	/* pobranie zaufanych certyfikatow dla podpisujacego z polityki podpisu */
	trust_trees=&(policy->signPolicyInfo.signatureValidationPolicy.commonRules.signingCertTrustCondition->signerTrustTrees);
	trusted_certs=(GenBuf_t **)calloc((trust_trees->list.count+2), sizeof(GenBuf_t *));
	for(i=0;i<trust_trees->list.count;i++)
	{
		trust_point=trust_trees->list.array[i];
		asn1_encode(&asn_DEF_Certificate,&(trust_point->trustpoint), NULL, &(trusted_certs[i]));
	}

	if(sig_params->tcc) /* jesli ustawiono Trusted Cert Store */
	{
		status=bmd_verify_cert_path(cert,sig_params->tcc,&constructed_cert_path,NULL);
		if(status==BMD_OK)
		{
			i=0;
			while(constructed_cert_path[i])
			{
				i++;
			}
			cert_path_length=i;

			i=0;
			while(trusted_certs[i])
			{
				if(trusted_certs[i]->size==constructed_cert_path[cert_path_length-1]->size)
				{
					if(memcmp(trusted_certs[i]->buf,constructed_cert_path[cert_path_length-1]->buf,trusted_certs[i]->size)==0)
					{
						flag_trust_found=1;
						break;
					}
				}
				i++;
			}
			if(flag_trust_found==0)
			{
				PRINT_DEBUG("LIBBMDPKIERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
				return BMD_ERR_OP_FAILED;
			}
		}
		else
		{
			PRINT_DEBUG("LIBBMDPKIERR Error in finding cert store. Error=%i\n",BMDPKI_ERR_FIND_CERT_STORE);
			return BMDPKI_ERR_FIND_CERT_STORE;
		}
	}
	else
	{
		PRINT_DEBUG("LIBBMDPKIERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
	}

	/* sprawdzenie czy certyfikat ktorym ma sie wykonac podpis jest na CRL - jesli tylko to jest w polityce */
	sig_cert_trust=policy->signPolicyInfo.signatureValidationPolicy.commonRules.signingCertTrustCondition;
	asn_INTEGER2long(&(sig_cert_trust->signerRevReq.endCertRevReq.enuRevReq),&trust_condition);
	if(trust_condition==EnuRevReq_clrCheck)
	{
		if(certCheckParams==NULL) /*na podstawie CRL distribution point w certyfikacie*/
		{
			status=bmd_verify_cert_with_crl(cert, NULL, NULL);
		}
		else if(certCheckParams->check_with_crl > 0)
		{
			if(certCheckParams->download_crl > 0)
			{
				status=bmd_verify_cert_with_crl(cert, certCheckParams->url_of_crl, NULL);
			}
			else if(certCheckParams->use_crl_file > 0)
			{
				status=bmd_verify_cert_with_crl(cert, NULL, certCheckParams->crl_file_path);
			}
			else /*na podstawie CRL distribution point w certyfikacie*/
			{
				status=bmd_verify_cert_with_crl(cert, NULL, NULL);
			}
		}
		else /*jesli wylaczona weryfikacja crl*/
		{
			status=BMD_OK;
		}

		if(status!=BMD_OK)
		{
			PRINT_DEBUG("LIBBMDPKIERR Error. Error=%li\n",status);
			return status;
		}
	}

	/* w tym momencie wiemy ze certyfikat moze wykonac podpis ??? */

	if(auto_params_create==1) /* stworzenie parametrow stalych do podpisu  */
	{
		policy=(SignaturePolicy_t *)sig_params->signature_policy;
		/* ustaw wskaznik na liste obowiazkowych atrybutow podpisanych */
		SigVerRules=policy->signPolicyInfo.signatureValidationPolicy.commonRules.signerAndVeriferRules;
		policy_signed_attrs=&(SigVerRules->signerRules.mandatedSignedAttr);
		sig_params->signed_attributes=(SignedAttributes_t *)malloc(sizeof(SignedAttributes_t));
		memset(sig_params->signed_attributes,0,sizeof(SignedAttributes_t));

		for(i=0;i<policy_signed_attrs->list.count;i++)
		{
			__bmd_determine_signed_attr_type(policy_signed_attrs->list.array[i],&type);
			switch(type)
			{
				case BMD_CMS_ATTR_CONTENT_TYPE: /* rozszerzenie standardowe - nic nie trzeba robic */
					break;
				case BMD_CMS_ATTR_MESSAGE_DIGEST: /* rozszerzenie standardowe - nic nie trzeba robic */
					break;
				case BMD_CMS_ATTR_SIGNING_TIME: /* zostanie stworzony pozniej */
					SigAttr_Create_SigningTime(&time_attr);
					asn_set_add(sig_params->signed_attributes,time_attr);
					break;
				case BMD_CMS_ATTR_SIGNING_CERT:
					if(cert)
					{
						bmd_ctx_destroy(&hash_ctx);
						bmd_set_ctx_hash(&hash_ctx,BMD_HASH_ALGO_SHA1);
						status=bmd_hash_data(cert,&hash_ctx,&cert_hash_buf,NULL);
						ber_decode(NULL,&asn_DEF_Certificate,(void **)&asn1_cert,cert->buf,cert->size);
						SigAttr_Create_SigningCertificate(cert_hash_buf,asn1_cert,&cert_attr);

						asn_set_add(sig_params->signed_attributes,cert_attr);
						bmd_ctx_destroy(&hash_ctx);
						asn_DEF_Certificate.free_struct(&asn_DEF_Certificate,asn1_cert,0);asn1_cert=NULL;
						free_gen_buf(&cert_hash_buf);
					}
					else
					{
						PRINT_DEBUG("LIBBMDPKIERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
						return BMD_ERR_OP_FAILED;
					}
					break;
				case BMD_CMS_ATTR_SIG_POLICY_ID:
					status=OBJECT_IDENTIFIER2string(&(policy->signPolicyInfo.signPolicyIdentifier),&policy_oid);
					if(status!=BMD_OK)
					{
						PRINT_DEBUG("LIBBMDPKIERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
						return BMD_ERR_OP_FAILED;
					}

					/* (by WSZ)  zmiana sposobu wyliczania skrotu polityki podipsu (ProCertum API compatible)
					bmd_ctx_destroy(&hash_ctx);
					status=bmd_set_ctx_hash(&hash_ctx,BMD_HASH_ALGO_SHA1);
					status=bmd_hash_data(buf,&hash_ctx,&policy_hash_buf,NULL);
					*/
					status=bmd_hash_SignaturePolicy(buf, &policy_hash_buf);
					g_names=&(policy->signPolicyInfo.policyIssuerName);
					for(j=0;j<g_names->list.count;j++)
					{
						g_name=g_names->list.array[j];
						if(g_name->present==GeneralName_PR_dNSName)
						{
							GeneralName2string(g_name,&policy_url);
							break;
						}
					}

					SigAttr_Create_SignaturePolicyId(policy_oid,policy_url,policy_hash_buf,&sig_pol_attr);
					asn_set_add(sig_params->signed_attributes,(void *)sig_pol_attr);
					free(policy_oid);
					free(policy_url);
					bmd_ctx_destroy(&hash_ctx);
					free_gen_buf(&policy_hash_buf);

					break;
			}
		}
	}

	i=0;
	if(trusted_certs)
	{
		while(trusted_certs[i])
		{
			free_gen_buf(&(trusted_certs[i]));
			i++;
		}
		free(trusted_certs);trusted_certs=NULL;
	}

	i=0;
	if(constructed_cert_path)
	{
		while(constructed_cert_path[i])
		{
			free_gen_buf(&(constructed_cert_path[i]));
			i++;
		}
		free(constructed_cert_path);constructed_cert_path=NULL;
	}

	return BMD_OK;
}

