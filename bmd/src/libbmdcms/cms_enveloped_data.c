#include <bmd/libbmdcms/libbmdcms.h>
#include <bmd/libbmdpki/libbmdpki.h>
#include <bmd/libbmdX509/libbmdX509.h>
#include <openssl/rand.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>

// Umieszczenie danych zaszyfrowanych i informacji o algorytmie
// szyfrujacym w strukturze
// WYKONYWANE SA KOPIE iv oraz data
long EnvelopedData_set_EncryptedData(EnvelopedData_t *envData,GenBuf_t *iv,GenBuf_t *data,bmd_encryption_params_t *options)
{
	long OID_id_data[] = {OID_CMS_ID_DATA_LONG};
	/*long OID_id_DES_CBC[] = {1,3,14,3,2,7};*/
	long OID_id_DES_EDE3_CBC[] = {1,2,840,113549,3,7};
	long *oid;
	long oidsize;

	long err = 0;
	OCTET_STRING_t *oiv = NULL;

	// set data OID
	err = OBJECT_IDENTIFIER_set_arcs(&(envData->encryptedContentInfo.contentType),OID_id_data, sizeof(OID_id_data[0]),
									 sizeof(OID_id_data)/sizeof(OID_id_data[0]));
	if ( err )
		return err;

	// set Algorithm OID
	if ( options->encryption_algo == BMD_CRYPT_ALGO_DES3 )
	{
		oid = OID_id_DES_EDE3_CBC;
		oidsize = sizeof(OID_id_DES_EDE3_CBC)/sizeof(OID_id_DES_EDE3_CBC[0]);
	}
	else
		return BMD_ERR_UNIMPLEMENTED;

	err = AlgorithmIdentifier_set_OID(&(envData->encryptedContentInfo.contentEncryptionAlgorithm),oid, oidsize);
	if ( err )
		return err;

	// set Algorithm IV
	oiv = mallocStructure(sizeof(OCTET_STRING_t));
	OCTET_STRING_fromBuf(oiv,(char *)iv->buf, iv->size);
	envData->encryptedContentInfo.contentEncryptionAlgorithm.parameters = mallocStructure(sizeof(ANY_t));
	err = ANY_fromType(envData->encryptedContentInfo.contentEncryptionAlgorithm.parameters,	&asn_DEF_OCTET_STRING, oiv);
	if ( err )
		return ERR_OPERATION_FAILED;
	asn_DEF_OCTET_STRING.free_struct(&asn_DEF_OCTET_STRING,oiv, 0);

	if( options->encryption_type==BMD_CMS_ENV_INTERNAL )
	{
		// set Encrypted Data
		envData->encryptedContentInfo.encryptedContent = mallocStructure(sizeof(EncryptedContent_t));
		OCTET_STRING_fromBuf(envData->encryptedContentInfo.encryptedContent,(char *)data->buf, data->size);
	}
	return err;
}

/*
 * Dodanie RecipientInfo do EnvelopedData
 * Nastepuje dodanie do listy wskazania ! Nie usuwac dodwanych danych !
 */
long EnvelopedData_add_RecipientInfo(EnvelopedData_t *envData, RecipientInfo_t *recInfo)
{
	if ( (envData == NULL) || (recInfo == NULL) )
		return ERR_WRONG_ARGUMENT;
	if ( asn_set_add( &(envData->recipientInfos.list), recInfo) != 0 )
		return ERR_OPERATION_FAILED;
	return 0;
}

long EnvelopedData_is_Certificate(EnvelopedData_t *envData, Certificate_t *cert)
{
	long err = 0;

	if (( cert == NULL ) || (envData == NULL ))
		return ERR_WRONG_ARGUMENT;
	if ( envData->originatorInfo == NULL )
		return 0;

	err = CertificateSet_is_Certificate(envData->originatorInfo->certs, cert);
	return err;
}

// Dodaj certyfikat do listy certyfikat�.
// Najpierw sprawd, czy taki jest obecny na licie
// Certyfikat jest umieszczany jako kopia !
long EnvelopedData_add_Certificate(EnvelopedData_t *envData, Certificate_t *cert)
{
	long err = 0;
	CertificateChoices_t *cc = NULL;
	Certificate_t *cptr = NULL;

	if (( envData == NULL ) || ( cert == NULL ))
		return ERR_WRONG_ARGUMENT;
	err = EnvelopedData_is_Certificate(envData, cert);
	if ( err < 0 )
		return err;
	if ( err == 1 )
		return 0;
	// budowanie struktur zawierajacych
	if ( envData->originatorInfo == NULL )
		envData->originatorInfo = mallocStructure(sizeof(OriginatorInfo_t));
	if ( envData->originatorInfo == NULL )
		return ERR_NO_MEMORY;
	envData->originatorInfo->certs = mallocStructure(sizeof(CertificateSet_t));
	if ( envData->originatorInfo->certs == NULL )
		return ERR_NO_MEMORY;
	// budowanie struktury dla certyfikatu
	cc = mallocStructure(sizeof(CertificateChoices_t));
	if ( cc == NULL )
			return ERR_NO_MEMORY;
	cc->present = CertificateChoices_PR_certificate;
	cptr = &cc->choice.certificate;
	asn_cloneContent(&asn_DEF_Certificate, (void *)cert, (void **)&cptr);
	// dodanie certu do listy
	if ( asn_set_add(&(envData->originatorInfo->certs->list), cc) )
		return ERR_OPERATION_FAILED;
	return err;
}

// ustawienie porzadanej wersji struktury EnvelopedData.
// Funkcja ustawia (!), nie wykonuje weryfikacji, czy takie ustawienie
// jest sluszne.
long EnvelopedData_set_CMSVersion(EnvelopedData_t *envData, CMSVersion_e version)
{
//	long err = 0;
	if ( envData == NULL )
		return ERR_WRONG_ARGUMENT;
	asn_long2INTEGER( &(envData->version), version);
	return 0;
}

// Funkcja umieszcza w ContentInfo struktur�EnvelopedData. Jeeli
// w ContentInfo istniay ju jakie dane, zostan zwolnione,
// a na ich miejsce umieszczona podana strukturka
// Wykonywana jest kopia struktury EnvelopedData
long ContentInfo_set_EnvelopedData(ContentInfo_t* content, EnvelopedData_t* envData)
{
	long OID_id_envelopedData[] = {OID_CMS_ID_ENVELOPED_DATA_LONG};
	long err = 0;
	if ((content == NULL) || (envData == NULL))
		return ERR_WRONG_ARGUMENT;
	// wyzeruj strukture ContentInfo
	asn_DEF_ContentInfo.free_struct( &asn_DEF_ContentInfo,
		content, 1);
	memset(content, 0, sizeof(ContentInfo_t));

	// ustaw identyfikator
	OBJECT_IDENTIFIER_set_arcs(
		&(content->contentType),
		OID_id_envelopedData,
		sizeof(*OID_id_envelopedData),
		sizeof(OID_id_envelopedData)/sizeof(*OID_id_envelopedData));
	// umiesc dane
	err = ANY_fromType(&content->content, &asn_DEF_EnvelopedData, envData);
	if ( err )
		return err;
	return err;
}

// Wyznacza wersj�dla struktury EnvelopedData na podstawie
// zawartosci owej struktury. Realizowane wedlug dokumentacji
long EnvelopedData_count_Version(EnvelopedData_t *envData)
{
	if ( (envData->originatorInfo != NULL)
		&& (isAnyCertOther(envData->originatorInfo->certs)
		|| isAnyCRLOther(envData->originatorInfo->crls)) )
		EnvelopedData_set_CMSVersion(envData, CMSVersion_v4);
	else if ( ((envData->originatorInfo != NULL) && (isAnyCertWithVer(envData->originatorInfo->certs, 2)))
		|| RecipientInfos_has_RecipientInfo_present(&(envData->recipientInfos), RecipientInfo_PR_pwri)
		|| RecipientInfos_has_RecipientInfo_present(&(envData->recipientInfos), RecipientInfo_PR_ori) )
		EnvelopedData_set_CMSVersion(envData, CMSVersion_v3);
	else if ( (envData->originatorInfo == NULL)
		|| (envData->unprotectedAttrs == NULL)
		|| RecipientInfos_allVersion(&(envData->recipientInfos), CMSVersion_v0) )
		EnvelopedData_set_CMSVersion(envData, CMSVersion_v0);
	else
		EnvelopedData_set_CMSVersion(envData, CMSVersion_v2);
	return 0;
}
