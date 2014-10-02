#include <bmd/libbmdpkcs12/libbmdpkcs12.h>
#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmderr/libbmderr.h>

/*dokumentowane i uporzadkowane przez WSZ*/

/**

funkcja _bmd_binary_content_to_BIO() laduje zawartosc genbufa do BIO

@param input (parametr wejsciowy) to wskaznik na bufor (z zalozenia z zawartoscia pliku PFX)
@param out_bio (parametr wyjsciowy) to wskaznik na tworzone BIO potrzebne pozniej do uzyskania PKCS12

@retval BMD_OK
@retval lub odpowiedni kod bledu
*/
long _bmd_binary_content_to_BIO(GenBuf_t *input,BIO *out_bio)
{
long nwrite=0;

	if(input==NULL)
	{
		PRINT_ERROR("LIBBMDPKCS12ERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(input->buf==NULL)
	{
		PRINT_DEBUG("LIBBMDPKCS12ERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(out_bio==NULL)
	{
		PRINT_DEBUG("LIBBMDPKCS12ERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
	if(input->size<=0)
	{
		PRINT_DEBUG("LIBBMDPKCS12ERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}

	nwrite=BIO_write(out_bio, input->buf, input->size);
	if(nwrite!=input->size)
	{
		PRINT_DEBUG("LIBBMDPKCS12ERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
	}

	return BMD_OK;
}


/**
funkcja _bmd_parse_pkcs12_safebags() dla podanego stosu sefabag'ow oraz hasla, na wyjsciu
daje stos odnalezionych certyfikatow oraz odnaleziony klucz prywatny
(w safebagach moze nie byc certyfikatow lub klucza prywatnego)

@param pkcs12_bags (parametr wejsciowy) to wskaznik na stos safebag'ow (openSSL oraz PKCS#12)
@param pass (parametr wejsciowy) to lancuch znakowy zawierajacy haslo
@param passlen (parametr wejsciowy) to dlugosc hasla pass
@param certs_from_safebag (parametr wyjsciowy) to adres wskaznika na tworzony stos z odnalezionymi (zaalokowanymi) certyfikatami
@param pkey_from_safebag (parametr wyjsciowy) to adres wskaznika na odnaleziony (zaalokowany) klucz prywatny (moze byc na wyjsciu NULL)

@retval BMD_OK
@retval lub odpowiedni kod bledu

*/
long _bmd_parse_pkcs12_safebags(	STACK_OF(PKCS12_SAFEBAG) *pkcs12_bags,
						char *pass,
						long passlen,
			       		STACK_OF(X509) **certs_from_safebag,
						EVP_PKEY **pkey_from_safebag)
{
PKCS12_SAFEBAG *pkcs12_bag=NULL;
X509 *cert=NULL;
PKCS8_PRIV_KEY_INFO *p8=NULL;
long i=0;

 	if(pkcs12_bags==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(certs_from_safebag==NULL)		{	BMD_FOK(BMD_ERR_PARAM4);	}
	if((*certs_from_safebag)!=NULL)	{	BMD_FOK(BMD_ERR_PARAM4);	}
	if(pkey_from_safebag==NULL)		{	BMD_FOK(BMD_ERR_PARAM5);	}
	if((*pkey_from_safebag)!=NULL)	{	BMD_FOK(BMD_ERR_PARAM5);	}
	if(passlen > 0 && pass == NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}

	(*certs_from_safebag)=sk_X509_new_null();
	if( (*certs_from_safebag) == NULL )	{	BMD_FOK(BMD_ERR_MEMORY);	}

	for(i=0; i<sk_PKCS12_SAFEBAG_num(pkcs12_bags); i++)
	{
		pkcs12_bag=sk_PKCS12_SAFEBAG_value(pkcs12_bags,i);
		switch(M_PKCS12_bag_type(pkcs12_bag))
		{
			case NID_keyBag:
			{
				(*pkey_from_safebag)=EVP_PKCS82PKEY(pkcs12_bag->value.keybag);
				break;
			}
			case NID_pkcs8ShroudedKeyBag:
			{
				p8 = PKCS12_decrypt_skey(pkcs12_bag, pass, passlen);
				if(p8==NULL)
				{
					BMD_FOK(BMD_ERR_MEMORY);
				}
				*pkey_from_safebag = EVP_PKCS82PKEY(p8);
				break;
			}
			case NID_certBag:
			{
				if (M_PKCS12_cert_bag_type(pkcs12_bag)==NID_x509Certificate)
				{
					cert=M_PKCS12_certbag2x509(pkcs12_bag);
					sk_X509_push(*certs_from_safebag,cert);
				}
				break;
			}
			default:
			{
				BMD_FOK(BMD_ERR_UNIMPLEMENTED); /*added by WSZ*/
			}
		}
	}

	return BMD_OK;
}


/**
funkcja __file_to_P12() laduje plik PFX do openSSL'owej struktury PKCS12

@param filename to lancuch znakow z nazwa wczytywanego pliku pfx
@param p12 to adres wskaznika na tworzana strukture PKCS12

@retval BMD_OK
@retval lub odpowiedni kod bledu

*/
long __file_to_P12(char *filename,PKCS12 **p12)
{
GenBuf_t *buf=NULL;
BIO *bp=NULL;

#ifdef WIN32
	long retVal=0;
	wchar_t *wideFilename=NULL;
#endif

	if(filename==NULL)
	{
		PRINT_DEBUG("LIBBMDPKCS12ERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(p12==NULL)
	{
		PRINT_DEBUG("LIBBMDPKCS12ERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
	if((*p12)!=NULL)
	{
		PRINT_DEBUG("LIBBMDPKCS12ERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

#ifdef WIN32
	UTF8StringToWindowsString(filename, &wideFilename, NULL);
	retVal=bmd_load_binary_content_wide(wideFilename, &buf);
	free(wideFilename); wideFilename=NULL;
	if(retVal != 0)
	{
		return BMD_ERR_DISK;
	}
#else
	BMD_FOK(bmd_load_binary_content(filename,&buf));
#endif

	bp=BIO_new(BIO_s_mem());
	if(bp==NULL)
	{
		free_gen_buf(&buf);
		PRINT_DEBUG("LIBBMDPKCS12ERR Memory error. Error=%i\n",BMD_ERR_MEMORY);
		return BMD_ERR_MEMORY;
	}

	BMD_FOK(_bmd_binary_content_to_BIO(buf,bp));
	free_gen_buf(&buf);

	(*p12)=d2i_PKCS12_bio(bp,NULL);
	if( (*p12) == NULL )
	{
		BIO_free(bp);
		bp=NULL;
		PRINT_DEBUG("LIBBMDPKCS12ERR Format error. Error=%i\n",BMD_ERR_FORMAT);
		return BMD_ERR_FORMAT;
	}

	BIO_free(bp);
	bp=NULL;

	return BMD_OK;
}



/**
funkcja __mime_to_P12() laduje zawartosc pliku PFX (wczesniej zaladowana do pamieci) do openSSL'owej struktury PKCS12

@param buf to bufor generyczny z zawartoscia pliku pfx
@param p12 to adres wskaznika na tworzana strukture PKCS12

@retval BMD_OK
@retval lub odpowiedni kod bledu

*/
long __mem_to_P12(GenBuf_t *file_buf, PKCS12 **p12)
{

	BIO *bp=NULL;

	if(file_buf==NULL)
	{
		PRINT_DEBUG("LIBBMDPKCS12ERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(file_buf->buf == NULL)
	{
		PRINT_DEBUG("LIBBMDPKCS12ERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(p12==NULL)
	{
		PRINT_DEBUG("LIBBMDPKCS12ERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
	if((*p12)!=NULL)
	{
		PRINT_DEBUG("LIBBMDPKCS12ERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}



	bp=BIO_new(BIO_s_mem());
	if(bp==NULL)
	{
		PRINT_DEBUG("LIBBMDPKCS12ERR Memory error. Error=%i\n",BMD_ERR_MEMORY);
		return BMD_ERR_MEMORY;
	}

	BMD_FOK(_bmd_binary_content_to_BIO(file_buf,bp));

	(*p12)=d2i_PKCS12_bio(bp,NULL);
	if( (*p12) == NULL )
	{
		BIO_free(bp);
		bp=NULL;
		PRINT_DEBUG("LIBBMDPKCS12ERR Format error. Error=%i\n",BMD_ERR_FORMAT);
		return BMD_ERR_FORMAT;
	}

	BIO_free(bp);
	bp=NULL;

	return BMD_OK;
}






/**
funkcja __is_mac_present() sprawdza, czy pfx wymaga podania hasla.
Jesli haslo jest wymagane, to present ustawiane jest na 1, w przeciwnym razie na 0;

@param p12 (parametr wejsciowy) to wskaznik na strukture z PKCS12 z zawartoscia wczytanego PFX
@param pass (parametr wejsciowy) to lancuch znakowy z haslem
@param passlen (parametr wejsciowy) to dlugosc hasla pass
@param present (parametr wyjsciowy) to wartosc long pod ktora zapisane jest, czy wymagane jest podanie hasla

@retval BMD_OK
@retval lub odpowiedni kod bledu
*/
long __is_mac_present(PKCS12 *p12,char *pass,long passlen,long *present)
{
	if(p12==NULL)
	{
		PRINT_DEBUG("LIBBMDPKCS12ERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(present==NULL)
	{
		PRINT_DEBUG("LIBBMDPKCS12ERR Invalid fourth parameter value. Error=%i\n",BMD_ERR_PARAM4);
		return BMD_ERR_PARAM4;
	}
	if(passlen > 0 && pass == NULL)
	{
		PRINT_DEBUG("LIBBMDPKCS12ERR Invalid third parameter value. Error=%i\n",BMD_ERR_PARAM3);
		return BMD_ERR_PARAM3;
	}

	if(PKCS12_mac_present(p12)) /*jest chronione haslem*/
	{
		/*
		if(!PKCS12_verify_mac(p12, NULL, 0)) // sprawdzenie braku hasla
		{
			//PKCS12_verify_mac(p12, "", 0) //ponoc NULL i "" to cos innego dla PKCS#12 (by WSZ)
			if(!PKCS12_verify_mac(p12,pass,passlen))
				{ return BMD_ERR_CREDENTIALS; }
		}
		*/
		*present=1;
	}
	else /* nie jest chronione haslem*/
	{
		*present=0;
	}

	return BMD_OK;
}


/**
funkcja __authsafe_to_pkcs12_safebags_stack z koperty CMS (PKCS7) tworzy stos safebag'ow

@param authsafe (parametr wejsciowy) to wkaznik na strukture PKCS7 (koperta CMS) z pliku pfx
@param pass (parametr wejsciowy) to lancuch znakowy z haslem
@param passlen (parametr wejsciowy) to dlugosc hasla
@param flaga_pfx_z_haslem (parametr wejsciowy) czy dostep do PKCS12 wymaga hasla (1 tak, 0 nie)
@param pkcs12_bags (parametr wyjsciowy) to adres wskaznika na tworzony stos safebag'ow

@retval BMD_OK
@retval lub odpowiedni kod bledu
*/
long __authsafe_to_pkcs12_safebags_stack(	PKCS7 *authsafe,
							char *pass,
							long passlen,
							long flaga_pfx_z_haslem,
							STACK_OF(PKCS12_SAFEBAG) **pkcs12_bags)
{
	if(authsafe==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(pkcs12_bags==NULL)			{	BMD_FOK(BMD_ERR_PARAM5);	}
	if((*pkcs12_bags)!=NULL)		{	BMD_FOK(BMD_ERR_PARAM5);	}
	if(passlen > 0 && pass == NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}

	switch(M_PKCS12_bag_type(authsafe))
	{

		case NID_pkcs7_data:
		{

			(*pkcs12_bags)=M_PKCS12_unpack_p7data(authsafe);
			if((*pkcs12_bags)==NULL)
			{
				BMD_FOK(BMD_ERR_OP_FAILED);
			}

			break;
		}
		case NID_pkcs7_encrypted:
		{

			if(flaga_pfx_z_haslem==1)
			{
				(*pkcs12_bags)=M_PKCS12_unpack_p7encdata(authsafe,pass,passlen);

/*				if((*pkcs12_bags)==NULL)
				{
					BMD_FOK(BMD_ERR_OP_FAILED);
				}*/
			}
			else
			{

				(*pkcs12_bags)=M_PKCS12_unpack_p7encdata(authsafe,NULL,0);

/*				if((*pkcs12_bags)==NULL)
				{
					BMD_FOK(BMD_ERR_OP_FAILED);
				}*/
			}

			break;
		}
		default:
		{
			BMD_FOK(BMD_ERR_UNIMPLEMENTED);
		}

	}

	return BMD_OK;
}

/**
funkcja _X509_to_genbuf() wczytuje certyfikat do bufora

@param cert (parametr wejsciowy) to wskaznik na certyfikat X509
@param buf (parametr wyjsciowy) to adres wskaznika na tworzony bufor

@retval BMD_OK
@retval lub odpowiedni kod bledu
*/
long _X509_to_genbuf(X509 *cert, GenBuf_t **buf)
{
unsigned char *tmp=NULL;
long length=0;

	if(cert==NULL)
	{
		PRINT_DEBUG("LIBBMDPKCS12ERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(buf==NULL)
	{
		PRINT_DEBUG("LIBBMDPKCS12ERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
	if((*buf)!=NULL)
	{
		PRINT_DEBUG("LIBBMDPKCS12ERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	length=i2d_X509(cert,&tmp);
	if(length<0)
	{
		PRINT_DEBUG("LIBBMDPKCS12ERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
	}

	BMD_FOK(set_gen_buf2((char*)tmp,length,buf));

	OPENSSL_free(tmp);
	tmp=NULL;

	return BMD_OK;
}

/**
funkcja bmd_check_p12_pass() sprawdza dla pfx przechowywanego w przekazanym genbufie,
czy wymagane jest podanie hasla

@param p12_buf (parametr wejsciowy) to wskaznik na genbuf z zawartoscia PFX
@param flag (parametr wyjsciowy) to adres zmiennej, pod ktora zostanie zapisane, czy wymagane jest haslo (1 na tak, 0 na nie)

@retval BMD_OK
@retval lub odpowiedni kod bledu
*/
long bmd_check_p12_pass(GenBuf_t *p12_buf,long *flag)
{
PKCS12 *p12=NULL;
long pytac_o_haslo=1;
BIO *bp=NULL;

	if(p12_buf==NULL)
	{
		PRINT_DEBUG("LIBBMDPKCS12ERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(p12_buf->buf==NULL)
	{
		PRINT_DEBUG("LIBBMDPKCS12ERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(p12_buf->size<0)
	{
		PRINT_DEBUG("LIBBMDPKCS12ERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}

	if(flag==NULL)
	{
		PRINT_DEBUG("LIBBMDPKCS12ERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	bp=BIO_new(BIO_s_mem());
	BMD_FOK(_bmd_binary_content_to_BIO(p12_buf,bp));

	p12=d2i_PKCS12_bio(bp,NULL);
	BIO_free(bp); bp=NULL;
	if(p12==NULL)
	{
		PRINT_DEBUG("LIBBMDPKCS12ERR Format error. Error=%i\n",BMD_ERR_FORMAT);
		return BMD_ERR_FORMAT;
	}

	if(PKCS12_verify_mac(p12, NULL, 0))
	{
		pytac_o_haslo=0;
	}
	PKCS12_free(p12); p12=NULL;

	*flag=pytac_o_haslo;
	return BMD_OK;
}

/**
funkcja wydobywa certyfikat, klucz publiczny i klucz prywatny z openSSL'owej struktury PKCS12
*/
long _bmd_parse_p12(PKCS12 *p12, char *pass,long passlen,RSA **publicKey,RSA **privateKey,GenBuf_t **cert)
{
STACK_OF(PKCS7) *authsafes=NULL;
STACK_OF(PKCS12_SAFEBAG) *pkcs12_bags=NULL;
STACK_OF(X509) *certs_from_safebag=NULL;
long flaga_pfx_z_haslem=0, i=0;
long flag_cert_found=0,flag_key_found=0;
X509 *tmp_cert=NULL;
EVP_PKEY *temp_pub_key=NULL;
EVP_PKEY *temp_priv_key=NULL;


long authsafesCount=0;
long certsCount=0;
long iter=0;
long retVal=0;
BASIC_CONSTRAINTS *basicConstraints=NULL;

	if( p12 == NULL )
	{
		PRINT_DEBUG("LIBBMDPKCS12ERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	
	if( ( passlen > 0 ) && ( pass == NULL ) )
	{
		PRINT_DEBUG("LIBBMDPKCS12ERR Invalid third parameter value. Error=%i\n",BMD_ERR_PARAM3);
		return BMD_ERR_PARAM3;
	}
	if( passlen < 0 )
	{
		PRINT_DEBUG("LIBBMDPKCS12ERR Invalid third parameter value. Error=%i\n",BMD_ERR_PARAM3);
		return BMD_ERR_PARAM3;
	}
	if( (passlen == 0 ) && ( pass != NULL ) )
	{
		PRINT_DEBUG("LIBBMDPKCS12ERR Invalid third parameter value. Error=%i\n",BMD_ERR_PARAM3);
		return BMD_ERR_PARAM3;
	}

	if(publicKey==NULL)
	{
		PRINT_DEBUG("LIBBMDPKCS12ERR Invalid fourth parameter value. Error=%i\n",BMD_ERR_PARAM4);
		return BMD_ERR_PARAM4;
	}
	if((*publicKey!=NULL))
	{
		PRINT_DEBUG("LIBBMDPKCS12ERR Invalid fourth parameter value. Error=%i\n",BMD_ERR_PARAM4);
		return BMD_ERR_PARAM4;
	}

	if(privateKey==NULL)
	{
		PRINT_DEBUG("LIBBMDPKCS12ERR Invalid fifth parameter value. Error=%i\n",BMD_ERR_PARAM5);
		return BMD_ERR_PARAM5;
	}
	if((*privateKey)!=NULL)
	{
		PRINT_DEBUG("LIBBMDPKCS12ERR Invalid fifth parameter value. Error=%i\n",BMD_ERR_PARAM5);
		return BMD_ERR_PARAM5;
	}

	if(cert==NULL)
	{
		PRINT_DEBUG("LIBBMDPKCS12ERR Invalid sixth parameter value. Error=%i\n",BMD_ERR_PARAM6);
		return BMD_ERR_PARAM6;
	}
	if((*cert)!=NULL)
	{
		PRINT_DEBUG("LIBBMDPKCS12ERR Invalid sixth parameter value. Error=%i\n",BMD_ERR_PARAM6);
		return BMD_ERR_PARAM6;
	}

	
	/* czy jest MAC */
	BMD_FOK(__is_mac_present(p12,pass,passlen,&flaga_pfx_z_haslem));

	authsafes=M_PKCS12_unpack_authsafes(p12);
	
	if(!authsafes)
	{
		PRINT_DEBUG("LIBBMDPKCS12ERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
	}

	/* poszukaj certyfikatu uzytkownika koncowego oraz korespondujacego klucza */
	authsafesCount=sk_PKCS7_num(authsafes);
	for(i=0; i<authsafesCount; i++)
	{
		BMD_FOK(__authsafe_to_pkcs12_safebags_stack(sk_PKCS7_value(authsafes,i),pass,passlen,flaga_pfx_z_haslem,
							&pkcs12_bags));
		if(flaga_pfx_z_haslem==1)
		{
			_bmd_parse_pkcs12_safebags(pkcs12_bags,pass,passlen,&certs_from_safebag,&temp_priv_key);
		}
		else /*roznica miedzy "" a NULL*/
		{
			_bmd_parse_pkcs12_safebags(pkcs12_bags,NULL,0,&certs_from_safebag,&temp_priv_key);
		}

		if(temp_priv_key)
		{
			(*privateKey)=RSAPrivateKey_dup(EVP_PKEY_get1_RSA(temp_priv_key));
			EVP_PKEY_free(temp_priv_key); temp_priv_key=NULL;
			if(*privateKey == NULL)
			{
				RSA_free(*publicKey); *publicKey=NULL;
				free_gen_buf(cert);
				sk_X509_zero(certs_from_safebag);certs_from_safebag=NULL;
				sk_PKCS12_SAFEBAG_zero(pkcs12_bags);pkcs12_bags=NULL;
				sk_PKCS7_zero(authsafes); authsafes=NULL;
				PRINT_DEBUG("LIBBMDPKCS12ERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
				return BMD_ERR_OP_FAILED;
			}
			flag_key_found=1;
		}

		
		certsCount=sk_X509_num(certs_from_safebag);
		if(certsCount == 0)
		{
			sk_X509_zero(certs_from_safebag);certs_from_safebag=NULL;
			sk_PKCS12_SAFEBAG_zero(pkcs12_bags);pkcs12_bags=NULL;
			continue;
		}
		else if(certsCount == 1)
		{
			//jesli znalazl tylko jeden certyfikat,
			//to traktuje go bez sprawdzania, jako certyfikat jednostki koncowej
			tmp_cert=sk_X509_value(certs_from_safebag, 0);
		}
		else
		{
			//wyszukiwanie certyfikatu jednostki koncowej
			for(iter=0; iter<certsCount; iter++)
			{
				tmp_cert=sk_X509_value(certs_from_safebag, iter);
				retVal=X509_get_ext_by_NID(tmp_cert, NID_basic_constraints, -1);
				//jesli nie znajdzie basicConstraints to znaczy, ze certyfikat jednostki koncowej
				if(retVal < 0)
				{
					break;
				}
				basicConstraints=(BASIC_CONSTRAINTS*)X509_get_ext_d2i(tmp_cert, NID_basic_constraints, NULL/*&critical*/, NULL/*indeks*/);
				if(basicConstraints == NULL)
				{
					RSA_free(*privateKey); *privateKey=NULL;
					X509_free(tmp_cert); tmp_cert=NULL;
					sk_X509_zero(certs_from_safebag);certs_from_safebag=NULL;
					sk_PKCS12_SAFEBAG_zero(pkcs12_bags);pkcs12_bags=NULL;
					sk_PKCS7_zero(authsafes); authsafes=NULL;
					PRINT_DEBUG("LIBBMDPKCS12ERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
					return BMD_ERR_OP_FAILED;
				}
				//jesli certyfikat jednostki koncowej
				if(basicConstraints->ca == 0)
				{
					BASIC_CONSTRAINTS_free(basicConstraints); basicConstraints=NULL;
					break;
				}

				BASIC_CONSTRAINTS_free(basicConstraints); basicConstraints=NULL;
				X509_free(tmp_cert); tmp_cert=NULL;
			}
		}

		_X509_to_genbuf(tmp_cert,cert);
		temp_pub_key=X509_get_pubkey(tmp_cert);
		X509_free(tmp_cert); tmp_cert=NULL;
		if(temp_pub_key == NULL)
		{
			RSA_free(*privateKey); *privateKey=NULL;
			free_gen_buf(cert);
			sk_X509_zero(certs_from_safebag);certs_from_safebag=NULL;
			sk_PKCS12_SAFEBAG_zero(pkcs12_bags);pkcs12_bags=NULL;
			sk_PKCS7_zero(authsafes); authsafes=NULL;
			PRINT_DEBUG("LIBBMDPKCS12ERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
			return BMD_ERR_OP_FAILED;
		}
		(*publicKey)=RSAPublicKey_dup(EVP_PKEY_get1_RSA(temp_pub_key)); //zle!!!
		EVP_PKEY_free(temp_pub_key);temp_pub_key=NULL;
		if(*publicKey==NULL || *cert==NULL)
		{
			RSA_free(*privateKey); *privateKey=NULL;
			RSA_free(*publicKey); *publicKey=NULL;
			free_gen_buf(cert);
			sk_X509_zero(certs_from_safebag);certs_from_safebag=NULL;
			sk_PKCS12_SAFEBAG_zero(pkcs12_bags);pkcs12_bags=NULL;
			sk_PKCS7_zero(authsafes); authsafes=NULL;
			PRINT_DEBUG("LIBBMDPKCS12ERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
			return BMD_ERR_OP_FAILED;
		}
		flag_cert_found=1;
		

		sk_X509_zero(certs_from_safebag);certs_from_safebag=NULL;
		sk_PKCS12_SAFEBAG_zero(pkcs12_bags);pkcs12_bags=NULL;

		if( (flag_key_found==1) && (flag_cert_found==1) )
		{
			break;
		}
	}

	sk_PKCS7_zero(authsafes); authsafes=NULL;


	if( flag_key_found==0 )
	{
		RSA_free(*privateKey); *privateKey=NULL;
		RSA_free(*publicKey); *publicKey=NULL;
		free_gen_buf(cert);
		PRINT_DEBUG("LIBBMDPKCS12ERR Format error. Error=%i\n",BMD_ERR_FORMAT);
		return BMD_ERR_FORMAT;
	}
	if( flag_cert_found==0 )
	{
		RSA_free(*privateKey); *privateKey=NULL;
		RSA_free(*publicKey); *publicKey=NULL;
		free_gen_buf(cert);
		PRINT_DEBUG("LIBBMDPKCS12ERR Unable to find end entity certificate). Error=%i\n", ERR_PKCS12_NO_ENDENTITY_CERT);
		return ERR_PKCS12_NO_ENDENTITY_CERT;
	}

	return BMD_OK;
}


/**
funkcja bmd_parse_p12_file() z pliku PFX wyciaga klucz prywatny , klucz publiczny oraz certyfikat klucza publicznego

@param filename (parametr wejsciowy) to lancuch znakowy z nazwa wczytywanego pliku pfx
@param pass (parametr wejsciowy) to lancuch znakowy z haslem do pliku pfx
@param passlen (parametr wejsciowy) to dlugosc hasla
@param publicKey (parametr wyjsciowy) to adres wskaznika na wyciagniety klucz publiczny
@param privateKey (parametr wyjsciowy) to adres wskaznika na wyciagany klucz prywatny
@param cert (parametr wyjsciowy) to adres wskaznika na wyciagniety certyfikat

@retval BMD_OK
@retval lub odpowiedni kod bledu
*/
long bmd_parse_p12_file(char *filename,char *pass,long passlen,RSA **publicKey,RSA **privateKey,GenBuf_t **cert)
{
long retVal=0;
PKCS12 *p12=NULL;

	if( filename == NULL )				{	BMD_FOK(BMD_ERR_PARAM1);	}
	if( ( passlen > 0 ) && ( pass == NULL ) )	{	BMD_FOK(BMD_ERR_PARAM3);	}
	if( passlen < 0 )					{	BMD_FOK(BMD_ERR_PARAM3);	}
	if( (passlen == 0 ) && ( pass != NULL ) )	{	BMD_FOK(BMD_ERR_PARAM3);	}
	if(publicKey==NULL)				{	BMD_FOK(BMD_ERR_PARAM4);	}
	if((*publicKey!=NULL))				{	BMD_FOK(BMD_ERR_PARAM4);	}
	if(privateKey==NULL)				{	BMD_FOK(BMD_ERR_PARAM5);	}
	if((*privateKey)!=NULL)				{	BMD_FOK(BMD_ERR_PARAM5);	}
	if(cert==NULL)					{	BMD_FOK(BMD_ERR_PARAM6);	}
	if((*cert)!=NULL)					{	BMD_FOK(BMD_ERR_PARAM6);	}

	/* plik --> P12 */
	BMD_FOK(__file_to_P12(filename,&p12));

	retVal=_bmd_parse_p12(p12, pass, passlen, publicKey, privateKey, cert);
	PKCS12_free(p12); p12=NULL;
	
	BMD_FOK(retVal);
	
	return BMD_OK;
}

/**
funkcja bmd_parse_p12_fileInMem() z pliku PFX zaladowanego do pamieci,
wyciaga klucz prywatny , klucz publiczny oraz certyfikat klucza publicznego

@param file_buf (parametr wejsciowy) to bufor generyczny z zawartoscia wczytanego pliku pfx
@param pass (parametr wejsciowy) to lancuch znakowy z haslem do pliku pfx
@param passlen (parametr wejsciowy) to dlugosc hasla
@param publicKey (parametr wyjsciowy) to adres wskaznika na wyciagniety klucz publiczny
@param privateKey (parametr wyjsciowy) to adres wskaznika na wyciagany klucz prywatny
@param cert (parametr wyjsciowy) to adres wskaznika na wyciagniety certyfikat

@retval BMD_OK
@retval lub odpowiedni kod bledu
*/
long bmd_parse_p12_fileInMem(GenBuf_t *file_buf,char *pass,long passlen,RSA **publicKey,RSA **privateKey,GenBuf_t **cert)
{
PKCS12 *p12=NULL;
long retVal=0;

	if( file_buf == NULL )
	{
		PRINT_DEBUG("LIBBMDPKCS12ERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if( file_buf->buf == NULL )
	{
		PRINT_DEBUG("LIBBMDPKCS12ERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}

	if( ( passlen > 0 ) && ( pass == NULL ) )
	{
		PRINT_DEBUG("LIBBMDPKCS12ERR Invalid third parameter value. Error=%i\n",BMD_ERR_PARAM3);
		return BMD_ERR_PARAM3;
	}
	if( passlen < 0 )
	{
		PRINT_DEBUG("LIBBMDPKCS12ERR Invalid third parameter value. Error=%i\n",BMD_ERR_PARAM3);
		return BMD_ERR_PARAM3;
	}
	if( (passlen == 0 ) && ( pass != NULL ) )
	{
		PRINT_DEBUG("LIBBMDPKCS12ERR Invalid third parameter value. Error=%i\n",BMD_ERR_PARAM3);
		return BMD_ERR_PARAM3;
	}

	if(publicKey==NULL)
	{
		PRINT_DEBUG("LIBBMDPKCS12ERR Invalid fourth parameter value. Error=%i\n",BMD_ERR_PARAM4);
		return BMD_ERR_PARAM4;
	}
	if((*publicKey!=NULL))
	{
		PRINT_DEBUG("LIBBMDPKCS12ERR Invalid fourth parameter value. Error=%i\n",BMD_ERR_PARAM4);
		return BMD_ERR_PARAM4;
	}

	if(privateKey==NULL)
	{
		PRINT_DEBUG("LIBBMDPKCS12ERR Invalid fifth parameter value. Error=%i\n",BMD_ERR_PARAM5);
		return BMD_ERR_PARAM5;
	}
	if((*privateKey)!=NULL)
	{
		PRINT_DEBUG("LIBBMDPKCS12ERR Invalid fifth parameter value. Error=%i\n",BMD_ERR_PARAM5);
		return BMD_ERR_PARAM5;
	}

	if(cert==NULL)
	{
		PRINT_DEBUG("LIBBMDPKCS12ERR Invalid sixth parameter value. Error=%i\n",BMD_ERR_PARAM6);
		return BMD_ERR_PARAM6;
	}
	if((*cert)!=NULL)
	{
		PRINT_DEBUG("LIBBMDPKCS12ERR Invalid sixth parameter value. Error=%i\n",BMD_ERR_PARAM6);
		return BMD_ERR_PARAM6;
	}

	/* plik --> P12 */
	BMD_FOK(__mem_to_P12(file_buf, &p12));

	retVal=_bmd_parse_p12(p12, pass, passlen, publicKey, privateKey, cert);
	PKCS12_free(p12); p12=NULL;

	BMD_FOK(retVal);

	return BMD_OK;
}

