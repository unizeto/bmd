#include <bmd/bmd_ks/bmd_ks.h>

#ifdef WIN32
#pragma warning(disable:4221)
#pragma warning(disable:4204)
#pragma warning(disable:4100)
#endif

extern svc_conf_t *svc_conf;

/* konwertuje lanuch SVC_CKA_ID_LENGTH bajtow (2*SVC_CKA_ID_LENGTH znakow) zapisanych jako %02X 
   na ciag char * o danej dlugosci 
 */
long __svc_get_cka_id(const char *input,char **buf,long *length,char **error)
{
    long i;
    char d[3];
    
    if( input == NULL )
        return SVC_ERROR_PARAM;
    if( buf == NULL )
        return SVC_ERROR_PARAM;
    if( (*buf) != NULL )
        return SVC_ERROR_PARAM;
    if( length == NULL )
        return SVC_ERROR_PARAM;
    
    if( strlen(input) != 2*SVC_CKA_ID_LENGTH )
	{
		svc_gen_err_desc(error,"CKA_ID has wrong length == %i",strlen(input));
        	return SVC_ERROR_FORMAT;       
	}

    (*buf)=(char *)malloc(SVC_CKA_ID_LENGTH);
    memset(*buf,0,SVC_CKA_ID_LENGTH);
    *length=SVC_CKA_ID_LENGTH;
    
    for(i=0;i<SVC_CKA_ID_LENGTH;i++)
    {
       memset(d,0,3);
       memmove(d,input+2*i,2);
       (*buf)[i]=(char)strtol(d,NULL,16);  
    }
    return SVC_OK;    
}

/* nawiazuje polaczenie z czytnikiem i loguje sie do sesji */
long __svc_connect_slot(const char *pin,long slot_nr,CK_SLOT_ID_PTR slots,CK_FUNCTION_LIST_PTR pFunctionList,
                       CK_SESSION_HANDLE *hSession,char **error)
{
    CK_RV rv;
    
    if( slot_nr < 0 )
        return SVC_ERROR_PARAM;
    if( pFunctionList == NULL )
        return SVC_ERROR_PARAM;
    if( hSession == NULL )
        return SVC_ERROR_PARAM;
    
	print_debug("Proba otwarcia sesji ze slotem numer: %i\n", slot_nr);
    rv=pFunctionList->C_OpenSession(slots[slot_nr],CKF_SERIAL_SESSION,NULL,NULL,hSession);
    if( rv != CKR_OK )
	{
		print_debug("Nie udalo sie otwarcia sesji ze slotem numer: %i, pkcs11 status %X\n", slot_nr,rv);
		svc_gen_err_desc(error,"C_OpenSession failed with code %X",rv);
        return SVC_ERROR_FAILED;
	}
	print_debug("Udalo sie otwarcia sesji ze slotem numer: %i\n", slot_nr);

	print_debug("Proba zalogowania sie do slotu numer: %i\n", slot_nr);
    rv=pFunctionList->C_Login(*hSession,CKU_USER,(CK_UTF8CHAR_PTR)pin,(CK_ULONG)strlen(pin));
    if( ( rv != CKR_OK ) && ( rv != CKR_USER_ALREADY_LOGGED_IN ) )
	{
		print_debug("Nie udalo sie zalogowac do slotu numer: %i, pkcs11 status %X\n", slot_nr,rv);
		svc_gen_err_desc(error,"C_Login failed with code %X",rv);
        return SVC_ERROR_FAILED;
	}
	print_debug("Udalo sie zalogowac do slotu numer: %i\n", slot_nr);

    return SVC_OK;
}

long __svc_get_certificate_by_cka_id(const char *cka_id,CK_FUNCTION_LIST_PTR pFunctionList,CK_SESSION_HANDLE hSession,
									char **cert, long *cert_length,char **error)
{
	long status;
	char *cka_buf=NULL;
	long cka_length;
	CK_OBJECT_CLASS CertificateClass=CKO_CERTIFICATE;
	CK_OBJECT_HANDLE hCerts[2];
	CK_BYTE myTrue=TRUE;
	CK_ATTRIBUTE certTemplate[]={
		{CKA_CLASS,&CertificateClass,sizeof(CertificateClass)},
		{CKA_TOKEN,&myTrue,sizeof(myTrue)},
		{CKA_ID,NULL_PTR,0}
	};
	CK_ATTRIBUTE certValueTemplate[]={
		{CKA_CLASS,&CertificateClass,sizeof(CertificateClass)},
		{CKA_TOKEN,&myTrue,sizeof(myTrue)},
		{CKA_VALUE,NULL_PTR,0}
	};
	CK_ULONG count=0;
	CK_RV rv;

	if( cka_id == NULL )
		return SVC_ERROR_PARAM;
	if( pFunctionList == NULL )
		return SVC_ERROR_PARAM;
	if( cert == NULL )
		return SVC_ERROR_PARAM;
	if( (*cert) != NULL )
		return SVC_ERROR_PARAM;
	if( cert_length == NULL )
		return SVC_ERROR_PARAM;

	status=__svc_get_cka_id(cka_id,&cka_buf,&cka_length,error);
	if( status != SVC_OK )
		return status;

	certTemplate[2].ulValueLen=cka_length;
	certTemplate[2].pValue=cka_buf;
	rv=pFunctionList->C_FindObjectsInit(hSession,certTemplate,3);
	if( rv != CKR_OK )
	{
		svc_gen_err_desc(error,"C_FindObjectsInit failed while searching certs with code %X",rv);
		return SVC_ERROR_FAILED;
	}
    rv=pFunctionList->C_FindObjects(hSession,hCerts,2,&count);
    if( rv != CKR_OK )
    {
		svc_gen_err_desc(error,"C_FindObjects failed while searching certs with code %X",rv);
		rv=pFunctionList->C_FindObjectsFinal(hSession);
		return SVC_ERROR_FAILED;
    }
    if( count == 0 )
    {
		rv=pFunctionList->C_FindObjectsFinal(hSession);
		svc_gen_err_desc(error,"Could not find certificate with desired CKA_ID value");
		return SVC_ERROR_NODATA;
    }   
    rv=pFunctionList->C_FindObjectsFinal(hSession);

	rv=pFunctionList->C_GetAttributeValue(hSession,hCerts[0],certValueTemplate,3);
	if(rv==CKR_OK)
	{
		long length;
        char *value=NULL;
                
		length=certValueTemplate[2].ulValueLen;
        value=(char *)malloc(length);
		certValueTemplate[2].ulValueLen=length;
        certValueTemplate[2].pValue=value;
		rv=pFunctionList->C_GetAttributeValue(hSession,hCerts[0],certValueTemplate,3);
		if( rv == CKR_OK )
		{
			(*cert)=(char *)malloc(length);
			memmove(*cert,certValueTemplate[2].pValue,length);
			*cert_length=length;
			free0(value);
		}
		else
		{
			svc_gen_err_desc(error,"C_GetAttributeValue failed while getting Certificate CKA_VALUE %X",rv);
			return SVC_ERROR_FAILED;
		}

	}
	else
	{
		svc_gen_err_desc(error,"C_GetAttributeValue failed while getting Certificate CKA_VALUE %X",rv);
		return SVC_ERROR_FAILED;
	}

	return SVC_OK;
}

/* wyszukuje w danej sesji klucza prywatnego o danym CKA_ID, sesja musi byc uwierzytelniona */
long __svc_find_privkey(const char *cka_id,CK_SESSION_HANDLE hSession,CK_FUNCTION_LIST_PTR pFunctionList,
                       CK_OBJECT_HANDLE *hPrivKey,char **error)
{
	char *buf=NULL;
    long length;
    long status;
    CK_RV rv;
    CK_BYTE myTrue=TRUE;
    CK_ULONG count=0;
    CK_OBJECT_CLASS PrivateKeyClass=CKO_PRIVATE_KEY;
	
    CK_ATTRIBUTE privatekeyTemplate[]={
				{CKA_CLASS,&PrivateKeyClass,sizeof(PrivateKeyClass)},
				{CKA_TOKEN,&myTrue,sizeof(myTrue)},
				{CKA_ID,NULL_PTR,0}
	};

    if( cka_id == NULL )
        return SVC_OK;
    if( pFunctionList == NULL )
        return SVC_ERROR_PARAM;
    if( hPrivKey == NULL )
        return SVC_ERROR_PARAM;
    
    status=__svc_get_cka_id(cka_id,&buf,&length,error);
    print_debug("__svc_get_cka_id status |%i| dla cka_id |%s|\n",status,cka_id);
    if( status != SVC_OK )
        return status;

    privatekeyTemplate[2].ulValueLen=length;
    privatekeyTemplate[2].pValue=buf;
	free0(*error);

	rv=pFunctionList->C_FindObjectsInit(hSession,privatekeyTemplate,3);
	print_debug("Inicjalizacja wyszukiwania klucza prywatnego dla sesji |%i| status |%X|\n",hSession,rv);
	if( rv != CKR_OK )
	{
		svc_gen_err_desc(error,"C_FindObjectsInit failed with code %X",rv);
        return SVC_ERROR_FAILED;
	}

    rv=pFunctionList->C_FindObjects(hSession,hPrivKey,1,&count);
    print_debug("Wyszukiwanie klucza prywatnego dla sesji |%i| status |%X| ilosc obiektow |%i|\n",hSession,rv, count);
    if( rv != CKR_OK )
    {
		svc_gen_err_desc(error,"C_FindObjects failed with code %X",rv);
        rv=pFunctionList->C_FindObjectsFinal(hSession);
        return SVC_ERROR_FAILED;
    }
    if( count == 0 )
    {
        rv=pFunctionList->C_FindObjectsFinal(hSession);
        return SVC_ERROR_NODATA;
    }   
    rv=pFunctionList->C_FindObjectsFinal(hSession);
    
    return SVC_OK;
}

long __svc_get_slot_number(const char *sig_key_id,const char *dec_key_id,const char *pin,
                          CK_SLOT_ID_PTR slots,CK_FUNCTION_LIST_PTR pFunctionList,long *sig_nr,long *dec_nr,
						  char **sig_cert,long *sig_cert_length,
						  char **ed_cert,long *ed_cert_length,char **error)
{
    CK_RV rv;
    CK_SESSION_HANDLE hSession=0;
    CK_OBJECT_HANDLE hSigKey,hDecKey;
    long status,i;
    long sig_found=-1,dec_found=-1;
    
    if( slots == NULL )
        return SVC_ERROR_PARAM;
    if( pFunctionList == NULL )
        return SVC_ERROR_PARAM;
    if( sig_nr == NULL )
        return SVC_ERROR_PARAM;
    if( dec_nr == NULL )
        return SVC_ERROR_PARAM;

    i=0;
    while( slots[i] != 0 )
    {
           status=__svc_connect_slot(pin,i,slots,pFunctionList,&hSession,error);
           print_debug("Polaczenie ze slotem nr |%i| z pin |%s| status |%i| uzyskana sesja |%i|\n",i,pin,status,(int)hSession);
           if( status != SVC_OK )
		   {
				i++;
				continue;
		   }
		   free0(*error);
           if( sig_found == -1 )
           {
               sig_found=__svc_find_privkey(sig_key_id,hSession,pFunctionList,&hSigKey,error);
               print_debug("szukanie klucza prywatnego do podpisu o id |%s| w slocie |%i| dla sesji |%i| status |%i|\n",
                           sig_key_id,i,hSession,sig_found);
           }
		   free0(*error);
           if( sig_found == SVC_OK )
           {
			   /* tutaj pobierz certyfikat */
			   status=__svc_get_certificate_by_cka_id(sig_key_id,pFunctionList,hSession,sig_cert,sig_cert_length,error);
			   print_debug("szukanie certyfikatu(s) o cka_id %s z rezultatem %i\n",sig_key_id,status);
			   if( status != SVC_OK )
				   return status;
               *sig_nr=i; /* zapamietaj numer slotu w ktorym jest klucz do podpisu */
               sig_found++;
           }
           else if( sig_found == (SVC_OK + 1) )
           {
               print_debug("klucz prywatny do podpisu o id |%s| jest juz znaleziony\n",sig_key_id);
           }
		   else
		   {
			   sig_found=-1;
		   }
           if( dec_found == -1 )
           {
               if( dec_key_id != NULL )
               {
                   dec_found=__svc_find_privkey(dec_key_id,hSession,pFunctionList,&hDecKey,error);
                   print_debug("szukanie klucza prywatnego do deszyfrowania o id |%s| w slocie |%i| dla sesji |%i| status |%i|\n",
                                dec_key_id,i,hSession,sig_found);
               }
               else
                   print_debug("identyfikator klucza do deszyfrowania jest nieustawiony\n");
			   free0(*error);
           }
           if( dec_found == SVC_OK )
           {
			   /* tutaj pobierz certyfikat */
			   status=__svc_get_certificate_by_cka_id(dec_key_id,pFunctionList,hSession,ed_cert,ed_cert_length,error);
			   print_debug("szukanie certyfikatu(ed) o cka_id %s z rezultatem %i %i\n",dec_key_id,status,(int)*ed_cert_length);
			   if( status != SVC_OK )
				   return status;
               *dec_nr=i;
               dec_found++;
           }
           else if( dec_found == (SVC_OK+1) )
           {
               print_debug("klucz prywatny do deszyfrowania jest juz znaleziony\n");
           }
		   else
		   {
			   dec_found=-1;
		   }
           if( ( sig_found == SVC_OK+1 ) && ( dec_found == SVC_OK+1 ) )
               break;   
           
           rv=pFunctionList->C_CloseSession(hSession);
           print_debug("Zamknieto sesje |%i| ze statusem |%X|\n",hSession,rv);
           hSigKey=0;hDecKey=0;hSession=0;
           i++;
    }
    if( sig_found != SVC_OK + 1 )
        *sig_nr=-1;
    if( dec_found != SVC_OK + 1 )
        *dec_nr=-1;
    
	return SVC_OK;
    
}

long _svc_pkcs11_get_data(const char *sig_key_id,const char *dec_key_id,const char *pin,svc_p11_t **svc_p11,char **error)
{
    long i,j,max_threads,status;
    char *sig_cert=NULL;
	long sig_cert_length;
	char *ed_cert=NULL;
	long ed_cert_length;

    if( svc_p11 == NULL )
        return SVC_ERROR_PARAM;
    if( (*svc_p11) == NULL )
        return SVC_ERROR_PARAM;
    if( (*svc_p11)->slots == NULL )
        return SVC_ERROR_PARAM;
    
    if( svc_conf == NULL )
        max_threads=5;
    else
        max_threads=svc_conf->max_threads;
    
    i=0;j=0;
    
    (*svc_p11)->hSigSes=(CK_SESSION_HANDLE *)malloc(sizeof(CK_SESSION_HANDLE)*(max_threads+2));
    if( (*svc_p11)->hSigSes == NULL )
        return SVC_ERROR_MEM;
    memset((*svc_p11)->hSigSes,0,sizeof(CK_SESSION_HANDLE)*(max_threads+1));
    
    (*svc_p11)->hSigKey=(CK_OBJECT_HANDLE *)malloc(sizeof(CK_OBJECT_HANDLE)*(max_threads+2));
    if( (*svc_p11)->hSigKey == NULL )
        return SVC_ERROR_MEM;
    memset((*svc_p11)->hSigKey,0,sizeof(CK_OBJECT_HANDLE)*(max_threads+1));
    
    if( dec_key_id != NULL )
    {
        (*svc_p11)->hDecSes=(CK_SESSION_HANDLE *)malloc(sizeof(CK_SESSION_HANDLE)*(max_threads+2));
        if( (*svc_p11)->hDecSes == NULL )
            return SVC_ERROR_MEM;
        memset((*svc_p11)->hDecSes,0,sizeof(CK_SESSION_HANDLE)*(max_threads+1));
        
        (*svc_p11)->hDecKey=(CK_OBJECT_HANDLE *)malloc(sizeof(CK_OBJECT_HANDLE)*(max_threads+2));
        if( (*svc_p11)->hDecKey == NULL )
            return SVC_ERROR_MEM;
        memset((*svc_p11)->hDecKey,0,sizeof(CK_OBJECT_HANDLE)*(max_threads+1));
    }
    
    /* pobranie numeru slotu w ktorym znajduja sie klucze o danych etykietach */
    status=__svc_get_slot_number(sig_key_id,dec_key_id,pin,(*svc_p11)->slots,(*svc_p11)->pFunctionList,
                                 &((*svc_p11)->_sig_slot),&((*svc_p11)->_dec_slot),
								 &sig_cert,&sig_cert_length,
								 &ed_cert,&ed_cert_length,error);
    if(status != SVC_OK )
	{
        return status;
	}

    print_debug("wyszukiwanie numery slotow w ktorym sa klucze status |%i|\n",status);
    print_debug("klucz do podpisu o id |%s| jest w slocie |%i|\n",sig_key_id,(*svc_p11)->_sig_slot);
	
	if( dec_key_id != NULL )
    {
        if( (*svc_p11)->_dec_slot != -1 )
            print_debug("klucz do deszyfrowania o id |%s| jest w slocie |%i|\n",dec_key_id,(*svc_p11)->_dec_slot);
        else
            print_debug("Klucza do deszyfrowania nie znaleziono\n");
    }       
    else
	{
		svc_log(MSG_SVC_WARN,SVC_LOG_WARNING,"",NULL,NULL,0,0);
        print_debug("ID klucza do deszyfrowania jest nieustawione\n");
	}
	if( (*svc_p11)->_sig_slot == SLOT_UNDEFINED )
	{
		if( error )
			if( *error == NULL ) /* nienadpisywanie opisu bledu */
				svc_gen_err_desc(error,"Sig key with specified label not found");
		return SVC_ERROR_FORMAT;
	}
    if( status != SVC_OK )
        return status;
    print_debug("pobieranie |%i| uchwytow do kluczy\n",max_threads);
	(*svc_p11)->sig_cert=(char *)malloc(sig_cert_length);
	memmove((*svc_p11)->sig_cert,sig_cert,sig_cert_length);
	(*svc_p11)->sig_cert_length=sig_cert_length;

	if( ed_cert != NULL )
	{
		if( ed_cert_length > 0 )
		{
			(*svc_p11)->ed_cert=(char *)malloc(ed_cert_length);
			memmove((*svc_p11)->ed_cert,ed_cert,ed_cert_length);
			(*svc_p11)->ed_cert_length=ed_cert_length;
		}
		else
		{
			(*svc_p11)->ed_cert=NULL;
			(*svc_p11)->ed_cert_length=0;
		}	
	}

    for(i=0;i<max_threads;i++)
    {
       /* klucz do podpisu */
       status=__svc_connect_slot(pin,(*svc_p11)->_sig_slot,(*svc_p11)->slots,
		                         (*svc_p11)->pFunctionList,&((*svc_p11)->hSigSes[i]),error);
       print_debug("Podlaczenie do slotu |%i| ze statusem |%i| utworzona sesja do podpisu %i\n",
                   (*svc_p11)->_sig_slot,status,(*svc_p11)->hSigSes[i]);
       if( status != SVC_OK )
           return status;
       status=__svc_find_privkey(sig_key_id,(*svc_p11)->hSigSes[i],
		                         (*svc_p11)->pFunctionList,&((*svc_p11)->hSigKey[i]),error);
       print_debug("wyszukanie klucza do podpisu o id |%s| w slocie |%i| status |%i|\n",
                   sig_key_id,(*svc_p11)->_sig_slot,status);
       if( status != SVC_OK )
           return status;
       /* klucz do deszyfrowania */
       if( dec_key_id != NULL )
       {
           status=__svc_connect_slot(pin,(*svc_p11)->_dec_slot,(*svc_p11)->slots,(*svc_p11)->pFunctionList,
                                     &((*svc_p11)->hDecSes[i]),error);
           print_debug("Podlaczenie do slotu |%i| ze statusem |%i| utworzona sesja do deszyfrowania %i\n",
                   (*svc_p11)->_sig_slot,status,(*svc_p11)->hSigSes[i]);
           if( status != SVC_OK )
               return status;
           status=__svc_find_privkey(dec_key_id,(*svc_p11)->hDecSes[i],
			                         (*svc_p11)->pFunctionList,&((*svc_p11)->hDecKey[i]),error);
           print_debug("wyszukanie klucza do deszyfrowania o id |%s| w slocie |%i| status |%i|\n",
                       sig_key_id,(*svc_p11)->_sig_slot,status);
           if( status != SVC_OK )
               return status;
	   }
    }
    
    return SVC_OK;
}

/* nawiazuje polaczenia ze wszystkimi mozliwymi urzadzeniami, sesje i wyszkuje uchwyty do
   dwoch kluczy prywatnych - do podpisu i szyfrowania
 */
long svc_pkcs11_prepare(const char *sig_key_id,const char *dec_key_id,const char *pin,svc_p11_t **svc_p11,char **error)
{
    CK_RV rv;
    CK_ULONG ulCount=0;
    long status;
    
    /* sig_key_id, enc_key_id, pin moga byc NULL */
    if( svc_p11 == NULL )
        return SVC_ERROR_PARAM;
    if( (*svc_p11) == NULL )
        return SVC_ERROR_PARAM;
            
    /* pobranie listy slotow */
    rv = (*svc_p11)->pFunctionList->C_GetSlotList(TRUE, NULL_PTR, &ulCount);
    if( (rv == CKR_OK) && (ulCount > 0) )
    {
        (*svc_p11)->slots=(CK_SLOT_ID_PTR)malloc(sizeof(CK_SLOT_ID)*(ulCount+2));
        if( (*svc_p11)->slots == NULL )
            return SVC_ERROR_MEM;
		memset((*svc_p11)->slots,0,sizeof(CK_SLOT_ID)*(ulCount+1));
        rv = (*svc_p11)->pFunctionList->C_GetSlotList(TRUE,(*svc_p11)->slots,&ulCount);
        if( rv != CKR_OK )
		{
			svc_gen_err_desc(error,"C_GetSlotList failed with code %X",rv);
            return SVC_ERROR_FAILED;
		}
    }
    else
	{
		if(ulCount == 0 )
			svc_gen_err_desc(error,"C_GetSlotList return zero number of slots");
		else
			svc_gen_err_desc(error,"C_GetSlotList failed with code %X",rv);
        return SVC_ERROR_FAILED;
	}

    status=_svc_pkcs11_get_data(sig_key_id,dec_key_id,pin,svc_p11,error);
    if( status != SVC_OK )
        return status;
    
    return 0;
}

/* inicjalizuje i alokuje strukture pod dane o PKCS#11, inicjalizuje biblioteke PKCS#11 */
long svc_pkcs11_init(const char *pkcs11_lib,svc_p11_t **svc_p11,char **error)
{
    CK_RV rv;
    CK_C_INITIALIZE_ARGS init_args;
    
    if( pkcs11_lib == NULL )
        return SVC_ERROR_PARAM;
    if( svc_p11 == NULL )
        return SVC_ERROR_PARAM;
    if( (*svc_p11) != NULL )
        return SVC_ERROR_PARAM;
        
    (*svc_p11)=(svc_p11_t *)malloc(sizeof(svc_p11_t));
    if( (*svc_p11) == NULL )
        return SVC_ERROR_MEM;
    memset(*svc_p11,0,sizeof(svc_p11_t));
    
    /* tzn ze jeszcze slot nieustalony */
    (*svc_p11)->_sig_slot=SLOT_UNDEFINED;
    (*svc_p11)->_dec_slot=SLOT_UNDEFINED;
    
    /* zaladowanie, inicjalizacja wielowatkowa biblioteki */
    (*svc_p11)->lHandle=dlopen(pkcs11_lib,RTLD_LAZY);
    if( (*svc_p11)->lHandle == NULL )
        return SVC_ERROR_DISK;
    (*svc_p11)->pC_GetFunctionList=(CK_C_GetFunctionList)dlsym((*svc_p11)->lHandle,"C_GetFunctionList");
    if( (*svc_p11)->pC_GetFunctionList == NULL )
        return SVC_ERROR_FORMAT;
    rv=((*svc_p11)->pC_GetFunctionList)(&((*svc_p11)->pFunctionList));
    if( rv != CKR_OK )
	{
		svc_gen_err_desc(error,"PKCS#11 GetFunctionList failed with code %X",rv);
        return SVC_ERROR_FAILED;
	}
    memset(&init_args,0,sizeof(CK_C_INITIALIZE_ARGS));
    init_args.flags=CKF_OS_LOCKING_OK;
    rv=((*svc_p11)->pFunctionList)->C_Initialize(&init_args);
    if( rv != CKR_OK )
	{
		if( error != NULL )
			svc_gen_err_desc(error,"PKCS#11 C_Initialize failed with code %X",rv);
		return SVC_ERROR_FAILED;
	}
    return SVC_OK;
}

long svc_pkcs11_sign(char *input,long length,
                    CK_SESSION_HANDLE hSession,CK_OBJECT_HANDLE hPrivKey,CK_FUNCTION_LIST_PTR pFunctionList,
                    char **output,long *out_length,char **error)
{
    CK_MECHANISM mechanism = { CKM_RSA_PKCS, NULL_PTR, 0 };
    CK_RV rv;
    CK_ULONG tmp=0;

	rv=pFunctionList->C_SignInit(hSession,&mechanism,hPrivKey);
	if( rv != CKR_OK )
	{
		svc_gen_err_desc(error,"C_SignInit failed with code %X",rv);
		return SVC_ERROR_FAILED;
	}
	rv=pFunctionList->C_Sign(hSession, (CK_BYTE_PTR)input,length,NULL,&tmp);
	if( rv != CKR_OK )
	{
		svc_gen_err_desc(error,"C_Sign failed with code %X",rv);
 	    	return SVC_ERROR_FAILED;
	}
	(*output)=(char *)malloc(tmp+2);
	if( (*output) == NULL )
	    return SVC_ERROR_FAILED;
	memset(*output,0,tmp+1);

	rv=pFunctionList->C_Sign(hSession, (CK_BYTE_PTR)input,length, (CK_BYTE_PTR) *output,&tmp);
	if( rv != CKR_OK )
	{
	    	svc_gen_err_desc(error,"C_Sign failed with code %X",rv);
 	    	return SVC_ERROR_FAILED;
	}
	*out_length=tmp;

    return SVC_OK;
}

long svc_pkcs11_decrypt(char *input,long length,
                       CK_SESSION_HANDLE hSession,CK_OBJECT_HANDLE hPrivKey,CK_FUNCTION_LIST_PTR pFunctionList,
                       char **output,long *out_length,char **error)
{
    CK_MECHANISM mechanism = { CKM_RSA_PKCS, NULL_PTR, 0 };
    CK_RV rv;
    CK_ULONG tmp=0;

	rv=pFunctionList->C_DecryptInit(hSession,&mechanism,hPrivKey);
	if( rv != CKR_OK ) 
		return SVC_ERROR_FAILED;

	rv=pFunctionList->C_Decrypt(hSession, (CK_BYTE_PTR)input,length,NULL,&tmp);
	if( rv != CKR_OK )
 	    return SVC_ERROR_FAILED;

	(*output)=(char *)malloc(tmp+2);
	if( (*output) == NULL )
	    return SVC_ERROR_FAILED;
	memset(*output,0,tmp+1);

	rv=pFunctionList->C_Decrypt(hSession, (CK_BYTE_PTR)input,length, (CK_BYTE_PTR)*output,&tmp);
	if( rv != CKR_OK )
	    return SVC_ERROR_FAILED;

	*out_length=tmp;
	
    return SVC_OK;
}

/* specjalna funkcja listujaca CKA_ID znalezionych kluczy prywatnych */
void svc_p11_list(void)
{
     svc_conf_t *conf=NULL;
     long status,i,j;
     HANDLE lHandle;
     CK_C_GetFunctionList pC_GetFunctionList;
     CK_RV rv;
     CK_FUNCTION_LIST_PTR pFunctionList;
     CK_C_INITIALIZE_ARGS init_args;
     CK_ULONG ulCount=0;
     CK_SLOT_ID_PTR slots=NULL;
     CK_SESSION_HANDLE hSession;
     CK_BYTE myTrue=TRUE;
     CK_ULONG count=0;
     CK_OBJECT_CLASS PrivateKeyClass=CKO_PRIVATE_KEY;
     CK_ATTRIBUTE privatekeyTemplate[]={
				{CKA_CLASS,&PrivateKeyClass,sizeof(PrivateKeyClass)},
				{CKA_TOKEN,&myTrue,sizeof(myTrue)},
     };
     CK_OBJECT_HANDLE hPrivKeys[100];
     CK_ATTRIBUTE privKeyValue[]={
					{CKA_ID,NULL_PTR,0},
					{CKA_LABEL,NULL_PTR,0},
				};     
	 char *error=NULL;

     memset(hPrivKeys,0,100*sizeof(CK_OBJECT_HANDLE));
    
#ifdef WIN32 
     status=svc_load_conf("c:\\bmd_ks\\bmd_ks.conf",&conf);
#else
     status=svc_load_conf("/home/si/bmd_ks.conf",&conf);
#endif
     if( status != SVC_OK )
     {
         printf("Nie mozna wczytac konfiguracji\n");
         return;
     }

     
    /* zaladowanie, inicjalizacja wielowatkowa biblioteki */
    lHandle=dlopen(conf->pkcs11_lib,RTLD_LAZY);
    if( lHandle == NULL )
    {
        printf("nie mozna zaladowac liba\n");
        return;
    }
    pC_GetFunctionList=(CK_C_GetFunctionList)dlsym(lHandle,"C_GetFunctionList");
    if( pC_GetFunctionList == NULL )
    {
        printf("zly format\n");
        return;
    }
    rv=(pC_GetFunctionList)(&pFunctionList);
    if( rv != CKR_OK )
        return;
    memset(&init_args,0,sizeof(CK_C_INITIALIZE_ARGS));
    init_args.flags=CKF_OS_LOCKING_OK;
    rv=pFunctionList->C_Initialize(&init_args);
    if( rv != CKR_OK )
    {
        printf("p11 nie zaincjalizowany\n");
        return;
    }
    /* pobranie slotow */
    /* pobranie listy slotow */
    rv = pFunctionList->C_GetSlotList(TRUE, NULL_PTR, &ulCount);
    if( (rv == CKR_OK) && (ulCount > 0) )
    {
        slots=(CK_SLOT_ID_PTR)malloc(sizeof(CK_SLOT_ID)*(ulCount+2));
        if( slots == NULL )
            return;

        rv = pFunctionList->C_GetSlotList(TRUE,slots,&ulCount);
        if( rv != CKR_OK )
            return;
    }
    else
        return;
    for(i=0; i < (long)ulCount; i++)
    {
		print_debug("Proba nawiazania polaczenia ze slotem numer: %i\n", i);
        status=__svc_connect_slot(conf->pin,i,slots,pFunctionList,&hSession,&error);
		if(status != SVC_OK)
		{
			print_debug("Nie udalo sie zalogowac do slotu numer: %i\n", i);
			continue;
		}
		print_debug("Udalo sie nawiazac polaczenie ze slotem numer: %i\n", i);
	    rv=pFunctionList->C_FindObjectsInit(hSession,privatekeyTemplate,2);
	    if( rv != CKR_OK )
            return ;
        rv=pFunctionList->C_FindObjects(hSession,hPrivKeys,100,&count);
        if( rv != CKR_OK )
        {
            rv=pFunctionList->C_FindObjectsFinal(hSession);
            return;
        }
        if( count == 0 )
        {
            rv=pFunctionList->C_FindObjectsFinal(hSession);
            continue;
        }   
        rv=pFunctionList->C_FindObjectsFinal(hSession);
        
        for(j=0; j < (long)count;j++)
        {
            privKeyValue[0].pValue=NULL;
			privKeyValue[0].ulValueLen=0;
			privKeyValue[1].pValue=NULL;
			privKeyValue[1].ulValueLen=0;
			/* pobierz wartosc certyfikatu oraz cka_id */
			rv=pFunctionList->C_GetAttributeValue(hSession,hPrivKeys[j],privKeyValue,2);
			if(rv==CKR_OK)
			{
                long length,l1;
                char *value,*v1;
                length=privKeyValue[0].ulValueLen;
                value=(char *)malloc(length);
				memset(value,0,length);
				l1=privKeyValue[1].ulValueLen;
				v1=(char *)malloc(l1+2);
				memset(v1,0,l1+1);
			    privKeyValue[0].ulValueLen=length;
                privKeyValue[0].pValue=value;
				privKeyValue[1].ulValueLen=l1;
				privKeyValue[1].pValue=v1;
						
			    rv=pFunctionList->C_GetAttributeValue(hSession,hPrivKeys[j],privKeyValue,2);
			    if(rv==CKR_OK)
			    {
                    long k;
					printf("Etykieta %s ",(char *)privKeyValue[1].pValue);
                    for(k=0;k<length;k++)
                    {
						printf("%02X",((unsigned char *)privKeyValue[0].pValue)[k]);
                    }
                    printf("\n");
                }
                free(value);value=NULL;
            }
        }
            
        pFunctionList->C_CloseSession(hSession);
        memset(hPrivKeys,0,100*sizeof(CK_OBJECT_HANDLE));
    }
}
