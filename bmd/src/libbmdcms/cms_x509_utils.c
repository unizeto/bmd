#include <bmd/libbmdcms/libbmdcms.h>
#include <bmd/libbmdpki/libbmdpki.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmderr/libbmderr.h>

/* Funkcja importuje pelny lancuch certyfikatow z koperty CMS - na razie z typu SignedData */
long GetX509CertListFromCMSSignature(	GenBuf_t *FileBuf,
							GenBufList_t **X509CertificatesList)
{
ContentInfo_t *cms	= NULL;
SignedData_t *SignedData= NULL;

long *signature_oid = NULL;
long signature_oid_size=0;

long OID_id_data[] 		= {OID_CMS_ID_DATA_LONG};
long OID_id_signedData[] 	= {OID_CMS_ID_SIGNED_DATA_LONG};
long OID_id_envelopedData[] 	= {OID_CMS_ID_ENVELOPED_DATA_LONG};
long OID_id_messageDigest[]	= {OID_CMS_ID_MESSAGE_DIGEST_LONG};
long OID_id_digestedData[] 	= {OID_CMS_ID_DIGESTED_DATA_LONG};
long OID_id_encryptedData[] 	= {OID_CMS_ID_ENCRYPTED_DATA_LONG};
long OID_id_ct_authData[] 	= {OID_CMS_ID_CT_AUTH_DATA_LONG};
long OID_id_countersignature[]	= {OID_CMS_ID_COUNTERSIGNATURE_LONG};

	/* Zdekoduj ContentInfo */
	BMD_FOK(get_CMScontent_from_GenBuf(FileBuf, &cms));

	/* Przekonwertuj OID do tablicy ulong */
	signature_oid_size = OID_to_ulong_alloc2(&(cms->contentType), &signature_oid);
	if(signature_oid == NULL) BMD_FOK(ERR_NO_MEMORY);

	/* Sprawdzamy co jest w polu content na podstawie OIDa contentType */
	if(!OID_cmp2(OID_id_data, sizeof(OID_id_data), signature_oid, signature_oid_size*sizeof(long)))
	{
		PRINT_ERROR("Error field data not suported in CMS yet.");
	}
	else if(!OID_cmp2(OID_id_signedData, sizeof(OID_id_signedData),signature_oid, signature_oid_size*sizeof(long)))
	{
		//PRINT_ERROR("Signature present in CMS.\n");
		BMD_FOK(get_SignedData_from_CMScontent(&(cms->content), &SignedData));
		BMD_FOK(GetX509CertListFromSignedData(SignedData, X509CertificatesList));
		/* SignedData juz nie potrzebne */
		asn_DEF_SignedData.free_struct(&asn_DEF_SignedData, SignedData, 0);
	}
	else if(!OID_cmp2(OID_id_envelopedData, sizeof(OID_id_envelopedData), signature_oid, signature_oid_size*sizeof(long)))
	{
		PRINT_ERROR("Error field envelopedData not suported in CMS yet.");
	}
	else if(!OID_cmp2(OID_id_messageDigest, sizeof(OID_id_messageDigest),signature_oid, signature_oid_size*sizeof(long)))
	{
		PRINT_ERROR("Error field messageDigest not suported in CMS yet.");
	}
	else if(!OID_cmp2(OID_id_digestedData, sizeof(OID_id_digestedData),signature_oid, signature_oid_size*sizeof(long)))
	{
		PRINT_ERROR("Error field digestedData not suported in CMS yet.");
	}
	else if(!OID_cmp2(OID_id_encryptedData, sizeof(OID_id_encryptedData),signature_oid, signature_oid_size*sizeof(long)))
	{
		PRINT_ERROR("Error field encryptedData not suported in CMS yet.");
	}
	else if(!OID_cmp2(OID_id_ct_authData, sizeof(OID_id_ct_authData),signature_oid, signature_oid_size*sizeof(long)))
	{
		PRINT_ERROR("Error field authData not suported in CMS yet.");
	}
	else if(!OID_cmp2(OID_id_countersignature, sizeof(OID_id_countersignature),signature_oid, signature_oid_size*sizeof(long)))
	{
		PRINT_ERROR("Error field countersignature not suported in CMS yet.");
	}
	else
	{
		PRINT_ERROR("Error: unknown content type!");
	}

	asn_DEF_ContentInfo.free_struct(&asn_DEF_ContentInfo, cms, 0);
	free(signature_oid); signature_oid=NULL;

	return BMD_OK;
}

/* Funkcja skanuje strukture SignedData w pozukiwaniu certyfikatow.	*/
/* Jesli istnieja to pobiera pierwszy z nich, koduje go do DER i	*/
/* umieszcza w GenBuf_t							*/
long get_first_certificate_from_SignedData(SignedData_t *SignedData, GenBuf_t **GenBuf)
{
long err=0, a=0;

	/* Sprawdź czy aby zbiór certyfikatow nie jest pusty */
	if(SignedData->certificates == NULL)
		return ERR_NO_CERIFICATES_PRESENT_IN_SIGNED_DATA;

	/*printf("No of certificates present in Signature: %d\n", SignedData->certificates->list.count);*/

	/* Przejdź się po liscie certow. Niestety sa one juz rozkodowane, wiec trzeba je ponownie zakodowac.	*/
	/* To jaki koder wywolam zalezy od typu certyfikatu wskazanego przez zmienna present.			*/
	/*for(a=0; a<SignedData->certificates->list.count; a++) */

	/* Ta funkcja pobiera tylko pierwszy certyfikat ze zbioru 	*/
	{
		switch (SignedData->certificates->list.array[a]->present)
		{
		case CertificateChoices_PR_certificate:
			err = der_encode_X509_certificate(&(SignedData->certificates->list.array[a]->choice.certificate), GenBuf);
			if(err<0) return err;
		break;
		/* Istnieje jeszcze mozliwosc wyciagania innych obiektow z SignedData ale na razie w pliku asn1 wykomentowalem to */
		/*case CertificateChoices_PR_extendedCertificate:
		break;
		case CertificateChoices_PR_v1AttrCert:
		break;
		case CertificateChoices_PR_v2AttrCert:
		break;
		case CertificateChoices_PR_NOTHING:*/
		default:
		break;
		}
	}
	return err;
}

/* Funkcja skanuje strukture SignedData w pozukiwaniu certyfikatow.	*/
/* Jesli istnieja to pobiera kazdy z nich, koduje go do DER i		*/
/* umieszcza w liscie GenBufList_t					*/
long GetX509CertListFromSignedData(SignedData_t *SignedData, GenBufList_t **X509CertificatesList)
{
long err=0, a=0;
long NoOfCertificates = 0;

	if(SignedData == NULL)
		return BMD_ERR_PARAM1;
	if(X509CertificatesList == NULL || *X509CertificatesList != NULL)
		return BMD_ERR_PARAM2;

	/* Sprawdź czy aby zbiór certyfikatow nie jest pusty */
	if(SignedData->certificates == NULL)
		return ERR_NO_CERIFICATES_PRESENT_IN_SIGNED_DATA;

	*X509CertificatesList = (GenBufList_t *)calloc(1, sizeof(GenBufList_t));
	if(*X509CertificatesList == NULL)
		{ return NO_MEMORY; }

	NoOfCertificates = SignedData->certificates->list.count;
	(*X509CertificatesList)->gbufs = (GenBuf_t **)calloc(NoOfCertificates, sizeof(GenBuf_t *));
	if((*X509CertificatesList)->gbufs == NULL)
	{
		free(*X509CertificatesList); *X509CertificatesList=NULL;
		return NO_MEMORY;
	}

	//PRINT_ERROR("No of certificates present in Signature: %d\n", NoOfCertificates);
	/* Przejdź się po liscie certow. Niestety sa one juz rozkodowane, wiec trzeba je ponownie zakodowac.	*/
	/* To jaki koder wywolam zalezy od typu certyfikatu wskazanego przez zmienna present.			*/
	for(a=0; a<NoOfCertificates; a++) {
		switch (SignedData->certificates->list.array[a]->present) {
		case CertificateChoices_PR_certificate:
			err = der_encode_X509_certificate(
				&(SignedData->certificates->list.array[a]->choice.certificate),
				&((*X509CertificatesList)->gbufs[a]));
			if(err<0) return err;
			break;
			/* Istnieje jeszcze mozliwosc wyciagania innych obiektow z SignedData	*
			* ale na razie w pliku asn1 wykomentowalem to 				*/
			/* case CertificateChoices_PR_extendedCertificate:			*
			* break;								*
			* case CertificateChoices_PR_v1AttrCert:				*
			* break;								*
			* case CertificateChoices_PR_v2AttrCert:				*
			* break;								*
			* case CertificateChoices_PR_NOTHING:					*/
		default:
			break;
		}
	}
	(*X509CertificatesList)->size = NoOfCertificates;
	return err;
}


/* Funkcja dekoduje pole content w CMSie i wyluskuje strukture SignedData. */
long get_CMScontent_from_GenBuf(	GenBuf_t *gbuf,
						ContentInfo_t **cms)
{
asn_dec_rval_t rval;

	rval = ber_decode(0, &asn_DEF_ContentInfo,  (void **)cms, gbuf->buf, gbuf->size);
	if(rval.code != RC_OK)
	{
		asn_DEF_ContentInfo.free_struct(&asn_DEF_ContentInfo, *cms, 0);
		BMD_FOK(ERR_DECODE_CMS_DATA);
	}
	return BMD_OK;
}

/* Funkcja dekoduje pole content w CMSie i wyluskuje strukture SignedData. */
long get_SignedData_from_CMScontent(ANY_t *content, SignedData_t **SignedData)
{
long err=0;
asn_dec_rval_t rval;

	rval = ber_decode(0, &asn_DEF_SignedData,  (void **)SignedData, content->buf, content->size);
	if(rval.code != RC_OK){
		asn_DEF_SignedData.free_struct(&asn_DEF_SignedData, *SignedData, 0);
		return ERR_DECODE_SIGNED_DATA;
	}
	return err;
}

/* Funkcja koduje Certificate_t do postaci DER alokujac przy tym niezbedna	*/
/* przestrzen na bufor.								*/
long der_encode_X509_certificate(Certificate_t *x509cert, GenBuf_t **gbuf)
{
	long err=0;

	err=asn1_encode(&asn_DEF_Certificate,x509cert, NULL, gbuf);

	return err;
}
