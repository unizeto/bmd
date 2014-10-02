#include <bmd/libbmdcms/libbmdcms.h>
#include <bmd/libbmdpki/libbmdpki.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmderr/libbmderr.h>

#ifdef WIN32
#pragma warning(disable:4127)
#pragma warning(disable:4100)
#endif

// Funkcja wytwarza atrybut MessageDigest na podstawie przekazanych
// do niej danych. Rodzaj skrotu jest wyznaczany poprzez flage
long Attribute_generate_MessageDigest(	Attribute_t **attribute,
							GenBuf_t *buf,
							long flags)
{
	long oid_md[] = {OID_ATTRIBUTE_MESSAGEDIGEST_LONG};
	long oid_md_size;
	GenBuf_t *computed_md=NULL;

	OCTET_STRING_t *value = NULL;
	ANY_t *attrvalue = NULL;
	long err = 0;
	bmd_crypt_ctx_t *hash_ctx=NULL;

	if(attribute==NULL)		{	return BMD_ERR_PARAM1;	}
	if((*attribute)!=NULL)		{	return BMD_ERR_PARAM1;	}
	if(buf==NULL)			{	return BMD_ERR_PARAM2;	}
	if(buf->buf==NULL)		{	return BMD_ERR_PARAM2;	}
	if(buf->size<=0)			{	return BMD_ERR_PARAM2;	}

	(*attribute)=(Attribute_t *)malloc(sizeof(Attribute_t));
	if ( (*attribute) == NULL )
            return BMD_ERR_MEMORY;
        memset(*attribute,0,sizeof(Attribute_t));

        oid_md_size=sizeof(oid_md)/sizeof(long);
	err = OBJECT_IDENTIFIER_set_arcs(&((*attribute)->attrType),oid_md, sizeof(long),oid_md_size);
	if ( err )
		{err = ERR_OPERATION_FAILED; return BMD_ERR_OP_FAILED;}

	value = mallocStructure(sizeof(OCTET_STRING_t));
	if ( value == NULL )
		{err = ERR_NO_MEMORY; return BMD_ERR_MEMORY;}
        if( flags & BMD_CMS_SIG_LOB )
	{
            set_gen_buf2(buf->buf,buf->size,&computed_md);
	    OCTET_STRING_fromBuf(value,(char *)buf->buf,buf->size);
	}
        else
        {
            BMD_FOK(bmd_set_ctx_hash(&hash_ctx,BMD_HASH_ALGO_SHA1));
            BMD_FOK(bmd_hash_data(buf,&hash_ctx,&computed_md,NULL));
            OCTET_STRING_fromBuf(value, (char *)computed_md->buf,(int)computed_md->size);
	    BMD_FOK(bmd_ctx_destroy(&hash_ctx));
        }

	attrvalue = mallocStructure(sizeof(ANY_t));
	if ( attrvalue == NULL )
		{err = ERR_NO_MEMORY; return BMD_ERR_MEMORY;}
	err = ANY_fromType(attrvalue, &asn_DEF_OCTET_STRING, value);
	if ( err  )
		{err = ERR_OPERATION_FAILED; return BMD_ERR_OP_FAILED;}
	if ( asn_set_add(&( (*attribute)->attrValues.list ), attrvalue) != 0 )
		{err = ERR_OPERATION_FAILED; return BMD_ERR_OP_FAILED;}

	// zwalnianie pamieci
	if ( err != 0 )
	{
		if ( *attribute )
			asn_DEF_Attribute.free_struct(
				&asn_DEF_Attribute, *attribute, 0);
	}
	if ( value )
		asn_DEF_OCTET_STRING.free_struct(
			&asn_DEF_OCTET_STRING, value, 0);

        free_gen_buf(&computed_md);

	return BMD_OK;
}

// Wygenerowanie odbiorcy
// Odbiorca jest generowany o typie KeyTransRecipientInfo
// Argumnety nie podlegaja sprawdzeniu - internal use
// Struktura recInfo jest ju zaalokowana na wejsciu !
long RecipientInfo_generate(
	RecipientInfo_t *recInfo,
	GenBuf_t *certificate,
	GenBuf_t *cipherKey)
{
	long oid_rsaencryption[] = {OID_RSAENCRYPTION_LONG};
	long err = 0;
	Certificate_t *cert = NULL;

	ber_decode(0, &asn_DEF_Certificate, (void **)&cert,
					   certificate->buf, certificate->size);

	// Budowanie KeyTransRecipientInfo
	recInfo->present = RecipientInfo_PR_ktri;
	{
		// Budowanie RecipientIdentifier jako IssuerAndSerialNumber

		Name_t *issuerptr = &(recInfo->choice.ktri.rid.choice.issuerAndSerialNumber.issuer);
		CertificateSerialNumber_t *serialptr = &(recInfo->choice.ktri.rid.choice.issuerAndSerialNumber.serialNumber);
		/*Name_t *issuerptr=NULL;
		CertificateSerialNumber_t *serialptr=NULL;*/

		recInfo->choice.ktri.rid.present = RecipientIdentifier_PR_issuerAndSerialNumber;
		err = asn_cloneContent( &asn_DEF_Name, &(cert->tbsCertificate.issuer), (void **)&(issuerptr));
		if ( err != 0 )
			return err;
		err = asn_cloneContent( &asn_DEF_INTEGER, &(cert->tbsCertificate.serialNumber), (void **)&(serialptr));
		if ( err != 0 )
			return err;
		// Skoro jest wybrany IssuerAndSerialNumber to ustaw wersje na 0
		asn_long2INTEGER( &(recInfo->choice.ktri.version), CMSVersion_v0);

	}
	{
		// Budowanie keyEncryptionAlgorithm
		// z zalozenia jest to na razie tylko RSAEncryption
		AlgorithmIdentifier_t *recalgptr = &(recInfo->choice.ktri.keyEncryptionAlgorithm);
		AlgorithmIdentifier_set_OID(recalgptr,
			oid_rsaencryption,
			sizeof(oid_rsaencryption)/sizeof(long));
		if ( err != 0 )
			return err;
	}
	{
		// Budowanie encryptedKey, czyli zaszyfrowanie klucza szyfrujacego
		// kluczem publicznym odbiorcy
		/*char *blob = getBlobFromSeq(cipherKey->buf, cipherKey->size, 3);
		long bloblen = getBlobLenFromSeq(cipherKey->buf, cipherKey->size, 3);*/

		OCTET_STRING_fromBuf( &(recInfo->choice.ktri.encryptedKey), (char *)cipherKey->buf, (int)cipherKey->size );
//		FILE *p = fopen("x.key", "wb");
//		fwrite(blob, bloblen, 1, p);
//		fclose(p);
	}
	// Koniec Budowania KeyTransRecipientInfo
	asn_DEF_Certificate.free_struct(&asn_DEF_Certificate,cert,0);cert=NULL;
	return err;
}

/*
 * Przyrownanie OIDa umieszczonego w AlgorithmIdentifier z
 * OIDem przekazanym do funkcji w postaci tablicy
 * Zwraca zero kiedy sa sobie rowne
 */
long AlgorithmIdentifier_cmp_OID(
	AlgorithmIdentifier_t *algId,
	long *oid_tab,
	long oid_size)
{
	long *alg_oid = NULL;
	long alg_oid_cnt;
	long res;
	if ( algId == NULL )
		return ERR_WRONG_ARGUMENT;
	alg_oid_cnt = OID_to_ulong_alloc2(&(algId->algorithm), &alg_oid);
	if ( alg_oid_cnt < 0 )
		return alg_oid_cnt;
	res = OID_cmp2(alg_oid, alg_oid_cnt * sizeof(long),
		oid_tab, oid_size * sizeof(long));
	free(alg_oid);
	return res;

}

/*
 * Ustawienie identyfikatora OID z tablicy w zmiennej typu
 * AlgorithmIdentifier
 */
long AlgorithmIdentifier_set_OID(
	AlgorithmIdentifier_t *digest,
	long* oid_table,
	long oid_size)
{
	long err = 0;
	err = OBJECT_IDENTIFIER_set_arcs(&(digest->algorithm),
		oid_table, sizeof(long), oid_size);
	if ( err != 0 )
		return ERR_OPERATION_FAILED;
	return err;
}

// sprawdza czy na liscie certyfikatow znajduje sie podany certyfikat.
// Wyszukiwanie odbywa sie na podstawie skrotu obliczonego z wystawcy
// certyfikatu oraz jego numeru serial
// rezultaty:
// <0 bd
// =0 nie znalaz
// =1 znalaz
// >1 nie zdefiniowane
long CertificateSet_is_Certificate(CertificateSet_t *certs, Certificate_t *cert)
{
	long i;
	char hashin[16];
	char hashcmp[16];
	CertificateSerialNumber_t *serialin;

	long err = 0;
	if ( certs == NULL )
		return 0;

	serialin = &(cert->tbsCertificate.serialNumber);
	err = Name_count_hash(&(cert->tbsCertificate.issuer), hashin, -1);
	if ( err )
		return err;

	for ( i=0; i<certs->list.count; i++ )
	{
		if ( certs->list.array[i]->present == CertificateChoices_PR_certificate )
		{
			Certificate_t *certcmp = &(certs->list.array[i]->choice.certificate);
			CertificateSerialNumber_t *serialcmp = &(certcmp->tbsCertificate.serialNumber);
			err = Name_count_hash( &(certcmp->tbsCertificate.issuer), hashcmp, -1);
			if ( err )
				return err;
			if ( serialin->size != serialcmp->size )
				continue;
			if ( memcmp(serialin->buf, serialcmp->buf, serialin->size) )
				continue;
			if ( !memcmp(hashin, hashcmp, 16) )
				return 1;
		}
	}

	return err;
}

// Funkcja generuje atrybut jako ContentType z ustawionym typem - dane
// Funkcja allokuje pami� ! dla Attribute
long Attribute_generate_ContentType_Data(	Attribute_t **attribute)
{
long oid_attribute_contenttype[] = {OID_ATTRIBUTE_CONTENTTYPE_LONG};
long oid_id_data[] = {OID_CMS_ID_DATA_LONG};

	OBJECT_IDENTIFIER_t *value = NULL;
	ANY_t *attrvalue = NULL;
	long err = 0;

	// zwodnicza petla
	while ( 1 )
	{

	if ( attribute == NULL )
		{err = ERR_WRONG_ARGUMENT; break;}
	if ( *attribute == NULL )
	{
		*attribute = mallocStructure(sizeof(Attribute_t));
		if ( *attribute == NULL )
			{err = ERR_NO_MEMORY; break;}
//		memset(*attribute, 0, sizeof(Attribute_t));
	}
	err = OBJECT_IDENTIFIER_set_arcs(
		&((*attribute)->attrType),
		oid_attribute_contenttype, sizeof(long),
		sizeof(oid_attribute_contenttype)/sizeof(*oid_attribute_contenttype));
	if ( err )
		{err = ERR_OPERATION_FAILED; break;}
	value = mallocStructure(sizeof(OBJECT_IDENTIFIER_t));
	if ( value == NULL )
		{err = ERR_NO_MEMORY; break;}
//	memset(value, 0, sizeof(OBJECT_IDENTIFIER_t));
	err = OBJECT_IDENTIFIER_set_arcs(
		value, oid_id_data, sizeof(long),
		sizeof(oid_id_data)/sizeof(*oid_id_data));
	if ( err )
		{err = ERR_OPERATION_FAILED; break;}
	attrvalue = mallocStructure(sizeof(ANY_t));
	if ( attrvalue == NULL )
		{err = ERR_NO_MEMORY; break;}
//	memset(attrvalue, 0, sizeof(ANY_t));
	// TAKIE WYWOLANIE BUBUJE OBIEKT OD NOWA I REZERWUJE SOBIE PAMIEC
	err = ANY_fromType(attrvalue, &asn_DEF_OBJECT_IDENTIFIER, value);
	if ( err  )
		{err = ERR_OPERATION_FAILED; break;}
	// DODAWANIE TA FUNKCJA NIE ROBI KOPII LECZ PRZEKAZUJE WSKAZANIE
	// DLA LISTY. NIE WOLNO WIEC USUWAC DODAWANEGO OBIEKTU !!!
	if ( asn_set_add(&( (*attribute)->attrValues.list ), attrvalue) != 0 )
		{err = ERR_OPERATION_FAILED; break;}
//	free(value);

	break;
	}

	// zwalnianie pamieci
	if ( err != 0 )
	{
		if ( *attribute )
			asn_DEF_Attribute.free_struct(
				&asn_DEF_Attribute, *attribute, 0);
		*attribute = NULL;
	}
	if ( value )
		asn_DEF_OBJECT_IDENTIFIER.free_struct(
			&asn_DEF_OBJECT_IDENTIFIER, value, 0);

	return err;
}

// zakodowanie ContentInfo do bufora
long der_encode_ContentInfo(ContentInfo_t *contentInfo, GenBuf_t **buf)
{
	long err = 0;

	if ( contentInfo == NULL )
		return ERR_WRONG_ARGUMENT;

	err=asn1_encode(&asn_DEF_ContentInfo,contentInfo, NULL, buf);
	if ( err != 0 )
		return err;

	return err;
}

// sprawdzenie, czy na liscie certyfikatow jest jakikolwiek certyfikat,
// wykreowany jako OTHER
long isAnyCertOther(CertificateSet_t *certs)
{
	// Brak OTHER ! na liscie typow
/*	long i;
	for (i=0; i<certs->list.count; i++)
	{
		if ( certs->list.array[i]->present == CertificateChoices_PR_other)
			return 1;
	}*/
	return 0;
}

// sprawdzenie czy na liscie certyfikatow znajduje sie certyfikat
// o podanym numerze wersji
long isAnyCertWithVer(CertificateSet_t *certs, long ver)
{
	long i;
	if ( certs == NULL )
		return 0;
	for ( i=0; i<certs->list.count; i++ )
	{
		if ( certs->list.array[i]->present == CertificateChoices_PR_certificate )
		{
			if ( certs->list.array[i]->choice.certificate.tbsCertificate.version )
			{
				long v;
				asn_INTEGER2long( certs->list.array[i]->choice.certificate.tbsCertificate.version, &v );
				if (v == ver-1)
					return 1;
			}
		}
	}
	return 0;
}

// sprawdzenie, czy na liscie CRL jest lista o typu OTHER
long isAnyCRLOther(RevocationInfoChoices_t *crls)
{
	long i;
	if ( crls == NULL )
		return 0;
	for ( i=0; i<crls->list.count; i++)
	{
		if ( crls->list.array[i]->present == RevocationInfoChoice_PR_other )
			return 1;
	}
	return 0;
}

// Sprawdzenie czy jeden z RecipientInfo w strukturze RecipientInfos
// nie jest zbudowany jako okreslony typ CHOICE
//
// 1 - posiada RecipientInfo danego typu
// 0 - nie posiada
long RecipientInfos_has_RecipientInfo_present(
	RecipientInfos_t *recInfos,
	RecipientInfo_PR present)
{
	long i;
	if ( recInfos == NULL )
		return 0;
	for ( i = 0; i<recInfos->list.count; i++ )
	{
		if ( recInfos->list.array[i]->present == present )
			return 1;
	}
	return 0;
}

// Sprawdzenie, czy dany RecipientInfo jest okreslonej wersji
// 0 - nie jest
// 1 - jest
long RecipientInfo_is_Version(
	RecipientInfo_t* recInfo,
	CMSVersion_e version)
{
	long v = -1;
	if ( recInfo == NULL )
		return 0;
	if ( recInfo->present == RecipientInfo_PR_NOTHING )
		return 0;
	else if ( recInfo->present == RecipientInfo_PR_ktri )
		asn_INTEGER2long( &(recInfo->choice.ktri.version), &v );
	else if ( recInfo->present == RecipientInfo_PR_kari )
		asn_INTEGER2long( &(recInfo->choice.kari.version), &v );
	else if ( recInfo->present == RecipientInfo_PR_kekri )
		asn_INTEGER2long( &(recInfo->choice.kekri.version), &v );
	else if ( recInfo->present == RecipientInfo_PR_pwri )
		asn_INTEGER2long( &(recInfo->choice.pwri.version), &v );
	else if ( recInfo->present == RecipientInfo_PR_ori )
		return 0;
	else
		return 0;
	return v == version;
}

// Sprawdz, czy wszystkie RecipientInfo w RecipientInfos maja
// okreslona wersje
// 0 - nie maja
// 1 - maja
long RecipientInfos_allVersion(
	RecipientInfos_t *recInfos,
	CMSVersion_e version)
{
	long i;
	if ( recInfos == NULL )
		return 0;
	for ( i = 0; i<recInfos->list.count; i++ )
	{
		if ( RecipientInfo_is_Version( recInfos->list.array[i], version ) == 0 )
			return 0;
	}
	return 1;
}
