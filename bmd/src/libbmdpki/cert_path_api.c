#include <bmd/common/bmd-common.h>

#include <bmd/libbmdpki/libbmdpki.h>
#include <bmd/libbmdpkcs12/libbmdpkcs12.h>
#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmderr/libbmderr.h>

#include <bmd/libbmdts/libbmdts.h>
#include <bmd/libbmdpkcs11/libbmdpkcs11.h>
#include <bmd/common/bmd-crypto_ds.h>

#include <bmd/common/bmd-const.h>

#include <bmd/libbmdcms/libbmdcms.h>

#include <bmd/libbmdasn1_common/SigPolicy/SignerAndVerifierRules.h>
#include <bmd/libbmdasn1_common/AuthorityKeyIdentifier.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>

#ifdef WIN32
#include <windows.h>
#include <Wincrypt.h>
#pragma warning(disable:4127)
#pragma warning(disable:4100)
#endif

typedef struct cert_keys_pair {
	GenBuf_t *subject_key_id;
	GenBuf_t *issuer_key_id;

	/* gdy sa certyfikaty bez SKI i AKI */
	GenBuf_t *subject_dn;
	GenBuf_t *issuer_dn;
} cert_keys_pair_t;

#define GET_AKI	0
#define GET_SKI 0

long _bmd_get_subject_issuer_DN(Certificate_t *asn1_cert,GenBuf_t **subject_DN,GenBuf_t **issuer_DN)
{
long status;

	/* pobranie DN podmiotu */
	if(subject_DN)
	{
		status=asn1_encode(&asn_DEF_Name,(void *)(&(asn1_cert->tbsCertificate.subject)), NULL, subject_DN);
		if(status!=BMD_OK)
		{
			PRINT_ERROR("LIBBMDPKIERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
			return BMD_ERR_OP_FAILED;
		}
	}

	/* pobranie DN wystawcy */
	if(issuer_DN)
	{
		status=asn1_encode(&asn_DEF_Name,(void *)(&(asn1_cert->tbsCertificate.issuer)), NULL, issuer_DN);
		if(status!=BMD_OK)
		{
			PRINT_ERROR("LIBBMDPKIERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
			return BMD_ERR_OP_FAILED;
		}
	}

	return BMD_OK;
}

/*
funkcja _cmp_issuer_with_authority_subject() porownuje wystawce certyfikatu
z podmiotem certyfikatu wystawcy (porownuje zawartosc pol a nie der)

@param issuerNameStruct to bufor z derem wystawcy certyfikatu (der struktury Name)
@param caSubjectNameStruct to bufor z derem podmiotu certyfikatu wystawcy (der struktury Name)

@retval 0 jesli issuerNameStruct oraz caSubjectNameStruct sa takie same
@retval 1 jesli issuerNameStruct oraz caSubjectNameStruct roznia sie
@retal <0 (odpowiednia wartosc ujemna) w przypadku bledu
*/
long _cmp_issuer_with_authority_subject(GenBuf_t *issuerNameStruct, GenBuf_t *caSubjectNameStruct)
{
	asn_dec_rval_t dec_result;
	Name_t *issuer=NULL;
	Name_t *ca_subject=NULL;
	long iter=0, jupiter=0;
	long attributeFound=0;

	RDNSequence_t *rdn_issuer=NULL;
	RDNSequence_t *rdn_ca_subject=NULL;

	long retVal=0;

	OCTET_STRING_t *issuerString=NULL; //UTF8String i PrintableString to typedefy OCTET_STRING
	OCTET_STRING_t *caSubjectString=NULL; //UTF8String i PrintableString to typedefy OCTET_STRING
	


	if(issuerNameStruct == NULL)
		{ return -1; }
	if(issuerNameStruct->size <= 0 || issuerNameStruct->buf == NULL)
		{ return -2; }
	if(caSubjectNameStruct == NULL)
		{ return -3; }
	if(caSubjectNameStruct->size <= 0 || caSubjectNameStruct->buf == NULL)
		{ return -4; }

	dec_result=ber_decode(0, &asn_DEF_Name, (void**)&issuer, issuerNameStruct->buf, issuerNameStruct->size);
	if(dec_result.code != RC_OK)
	{
		return -5;
	}

	dec_result=ber_decode(0, &asn_DEF_Name, (void**)&ca_subject, caSubjectNameStruct->buf, caSubjectNameStruct->size);
	if(dec_result.code != RC_OK)
	{
		asn_DEF_Name.free_struct(&asn_DEF_Name, issuer, 0); issuer=NULL;
		return -6;
	}

	rdn_issuer=&(issuer->choice.rdnSequence);
	rdn_ca_subject=&(ca_subject->choice.rdnSequence);

	if(rdn_issuer->list.count != rdn_ca_subject->list.count)
	{
		asn_DEF_Name.free_struct(&asn_DEF_Name, issuer, 0); issuer=NULL;
		asn_DEF_Name.free_struct(&asn_DEF_Name, ca_subject, 0); ca_subject=NULL;
		return 1; //rozna ilosc atrybutow
	}

	for(iter=0; iter<rdn_issuer->list.count; iter++)
	{
		attributeFound=0;
		
		for(jupiter=0; jupiter<rdn_ca_subject->list.count; jupiter++)
		{	
			/*zabezpieczenie (antiSEGV)*/
			if(rdn_issuer->list.array[iter]->list.count < 1 || rdn_ca_subject->list.array[jupiter]->list.count < 1)
				{ continue; }

			retVal=OID_cmp2(rdn_issuer->list.array[iter]->list.array[0]->type.buf,
				rdn_issuer->list.array[iter]->list.array[0]->type.size,
				rdn_ca_subject->list.array[jupiter]->list.array[0]->type.buf,
				rdn_ca_subject->list.array[jupiter]->list.array[0]->type.size);
			
			if(retVal != 0)
				{ continue; }

			/*wyciagam string z wystawca certyfikatu
			niezaleznie, czy jest to UTF8String , IA5String, czy PrintableString, implementacja odbywa sie na OCTET_STRING*/
			dec_result=ber_decode(NULL,&asn_DEF_PrintableString,(void**)&issuerString,rdn_issuer->list.array[iter]->list.array[0]->value.buf,rdn_issuer->list.array[iter]->list.array[0]->value.size);
			if(dec_result.code!=RC_OK)
			{
				dec_result=ber_decode(NULL,&asn_DEF_UTF8String,(void**)&issuerString,rdn_issuer->list.array[iter]->list.array[0]->value.buf,rdn_issuer->list.array[iter]->list.array[0]->value.size);
				if(dec_result.code!=RC_OK)
				{
					dec_result=ber_decode(NULL,&asn_DEF_IA5String,(void**)&issuerString,rdn_issuer->list.array[iter]->list.array[0]->value.buf,rdn_issuer->list.array[iter]->list.array[0]->value.size);
					if(dec_result.code!=RC_OK)
					{
						asn_DEF_Name.free_struct(&asn_DEF_Name, issuer, 0); issuer=NULL;
						asn_DEF_Name.free_struct(&asn_DEF_Name, ca_subject, 0); ca_subject=NULL;
						return -7;
					}
					
				}
			}
			
			/*wyciagam string z podmiotem z certyfikatu wystawcy
			niezaleznie, czy jest to UTF8String , IA5String, czy PrintableString, implementacja odbywa sie na OCTET_STRING*/
			dec_result=ber_decode(NULL,&asn_DEF_PrintableString,(void**)&caSubjectString,rdn_ca_subject->list.array[jupiter]->list.array[0]->value.buf,rdn_ca_subject->list.array[jupiter]->list.array[0]->value.size);
			if(dec_result.code!=RC_OK)
			{
				dec_result=ber_decode(NULL,&asn_DEF_UTF8String,(void**)&caSubjectString,rdn_ca_subject->list.array[jupiter]->list.array[0]->value.buf,rdn_ca_subject->list.array[jupiter]->list.array[0]->value.size);
				if(dec_result.code!=RC_OK)
				{
					dec_result=ber_decode(NULL,&asn_DEF_IA5String,(void**)&caSubjectString,rdn_ca_subject->list.array[jupiter]->list.array[0]->value.buf,rdn_ca_subject->list.array[jupiter]->list.array[0]->value.size);
					if(dec_result.code!=RC_OK)
					{
						asn_DEF_Name.free_struct(&asn_DEF_Name, issuer, 0); issuer=NULL;
						asn_DEF_Name.free_struct(&asn_DEF_Name, ca_subject, 0); ca_subject=NULL;
						asn_DEF_OCTET_STRING.free_struct(&asn_DEF_OCTET_STRING, issuerString, 0); issuerString=NULL;
						return -8;
					}
				}
			}
			
			if(issuerString->size == caSubjectString->size)
			{
				if(memcmp(issuerString->buf, caSubjectString->buf, issuerString->size) == 0)
				{
					attributeFound=1;
					asn_DEF_OCTET_STRING.free_struct(&asn_DEF_OCTET_STRING, issuerString, 0); issuerString=NULL;
					asn_DEF_OCTET_STRING.free_struct(&asn_DEF_OCTET_STRING, caSubjectString, 0); caSubjectString=NULL;
					break;
				}
			}
			asn_DEF_OCTET_STRING.free_struct(&asn_DEF_OCTET_STRING, issuerString, 0); issuerString=NULL;
			asn_DEF_OCTET_STRING.free_struct(&asn_DEF_OCTET_STRING, caSubjectString, 0); caSubjectString=NULL;
		}

		if(attributeFound == 0)
		{
			asn_DEF_Name.free_struct(&asn_DEF_Name, issuer, 0); issuer=NULL;
			asn_DEF_Name.free_struct(&asn_DEF_Name, ca_subject, 0); ca_subject=NULL;
			return 1; //roznia sie, bo brak atrybutu
		}
	}

	asn_DEF_Name.free_struct(&asn_DEF_Name, issuer, 0); issuer=NULL;
	asn_DEF_Name.free_struct(&asn_DEF_Name, ca_subject, 0); ca_subject=NULL;
	return 0; //identyczne
}

long _bmd_get_AKI_and_SKI(	Certificate_t *asn1_cert,
					GenBuf_t **aki,
					GenBuf_t **ski)
{
char subjectKeyID[]	= {"2.5.29.14"};
char issuerKeyID[]	= {"2.5.29.35"};
Extensions_t *extensions		= NULL;
Extension_t *ext				= NULL;
OCTET_STRING_t *tmp_octet		= NULL;
AuthorityKeyIdentifier_t *tmp_id	= NULL;
asn_dec_rval_t rval;
long i						= 0;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (asn1_cert==NULL)
	{
		return BMD_ERR_PARAM1;
	}
	if (asn1_cert->tbsCertificate.extensions==NULL)
	{
		return BMD_ERR_PARAM1;
	}

	extensions=asn1_cert->tbsCertificate.extensions;
	for(i=0;i<extensions->list.count;i++)
	{

		ext=extensions->list.array[i];
		if(OBJECT_IDENTIFIER_cmp(&(ext->extnID), subjectKeyID)==0)
		{
			if(ski)
			{

				rval=ber_decode(NULL,&asn_DEF_OCTET_STRING,(void **)&tmp_octet,ext->extnValue.buf,ext->extnValue.size);
				if(rval.code!=RC_OK)
				{
					BMD_FOK(BMD_ERR_FORMAT);
				}
				set_gen_buf2((char*)(tmp_octet->buf), tmp_octet->size,ski);
				asn_DEF_OCTET_STRING.free_struct(&asn_DEF_OCTET_STRING,tmp_octet,0);tmp_octet=NULL;
			}
		}
		else
		{
			if(OBJECT_IDENTIFIER_cmp(&(ext->extnID),issuerKeyID)==0)
			{
				if(aki)
				{
					rval=ber_decode(NULL,&asn_DEF_AuthorityKeyIdentifier,(void **)&tmp_id,ext->extnValue.buf,ext->extnValue.size);
					if(rval.code!=RC_OK)		{	BMD_FOK(BMD_ERR_FORMAT);	}

					if(tmp_id->keyIdentifier)
					{
						set_gen_buf2((char*)(tmp_id->keyIdentifier->buf), tmp_id->keyIdentifier->size,aki);
// 						PRINT_TEST(">>> PKL <<< Certyfikat ma pole keyIdentifier\n");
					}
					else
					{
// 						BMD_FOK(BMD_ERR_FORMAT);
// 						PRINT_TEST(">>> PKL <<< Certyfikat nie ma pola keyIdentifier\n");
					}

					asn_DEF_AuthorityKeyIdentifier.free_struct(&asn_DEF_AuthorityKeyIdentifier,tmp_id,0);tmp_id=NULL;
				}
			}
		}
	}
	return BMD_OK;
}


#ifdef WIN32

/*
funkcja _find_CA_in_store dla przekazywanego certyfikatu ma odnalezc
we wskazanym magazynie systemowym certyfikat wystawcy

@param store_name to lancuch znakowy z nazwa systemowego magazynu certyfikatow
	dopuszczalne wartosci ("CA", "ROOT", "MY")
@param checkCert to certyfikat, dla ktorego ma zostac znalezniony certyfikat wystawcy
@param found_CAcert to adres do alokowanego bufor ze znalezionym certyfikatem wystawcy (DER)

@retval 0 jesli znaleziono we wskazanym magazynie certyfikat wystawcy
@retval -1 jesli za store_name przekazano NULL
@retval -2 jesli za checkCert przekazano NULL
@retval -3 jesli za found_CAcert przekazano NULL
@retval -4 jesli wartosc wyluskana z found_CAcert nie jest wyNULLowana
@retval -5 jesli nie mozna zdekodowac sprawdzanego certyfikatu
@retval -6 jesli nie mozna wydobyc wymaganych danych ze sprawdzanego certyfikatu
@retval -7 jesli nie mozna otworzyc systemowego magazynu certyfikatow
@retval -8 jesli nie mozna zdekodowac certyfikatu z magazynu systemowego
@retval -9 jesli nie mozna wydobyc danych ze sprawdzanego certyfikatu z magazynu systemowego
@retval -10 jesli nie mozna odnalezc certyfikatu wystawcy we wskazanym magazynie

*/
long _find_CA_in_store(char *store_name, GenBuf_t* checkCert, GenBuf_t **found_CAcert)
{
	long status=0;
	asn_dec_rval_t rval;

	HCERTSTORE CAstore=NULL;
	PCCERT_CONTEXT CA_iterCert=NULL;
	GenBuf_t *checkCert_issuer=NULL;
	Certificate_t* check_CertStruct=NULL;

	GenBuf_t *CA_der=NULL;
	Certificate_t *CA_CertStruct=NULL;
	GenBuf_t *CA_subject=NULL;
	GenBuf_t *CA_issuer=NULL;


	if(store_name == NULL)
		{ return -1; }
	if(checkCert == NULL)
		{ return -2; }
	if(found_CAcert == NULL)
		{ return -3; }
	if(*found_CAcert != NULL)
		{ return -4; }

	rval=ber_decode(NULL,&asn_DEF_Certificate,(void **)&check_CertStruct,checkCert->buf,checkCert->size);
	if(rval.code!=RC_OK)
	{
		return -5;
	}

	//wydobycie issuer ze sprawdzanego certyfikatu
	status=_bmd_get_subject_issuer_DN(check_CertStruct,NULL,&checkCert_issuer);
	if(status != BMD_OK)
	{
		asn_DEF_Certificate.free_struct(&asn_DEF_Certificate,check_CertStruct,1);
		free(check_CertStruct); check_CertStruct=NULL;
		return -6;
	}

	//otwarcie magazynu certyfikatow
	CAstore=CertOpenSystemStore(0, store_name);
	if(CAstore == NULL)
	{
		asn_DEF_Certificate.free_struct(&asn_DEF_Certificate,check_CertStruct,1);
		free(check_CertStruct); check_CertStruct=NULL;
		free_gen_buf(&checkCert_issuer);
		return -7;
	}

	//wyszukiwanie w magazynie posrednich urzedow certyfikacji
	while(1) //while
	{
		if(CA_iterCert == NULL )
		{
			CA_iterCert=CertEnumCertificatesInStore(CAstore, NULL);
		}
		else
		{
			CA_iterCert=CertEnumCertificatesInStore(CAstore, CA_iterCert);
		}
		//jesli nie ma juz wiecej certyfikatow w magazynie
		if(CA_iterCert == NULL)
			{ break; }

		set_gen_buf2((char *)CA_iterCert->pbCertEncoded,CA_iterCert->cbCertEncoded,&CA_der);
		rval=ber_decode(NULL,&asn_DEF_Certificate,(void **)&CA_CertStruct,CA_der->buf,CA_der->size);
		if(rval.code!=RC_OK)
		{
			asn_DEF_Certificate.free_struct(&asn_DEF_Certificate,check_CertStruct,1);
			free(check_CertStruct); check_CertStruct=NULL;
			free_gen_buf(&CA_der);
			free_gen_buf(&checkCert_issuer);
			CertCloseStore(CAstore, 0);
			return -8;
		}

		status=_bmd_get_subject_issuer_DN(CA_CertStruct,&CA_subject,&CA_issuer);
		if(status != BMD_OK)
		{
			asn_DEF_Certificate.free_struct(&asn_DEF_Certificate,check_CertStruct,1);
			free(check_CertStruct); check_CertStruct=NULL;
			free_gen_buf(&CA_der);
			free_gen_buf(&checkCert_issuer);
			asn_DEF_Certificate.free_struct(&asn_DEF_Certificate,CA_CertStruct,1);
			free(CA_CertStruct); CA_CertStruct=NULL;
			CertCloseStore(CAstore, 0);
			return -9;
		}

		if(checkCert_issuer->size==CA_subject->size)
		{
			if( memcmp(checkCert_issuer->buf, CA_subject->buf, checkCert_issuer->size) == 0 )
			{
				status=bmd_verify_cert_with_cert(checkCert,CA_der);
				if(status == BMD_OK)
				{
					*found_CAcert=CA_der;
					asn_DEF_Certificate.free_struct(&asn_DEF_Certificate,check_CertStruct,1);
					free(check_CertStruct); check_CertStruct=NULL;
					free_gen_buf(&CA_subject);
					free_gen_buf(&CA_issuer);
					free_gen_buf(&checkCert_issuer);
					asn_DEF_Certificate.free_struct(&asn_DEF_Certificate,CA_CertStruct,1);
					free(CA_CertStruct); CA_CertStruct=NULL;
					CertCloseStore(CAstore, 0);
					return 0;
				}
			}
		}

		free_gen_buf(&CA_issuer);
		free_gen_buf(&CA_subject);
		free_gen_buf(&CA_der);
		asn_DEF_Certificate.free_struct(&asn_DEF_Certificate,CA_CertStruct,1);
		free(CA_CertStruct); CA_CertStruct=NULL;

	}//end while

	CertCloseStore(CAstore, 0);
	asn_DEF_Certificate.free_struct(&asn_DEF_Certificate,check_CertStruct,1);
	free(check_CertStruct); check_CertStruct=NULL;
	free_gen_buf(&checkCert_issuer);

	return -10; //nie znalazl
}

/*
funkcja bmd_verify_cert_path weryfikuje sciezke dla przekazywanego certyfikatu

@param checkCert to bufor z certyfikatem (DER), dla ktorego ma byc sprawdzona sciezka certyfikacji
@param cert_path to adres na tworzona liste buforow z certyfikatami wystawcow w sciezce certyfikacji

@retval 0 jesli zweryfikowano poprawnie sciezke certyfikacji
@retval -1 jesli za checkCert podano NULL
@retval -2 jesli za cert_path podano NULL
@retval -3 w przypadku problemow z alokacja pamieci
@retval -4 jesli nie odnaleziono wymaganego certyfikatu wystawcy w magazynach systemowych (dowolny poziom w sciezce)
@retval -5 jesli odnaleziony w magazynie certyfikat nie moze byc zdekodowany
@retval -6 jesli nie mozna wydobyc potrzebnych danych z odnalezionego certyfikatu wystawcy
@retval -7 jesli nie zgadza sie podpis certyfikatu ROOT w sciezce certyfikacji

*/
long bmd_verify_cert_path_sysStore(GenBuf_t *checkCert, GenBuf_t ***cert_path)
{

	long status=0;
	asn_dec_rval_t rval;

	char *storeNames[]={"CA", "ROOT"}; //moze byc jeszcze "MY"
	long num_storeNames=2;
	long iter=0;
	long num_certs_in_path=0;
	long iter_destroy_path=0;

	GenBuf_t *tmp=NULL;
	GenBuf_t *CA_copy=NULL;
	GenBuf_t *CA_der=NULL;
	Certificate_t *CA_CertStruct=NULL;
	//GenBuf_t *CA_aki=NULL;
	//GenBuf_t *CA_ski=NULL;
	GenBuf_t *CA_subject=NULL;
	GenBuf_t *CA_issuer=NULL;

	GenBuf_t **path=NULL; //rozmiar listy rowny ilosci wystawcow certyfikatow ze sciezki + 1 (na NULL)

	//Certificate_t *check_CertStruct=NULL;

	if(checkCert == NULL)
		{ return -1; }
	if(cert_path == NULL)
		{ return -2; }


	set_gen_buf2(checkCert->buf, checkCert->size,&tmp);

	path=(GenBuf_t**)calloc(1, sizeof(GenBuf_t*));
	if(path == NULL)
	{
		free_gen_buf(&tmp);
		return -3;
	}

	while(1) //poki nie dojdzie do certyfikatu ROOT
	{
		for(iter=0; iter<num_storeNames; iter++) //dla kazdego magazynu z listy
		{

			status=_find_CA_in_store(storeNames[iter], tmp, &CA_der);
			//jesli znaleziono juz certyfikat podpisujacy, to nie trzeba przeszukiwac pozostalych magazynow
			if(status == 0)
			{
				break;
			}

		} // end for

		//jesli nie znalazl certyfikatu wystawcy w zadnym z magazynow
		if(CA_der == NULL)
		{
			free_gen_buf(&tmp);
			for(iter_destroy_path=0; iter_destroy_path <= num_certs_in_path; iter_destroy_path++ )
				{ free_gen_buf(&(path[iter_destroy_path])); }
			free(path);
			return -4;
		}

		rval=ber_decode(NULL,&asn_DEF_Certificate,(void **)&CA_CertStruct,CA_der->buf, CA_der->size);
		if(rval.code!=RC_OK)
		{
			free_gen_buf(&tmp);
			free_gen_buf(&CA_der);
			for(iter_destroy_path=0; iter_destroy_path <= num_certs_in_path; iter_destroy_path++ )
				{ free_gen_buf(&(path[iter_destroy_path])); }
			free(path);
			return -5;
		}

		status=_bmd_get_subject_issuer_DN(CA_CertStruct,&CA_subject,&CA_issuer);

		asn_DEF_Certificate.free_struct(&asn_DEF_Certificate,CA_CertStruct,1);
		free(CA_CertStruct); CA_CertStruct=NULL;

		if(status != BMD_OK)
		{
			free_gen_buf(&tmp);
			free_gen_buf(&CA_der);
			for(iter_destroy_path=0; iter_destroy_path <= num_certs_in_path; iter_destroy_path++ )
				{ free_gen_buf(&(path[iter_destroy_path])); }
			free(path);
			return -6;
		}

		//spradzenie, czy to juz ROOT
		if(CA_issuer->size == CA_subject->size)
		{
			if( (memcmp(CA_issuer->buf, CA_subject->buf, CA_subject->size)) == 0 )
			{
				//sprawdzenie, czy faktycznie sam siebie podpisuje (certyfikat ROOT)
				status=bmd_verify_cert_with_cert(CA_der,CA_der);
				//jesli nie zgadza sie podpis ROOT (sam sebie nie podpisuje)
				if(status != BMD_OK)
				{
					free_gen_buf(&CA_der);
					free_gen_buf(&CA_subject);
					free_gen_buf(&CA_issuer);
					free_gen_buf(&tmp);
					for(iter_destroy_path=0; iter_destroy_path <= num_certs_in_path; iter_destroy_path++ )
						{ free_gen_buf(&(path[iter_destroy_path])); }
					free(path);
					return -7;
				}

				//dodawanie certyfikatu wystawcy do listy (sciezki)
				num_certs_in_path++;
				path=(GenBuf_t **)realloc(path, (num_certs_in_path+1)*sizeof(GenBuf_t *));
				path[num_certs_in_path-1]=CA_der;
				path[num_certs_in_path]=NULL;

				CA_der=NULL;
				free_gen_buf(&CA_subject);
				free_gen_buf(&CA_issuer);
				free_gen_buf(&tmp);

				*cert_path=path;
				return 0;
			}
		}
		free_gen_buf(&CA_subject);
		free_gen_buf(&CA_issuer);

		//dodawanie certyfikatu wystawcy do listy (sciezki)
		num_certs_in_path++;
		set_gen_buf2(CA_der->buf, CA_der->size, &CA_copy);
		path=(GenBuf_t **)realloc(path, (num_certs_in_path+1)*sizeof(GenBuf_t *));
		path[num_certs_in_path-1]=CA_copy;
		CA_copy=NULL;
		path[num_certs_in_path]=NULL;


		// sprawdzanym jest teraz CA_der
		free_gen_buf(&tmp);
		tmp=CA_der; CA_der=NULL;

	} // end while zewnetrzny

	/* martwy kod - zwrot wartosci tylko w petli while
	free_gen_buf(&tmp);
	for(iter_destroy_path=0; iter_destroy_path <= num_certs_in_path; iter_destroy_path++ )
		{ free_gen_buf(&(path[iter_destroy_path])); }
	free(path);

	return -10; //nie znalazl
	*/
}

#endif //WIN32


long bmd_verify_cert_path(GenBuf_t *cert,char *tcc,GenBuf_t ***cert_path,void *opt)
{
long i							= 0;
long j							= 0;
long k							= -1;
long count						= 0;
long count_invalid					= 0;
long flag						= 0;
long path_counter					= 0;
long status						= 0;
char **certs					= NULL;
long *certs_invalid				= NULL;
GenBuf_t *tmp					= NULL;
GenBuf_t *tmp1					= NULL;
GenBuf_t *tmp_certificate_source		= NULL;
GenBuf_t *tmp_certificate_path		= NULL;
cert_keys_pair_t **pairs			= NULL;
asn_dec_rval_t rval;
Certificate_t *path_cert			= NULL;
Certificate_t *check_cert			= NULL;

#ifdef WIN32
	wchar_t *wideFilename=NULL;
#endif

	PRINT_INFO("LIBBMDPKIINF Verifying certificate path\n");
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(cert==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(tcc==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	/************************************************************************/
	/*	otworz katalog "tcc" i pobierz z niego wszystkie certyfikaty	*/
	/************************************************************************/
	status=bmd_get_files_from_dir(tcc,&certs);
	if(status!=BMD_OK)
	{
		PRINT_ERROR("LIBBMDPKIERR %s. Error=%i\n",GetErrorMsg(BMD_VERIFY_LOGIN_CERTIFICATE_CERT_STORE_DIR_ERROR), BMD_VERIFY_LOGIN_CERTIFICATE_CERT_STORE_DIR_ERROR);
		return BMD_VERIFY_LOGIN_CERTIFICATE_CERT_STORE_DIR_ERROR;
	}

	if(certs==NULL)
	{
		PRINT_ERROR("LIBBMDPKIERR %s. Error=%i\n",GetErrorMsg(BMD_VERIFY_LOGIN_CERTIFICATE_NO_CERTS_IN_CERT_STORE), BMD_VERIFY_LOGIN_CERTIFICATE_NO_CERTS_IN_CERT_STORE);
		return BMD_VERIFY_LOGIN_CERTIFICATE_NO_CERTS_IN_CERT_STORE;
	}

	while(certs[i])
	{
		i++;

	}
	count=i;

	pairs=(cert_keys_pair_t **)calloc(count+2, sizeof(cert_keys_pair_t *));

	certs_invalid=(long *)malloc(sizeof(long)*(count+1));
	memset(certs_invalid,-1,sizeof(long)*(count));

	i=0;

	while(certs[i])
	{
// 		PRINT_TEST(">>> PKL <<< sprawdzany certyfikat: %s\n",certs[i]);
#ifdef WIN32
		UTF8StringToWindowsString(certs[i], &wideFilename, NULL);
		status=bmd_load_binary_content_wide(wideFilename, &tmp);
		free(wideFilename); wideFilename=NULL;
#else
		PRINT_INFO("LIBBMDPKIINF Reading certificate %s\n",certs[i]);
		status=bmd_load_binary_content(certs[i],&tmp);
#endif
		if(status!=BMD_OK)
		{
			certs_invalid[count_invalid]=i;
			count_invalid++;
			i++;
			continue;
		}

		/************************************/
		/*	zdekodowanie certyfikatu	*/
		/************************************/
		rval=ber_decode(NULL,&asn_DEF_Certificate,(void **)&path_cert,tmp->buf,tmp->size);
		if(rval.code!=RC_OK)
		{
			certs_invalid[count_invalid]=i;
			count_invalid++;
			i++;
			free_gen_buf(&tmp);
			asn_DEF_Certificate.free_struct(&asn_DEF_Certificate,path_cert,0);path_cert=NULL;
			continue;
		}

		pairs[i]=(cert_keys_pair_t *)calloc(1, sizeof(cert_keys_pair_t));
		status=_bmd_get_AKI_and_SKI(path_cert,&(pairs[i]->issuer_key_id),&(pairs[i]->subject_key_id));
		if(status!=BMD_OK)
		{
			certs_invalid[count_invalid]=i;
			count_invalid++;
			i++;
			free_gen_buf(&tmp);
			asn_DEF_Certificate.free_struct(&asn_DEF_Certificate,path_cert,0);path_cert=NULL;
			continue;
		}

		status=_bmd_get_subject_issuer_DN(path_cert,&(pairs[i]->subject_dn),&(pairs[i]->issuer_dn));
		if(status!=BMD_OK)
		{
			PRINT_ERROR("LIBBMDPKIERR Invalid result format while processing %s certificate. Error=%i\n",certs[i],BMD_ERR_FORMAT);
			certs_invalid[count_invalid]=i;
			count_invalid++;
			i++;
			free_gen_buf(&tmp);
			asn_DEF_Certificate.free_struct(&asn_DEF_Certificate,path_cert,0);path_cert=NULL;
			continue;
		}

		if(pairs[i]->issuer_key_id==NULL)
		{
			/* dany certyfikat nie ma rozszerzenia authorityKeyIdentifier - wiec na 99% jest samopodpisany - sprawdzenie ;-) */
		}

		free_gen_buf(&tmp);
		asn_DEF_Certificate.free_struct(&asn_DEF_Certificate,path_cert,0);path_cert=NULL;

		i++;
	}

	rval=ber_decode(NULL,&asn_DEF_Certificate,(void **)&check_cert,cert->buf,cert->size);
	if(rval.code!=RC_OK)
	{
		PRINT_ERROR("LIBBMDPKIERR %s. Error=%i\n",GetErrorMsg(BMD_VERIFY_LOGIN_CERTIFICATE_DECODE_ERROR),BMD_VERIFY_LOGIN_CERTIFICATE_DECODE_ERROR);
		return BMD_VERIFY_LOGIN_CERTIFICATE_DECODE_ERROR;
	}

	status=_bmd_get_AKI_and_SKI(check_cert,&tmp,NULL);
	if(status!=BMD_OK)
	{
		PRINT_ERROR("LIBBMDPKIERR %s. Error=%i\n",GetErrorMsg(BMD_VERIFY_LOGIN_CERTIFICATE_INVALID_FORMAT),BMD_VERIFY_LOGIN_CERTIFICATE_INVALID_FORMAT);
		return BMD_VERIFY_LOGIN_CERTIFICATE_INVALID_FORMAT;
	}

	status=_bmd_get_subject_issuer_DN(check_cert,NULL,&tmp1);
	if(status!=BMD_OK)
	{
		PRINT_ERROR("LIBBMDPKIERR %s. Error=%i\n",GetErrorMsg(BMD_VERIFY_LOGIN_CERTIFICATE_INVALID_FORMAT),BMD_VERIFY_LOGIN_CERTIFICATE_INVALID_FORMAT);
		return BMD_VERIFY_LOGIN_CERTIFICATE_INVALID_FORMAT;

	}

	set_gen_buf2(cert->buf,cert->size,&tmp_certificate_source);

	/******************************************/
	/*	dla kazdego certyfikatu ze sciezki	*/
	/******************************************/
	for(i=0;i<count;i++)
	{
		k=0;
		for (j=0; j<count_invalid; j++)
		{
			if (i==certs_invalid[j])
			{
				k=1;
				break;
			}
		}
		if (k==1)
		{
			continue;
		}
		if(tmp1->size==pairs[i]->subject_dn->size)
		{
			if(_cmp_issuer_with_authority_subject(tmp1, pairs[i]->subject_dn) == 0)
			{
				/******************************************************************************************************/
				/*	znaleziono certyfikat w sciezce - najpierw weryfikacja czy ten co znaleziono podpisal zrodlowy	*/
				/******************************************************************************************************/
#ifdef WIN32
				UTF8StringToWindowsString(certs[i], &wideFilename, NULL);
				status=bmd_load_binary_content_wide(wideFilename,&tmp_certificate_path);
				free(wideFilename); wideFilename=NULL;
#else
				status=bmd_load_binary_content(certs[i],&tmp_certificate_path);
#endif
				if(status!=BMD_OK)
				{
					PRINT_ERROR("LIBBMDPKIERR %s. Error=%i\n",GetErrorMsg(BMD_VERIFY_LOGIN_CERTIFICATE_LOAD_CERT_ERROR),BMD_VERIFY_LOGIN_CERTIFICATE_LOAD_CERT_ERROR);
					return BMD_VERIFY_LOGIN_CERTIFICATE_LOAD_CERT_ERROR;
				}

				status=bmd_verify_cert_with_cert(tmp_certificate_source,tmp_certificate_path);
				if(status!=BMD_OK)
				{

					PRINT_WARNING("LIBBMDPKIWARNING %s. Status=%i. Possibly CA certificate " \
					"is newer/older than the original issuer of the certificate or is corrupted.\n", \
					GetErrorMsg(BMD_VERIFY_LOGIN_CERTIFICATE_WITH_CERT_ERROR), \
					BMD_VERIFY_LOGIN_CERTIFICATE_WITH_CERT_ERROR);
					free_gen_buf(&tmp_certificate_path);
					/*return BMD_VERIFY_LOGIN_CERTIFICATE_WITH_CERT_ERROR;*/
					continue;
				}

				free_gen_buf(&tmp1);
				set_gen_buf2(pairs[i]->issuer_dn->buf,pairs[i]->issuer_dn->size,&tmp1);
				free_gen_buf(&tmp_certificate_source);
				set_gen_buf2(tmp_certificate_path->buf,tmp_certificate_path->size,&tmp_certificate_source);
				free_gen_buf(&tmp_certificate_path);

				if(pairs[i]->subject_dn->size==pairs[i]->issuer_dn->size)
				{
					if(memcmp(pairs[i]->subject_dn->buf,pairs[i]->issuer_dn->buf,
					pairs[i]->subject_dn->size)==0)
					{

						flag=1;
						if(cert_path)
						{

							(*cert_path)=(GenBuf_t **)realloc(*cert_path ,
							sizeof(GenBuf_t *)*(path_counter+2));
							(*cert_path)[path_counter]=NULL; (*cert_path)[path_counter+1]=NULL;
#ifdef WIN32
							UTF8StringToWindowsString(certs[i], &wideFilename, NULL);
							status=bmd_load_binary_content_wide(wideFilename, &((*cert_path)[path_counter]));
							free(wideFilename); wideFilename=NULL;
#else
							status=bmd_load_binary_content(certs[i], &((*cert_path)[path_counter]));
#endif
							path_counter++;
						}
						break;
					}
				}
				if(cert_path)
				{

					(*cert_path)=(GenBuf_t **)realloc(*cert_path,sizeof(GenBuf_t *)*(path_counter+2));
					(*cert_path)[path_counter]=NULL;(*cert_path)[path_counter+1]=NULL;
#ifdef WIN32
					UTF8StringToWindowsString(certs[i], &wideFilename, NULL);
					status=bmd_load_binary_content_wide(wideFilename,&((*cert_path)[path_counter]));
					free(wideFilename); wideFilename=NULL;
#else
					status=bmd_load_binary_content(certs[i],&((*cert_path)[path_counter]));
#endif
					path_counter++;
				}
				i=-1;
			}
		}
	}
	free_gen_buf(&tmp_certificate_source);
	free_gen_buf(&tmp);
	free_gen_buf(&tmp1);
	asn_DEF_Certificate.free_struct(&asn_DEF_Certificate,check_cert,0);check_cert=NULL;
	i=0;

	if(certs)
	{
		while(certs[i])
		{
			free(certs[i]);certs[i]=NULL;
			i++;
		}
		free(certs);certs=NULL;
	}

	i=0;
	if(pairs)
	{

		while(pairs[i])
		{
			free_gen_buf(&(pairs[i]->issuer_key_id));
			free_gen_buf(&(pairs[i]->subject_key_id));
			free_gen_buf(&(pairs[i]->subject_dn));
			free_gen_buf(&(pairs[i]->issuer_dn));
			free(pairs[i]);pairs[i]=NULL;
			i++;
		}
		free(pairs);pairs=NULL;
	}
	if(flag==0)
	{
		PRINT_ERROR("LIBBMDPKIERR %s. Error=%i\n",GetErrorMsg(BMD_VERIFY_LOGIN_CERTIFICATE_CERT_PATH),BMD_VERIFY_LOGIN_CERTIFICATE_CERT_PATH);
		return BMD_VERIFY_LOGIN_CERTIFICATE_CERT_PATH;
	}
	return BMD_OK;
}
