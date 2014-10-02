#include <bmd/libbmdcms/libbmdcms.h>
#include <bmd/libbmdpki/libbmdpki.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>

/*
 * Dodanie SignerInfo do SignedData
 * Nastepuje dodanie wskazania do listy, wiec nie usuwac dancyh
 */
long SignedData_add_SignerInfo(SignedData_t *sigData, SignerInfo_t *sigInfo)
{
	if (( sigData == NULL ) || ( sigInfo == NULL ))
		return ERR_WRONG_ARGUMENT;
	if ( asn_set_add(&(sigData->signerInfos.list), sigInfo) != 0 )
		return ERR_OPERATION_FAILED;
	return 0;
}

/*
 * Dodanie do listy algorytmow nowego algorytmu.
 * Funkcja przy dodawaniu sprawdza, czy dany algorytm juz istnieje,
 * jezeli stwierdzi istnienie, nie zwraca bledu.
 */
long SignedData_add_DigestAlgorithm(SignedData_t *sigData, DigestAlgorithmIdentifier_t *digest)
{
	long err = 0;
	long i;
	long *arcs = NULL;
	long arccnt;
	if (( sigData == NULL ) || ( digest == NULL ))
		return ERR_WRONG_ARGUMENT;
	arccnt = OID_to_ulong_alloc2(&(digest->algorithm), &arcs);
	if ( arccnt < 0 )
		return ERR_OPERATION_FAILED;
	// sprawdz, czy nie jest ju na liscie
	for (i =0; i<sigData->digestAlgorithms.list.count; i++)
	{
		AlgorithmIdentifier_t *aid = sigData->digestAlgorithms.list.array[i];
		if ( !AlgorithmIdentifier_cmp_OID(aid, arcs, arccnt) )
		{
			free(arcs);
			return 0;
		}
	}
	if ( asn_set_add(&(sigData->digestAlgorithms.list), digest) != 0 )
	{
		free(arcs);
		return ERR_OPERATION_FAILED;
	}
	free(arcs);
	return err;
}

// sprawdza czy na liscie certyfikatow danych podpisanych znajduje sie
// podany certyfikat.
// Wyszukiwanie odbywa sie na podstawie skrotu obliczonego z wystawcy
// certyfikatu oraz jego numeru serial
// rezultaty:
// <0 bd
// =0 nie znalaz
// =1 znalaz
// >1 nie zdefiniowane
long SignedData_is_Certificate(SignedData_t *sigData, Certificate_t *cert)
{
	long err = 0;

	if (( cert == NULL ) || (sigData == NULL ))
		return ERR_WRONG_ARGUMENT;

	err = CertificateSet_is_Certificate(sigData->certificates, cert);
	return err;
}

// Dodaj certyfikat do listy certyfikat�.
// Najpierw sprawd, czy taki jest obecny na licie
// Certyfikat jest umieszczany jako kopia !
long SignedData_add_Certificate(SignedData_t *sigData, Certificate_t *cert)
{
	long err = 0;
	CertificateChoices_t *cc = NULL;
	Certificate_t *cptr = NULL;
	if (( sigData == NULL ) || ( cert == NULL ))
		return ERR_WRONG_ARGUMENT;
	err = SignedData_is_Certificate(sigData, cert);
	if ( err < 0 )
		return err;
	if ( err == 1 )
		return 0;
	if ( sigData->certificates == NULL )
	{
		sigData->certificates = mallocStructure(sizeof(CertificateSet_t));
		if ( sigData->certificates == NULL )
			return ERR_NO_MEMORY;
//		memset(sigData->certificates, 0, sizeof(CertificateSet_t));
	}
	cc = mallocStructure(sizeof(CertificateChoices_t));
	if ( cc == NULL )
			return ERR_NO_MEMORY;
//	memset(cc, 0, sizeof(CertificateChoices_t));
	cc->present = CertificateChoices_PR_certificate;
	cptr = &cc->choice.certificate;
	/*cptr = NULL;*/
	asn_cloneContent(&asn_DEF_Certificate, (void *)cert, (void **)&cptr);
//	memmove(&(cc->choice), cert, sizeof(Certificate_t));

	if ( asn_set_add(&(sigData->certificates->list), cc) )
		return ERR_OPERATION_FAILED;
	return 0;
}

// Dodanie listy CRL do struktury SignedData
// Struktura jest dodawana poprzez wykonanie kopiowania
long SignedData_add_CRL(SignedData_t *sigData, CertificateList_t *crl)
{
	RevocationInfoChoice_t *rc = NULL;
	CertificateList_t *clptr = NULL;
	if ((sigData == NULL) || (crl == NULL))
		return ERR_WRONG_ARGUMENT;
	if ( sigData->crls == NULL )
	{
		sigData->crls = mallocStructure(sizeof(RevocationInfoChoices_t));
		if ( sigData->crls == NULL )
			return ERR_NO_MEMORY;
//		memset(sigData->crls, 0, sizeof(RevocationInfoChoices_t));
	}
	rc = mallocStructure(sizeof(RevocationInfoChoice_t));
	if ( rc == NULL )
			return ERR_NO_MEMORY;
//	memset(rc, 0, sizeof(RevocationInfoChoice_t));
	rc->present = RevocationInfoChoice_PR_crl;
	clptr = &(rc->choice.crl);
	/*clptr=NULL;*/
	asn_cloneContent(&asn_DEF_CertificateList, (void *)crl, (void **)&clptr);
	if ( asn_set_add(&(sigData->crls->list), rc) )
		return ERR_OPERATION_FAILED;
	return 0;
}

// ustawienie porzadanej wersji struktury SignedData.
// Funkcja ustawia (!), nie wykonuje weryfikacji, czy takie ustawienie
// jest sluszne.
long SignedData_set_CMSVersion(SignedData_t *sigData, CMSVersion_e version)
{
//	long err = 0;
	if ( sigData == NULL )
		return ERR_WRONG_ARGUMENT;
	asn_long2INTEGER( &(sigData->version), version);
	return 0;
}

// ustawienie typu zawratosci danych struktury SignedData
long SignedData_set_ContentType(	SignedData_t *sigData,
						long *oid_tab,
						long oid_size)
{
	long err = 0;
	if ((sigData == NULL) || (oid_tab == NULL))
		return ERR_WRONG_ARGUMENT;
	err = OBJECT_IDENTIFIER_set_arcs(
		&(sigData->encapContentInfo.eContentType),
		oid_tab, sizeof(oid_tab[0]), oid_size);
	if ( err != 0 )
		return err;
	return err;
}

// ustawienie danych struktury SignedData
// wykonywana jest kopia na podstawie tworzenia OCTET_STRING
// z przekazanych danych
long SignedData_set_Content(
	SignedData_t *sigData,
	GenBuf_t *buf)
{
	long err = 0;
	if ((sigData == NULL) || (buf == NULL))
		return ERR_WRONG_ARGUMENT;
	sigData->encapContentInfo.eContent = mallocStructure(sizeof(OCTET_STRING_t));
	if ( sigData->encapContentInfo.eContent == NULL )
		return ERR_NO_MEMORY;
//	memset(sigData->encapContentInfo.eContent, 0, sizeof(OCTET_STRING_t));
	err = OCTET_STRING_fromBuf(sigData->encapContentInfo.eContent,
		(char *)buf->buf, buf->size);
	if ( err != 0 )
		return err;
	return err;
}

// Ustawienie wersji struktury SignerInfo
// Nie nastepuje weryfikacja poprawnosci.
long SignerInfo_set_CMSVersion(
	SignerInfo_t *signerInfo,
	CMSVersion_e version)
{
	if ( signerInfo == NULL )
		return ERR_WRONG_ARGUMENT;
	asn_long2INTEGER( &(signerInfo->version), version);
	return 0;
}

// Ustawienie pola z sygnatura struktury SignerInfo
// Wykonywana jest kopia sygnatury
long SignerInfo_set_Signature(
	SignerInfo_t *signerInfo,
	GenBuf_t *signature)
{
	long err = 0;
	if ( signerInfo == NULL )
		return ERR_WRONG_ARGUMENT;
	err = OCTET_STRING_fromBuf(&(signerInfo->signature),
		(char *)signature->buf, signature->size);
	if ( err != 0 )
		return err;
	return 0;
}

// Dodanie atrybutu do atrybutow podpisanych
// Atrybut jest dodawany do listy poprzez wskazanie, wiec nie nalezy
// usuwac go z pamieci po dodaniu
long SignedAttributes_add_Attribute(
	SignedAttributes_t *attrs,
	Attribute_t *attr)
{
	if ( (attrs==NULL) || (attr==NULL) )
		return ERR_WRONG_ARGUMENT;
	if ( asn_set_add( &(attrs->list), attr ) != 0 )
		return ERR_OPERATION_FAILED;
	return 0;
}

// Dodanie atrybutu do atrybutow niepodpisanych
// Atrybut jest dodawany do listy poprzez wskazanie, wiec nie nalezy
// usuwac go z pamieci po dodaniu
long UnsignedAttributes_add_Attribute(
	UnsignedAttributes_t *attrs,
	Attribute_t *attr)
{
	if ( (attrs==NULL) || (attr==NULL) )
		return ERR_WRONG_ARGUMENT;
	if ( asn_set_add( &(attrs->list), attr ) != 0 )
		return ERR_OPERATION_FAILED;
	return 0;
}

// Dodanie do pola SignerInfo atrybutow podpisanych
// UWAGA ! Wykonywana jest kopia z atrybutow, wiec struktura
// podanych atrybutow moze zostac usunieta
long SignerInfo_add_SignedAttributes(	SignerInfo_t *signerInfo,
							SignedAttributes_t *sigattrs)
{
char signingTime_OID[]={"1.2.840.113549.1.9.5"};
long err = 0;
long i;
	if ( sigattrs == NULL )
		return err;
	if ( signerInfo->signedAttrs == NULL )
		signerInfo->signedAttrs = mallocStructure(sizeof(SignedAttributes_t));
	if ( signerInfo->signedAttrs == NULL )
		return ERR_NO_MEMORY;
	for ( i=0; i<sigattrs->list.count; i++ )
	{
		Attribute_t *tmp = NULL;
		Attribute_t *tmp1 = NULL;

		if(OBJECT_IDENTIFIER_cmp(&(sigattrs->list.array[i]->attrType),signingTime_OID)!=BMD_OK)
		{
			err = asn_cloneContent( &asn_DEF_Attribute, sigattrs->list.array[i], (void **)&tmp );
			if ( err != 0 )
				return err;
		}
		else
		{
			SigAttr_Create_SigningTime(&tmp1);
			err = asn_cloneContent( &asn_DEF_Attribute, tmp1, (void **)&tmp );
			if(err !=0)
				return err;
			asn_DEF_Attribute.free_struct(&asn_DEF_Attribute,tmp1,0);tmp1=NULL;
		}

		err = SignedAttributes_add_Attribute( signerInfo->signedAttrs, tmp );
		if ( err != 0 )
			return err;
	}
	return err;
}

// Dodanie do pola SignerInfo atrybutow niepodpisanych
long SignerInfo_add_UnsignedAttributes(
	SignerInfo_t *signerInfo,
	UnsignedAttributes_t *unsigattrs)
{
	long i;
	long err = 0;
	if ( unsigattrs == NULL )
		return err;
	if ( signerInfo->unsignedAttrs == NULL )
		signerInfo->unsignedAttrs = mallocStructure(sizeof(UnsignedAttributes_t));
	if ( signerInfo->unsignedAttrs == NULL )
		return ERR_NO_MEMORY;
	for ( i=0; i<unsigattrs->list.count; i++ )
	{
		Attribute_t *tmp = NULL;
		err = asn_cloneContent( &asn_DEF_Attribute, unsigattrs->list.array[i], (void **)&tmp );
		if ( err != 0 )
			return err;
		err = UnsignedAttributes_add_Attribute( signerInfo->unsignedAttrs, tmp );
		if ( err != 0 )
			return err;
	}
	return err;
}

// Ustawienie danych do podpisania oraz nadanie im typu id-data
long SignedData_set_Data(
	SignedData_t *sigData,
	GenBuf_t *buf)
{
	long OID_id_data[] = {OID_CMS_ID_DATA_LONG};
	long err = 0;
	err = SignedData_set_Content(sigData, buf);
	if ( err != 0 )
		return err;
	return SignedData_set_ContentType(sigData,
		OID_id_data, sizeof(OID_id_data)/sizeof(*OID_id_data));
}

// zakodowanie SignedData do bufora
long der_encode_SignedData(SignedData_t *sigData, GenBuf_t **buf)
{
	long err = 0;

	if ( sigData == NULL )
		return ERR_WRONG_ARGUMENT;

	err=asn1_encode(&asn_DEF_SignedData,(void *)sigData, NULL, buf);
	if ( err != 0 )
		return err;

	return err;
}

// Funkcja umieszcza w ContentInfo struktur�SignedData. Jeeli
// w ContentInfo istniay ju jakie dane, zostan zwolnione,
// a na ich miejsce umieszczona podana strukturka
// Wykonywana jest kopia struktury SignedData
long ContentInfo_set_SignedData(ContentInfo_t* content, SignedData_t* sigData)
{
	long OID_id_signedData[] 	= {OID_CMS_ID_SIGNED_DATA_LONG};
	long ret;
	if ((content == NULL) || (sigData == NULL))
		return ERR_WRONG_ARGUMENT;

	// wyzeruj strukture ContentInfo
	asn_DEF_ContentInfo.free_struct( &asn_DEF_ContentInfo,
		content, 1);
	memset(content, 0, sizeof(ContentInfo_t));

	// ustaw identyfikator
	OBJECT_IDENTIFIER_set_arcs(
		&(content->contentType),
		OID_id_signedData,
		sizeof(*OID_id_signedData),
		sizeof(OID_id_signedData)/sizeof(*OID_id_signedData));
	// umiesc dane
	ret = ANY_fromType(&content->content, &asn_DEF_SignedData, sigData);

	if ( ret )
		return ret;
	return 0;
}

// Sprawdzenie, czy dany SignerInfo jest okreslonej wersji
// 0 - nie jest
// 1 - jest
long SignerInfo_is_Version(
	SignerInfo_t* sigInfo,
	CMSVersion_e version)
{
	long v = -1;
	if ( sigInfo == NULL )
		return 0;
	asn_INTEGER2long( &(sigInfo->version), &v );
	return v == version;
}

// czy ktorykolwiek SignerInfo z SignerInfos posiada wersj�podana
// w argumencie
// 1 - posiada
// 0 - nie posiada
long SignerInfos_hasVersion(
	SignerInfos_t *sigInfos,
	CMSVersion_e version)
{
	long i;
	if ( sigInfos == NULL )
		return 0;
	for ( i = 0; i<sigInfos->list.count; i++ )
	{
		if ( SignerInfo_is_Version( sigInfos->list.array[i], version ) == 1 )
			return 1;
	}
	return 0;
}

// na podstawie zbudowanej struktury SignedData sprobuj ustalic,
// jaka wersje nalezy nadac strukturze
// TODO: nie wypelnione do konca dla wersji 3 (poniewaz zawsze w
// SignerInfo nie tworze ski oraz dodawane sa dane tylko jako id-data
long SignedData_count_Version(SignedData_t *sigData)
{
char OID_id_data[] 		= {OID_CMS_ID_DATA};

	if ( (sigData->certificates && isAnyCertOther(sigData->certificates))
		|| (sigData->crls && isAnyCRLOther(sigData->crls)) )
		SignedData_set_CMSVersion(sigData, CMSVersion_v5);
	else if ((sigData->certificates) && isAnyCertWithVer(sigData->certificates, 2))
		SignedData_set_CMSVersion(sigData, CMSVersion_v4);
	else if (((sigData->certificates) && isAnyCertWithVer(sigData->certificates, 1))
		|| (SignerInfos_hasVersion(&(sigData->signerInfos), CMSVersion_v3))
		|| (OBJECT_IDENTIFIER_cmp(&(sigData->encapContentInfo.eContentType), OID_id_data))
		)
		SignedData_set_CMSVersion(sigData, CMSVersion_v3);
	else
		SignedData_set_CMSVersion(sigData, CMSVersion_v1);
	return 0;
}

// pobranie z ContenInfo struktury SignedData.
// Jest wykonywana kopia !, gdyz struktura SignedData jest dekodowana
long ContentInfo_get_SignedData(
	ContentInfo_t *content,
	SignedData_t **sigData)
{
	long OID_id_signedData[] = {OID_CMS_ID_SIGNED_DATA_LONG};
	long *oid = NULL;
	long oid_cnt;

	if (( content == NULL ) || ( sigData == NULL ))
		return ERR_WRONG_ARGUMENT;
	oid_cnt = OID_to_ulong_alloc2(&(content->contentType), &oid);
	if ( oid_cnt < 0 )
		return oid_cnt;
	if ( OID_cmp2(OID_id_signedData, sizeof(OID_id_signedData),
		oid, sizeof(*oid)*oid_cnt) )
	{
		free(oid);
		return ERR_OPERATION_FAILED; // not id-signed-data
	}
	free(oid);
	if ( ANY_to_type(&(content->content), &asn_DEF_SignedData, (void **)sigData) )
		return ERR_OPERATION_FAILED; // nieudana konwersja
	return 0;
}

// UWAGA ! Dane wynikowe nie s allokowane !
// Ustawiany jest jedynie wskanik oraz dugo�danych !
// Ma to zapobiega�powielaniu kopii tej samej informacji !
long SignedData_get_Content(
	SignedData_t *sigData,
	GenBuf_t *data)
{
	if ((sigData == NULL) || (data == NULL))
		return ERR_WRONG_ARGUMENT;
	if ( sigData->encapContentInfo.eContent == NULL )
		return 0;
	data->buf = (char *)(sigData->encapContentInfo.eContent->buf);
	data->size = sigData->encapContentInfo.eContent->size;
	return 0;
}

