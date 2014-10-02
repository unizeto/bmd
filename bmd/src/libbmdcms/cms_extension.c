#include <bmd/libbmdcms/libbmdcms.h>
#include <bmd/libbmdts/libbmdts.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmderr/libbmderr.h>

/*
Autor: Wlodek Szczygiel

rozszerzenia ETSI do CMS, identyfikator polityki podpisu, polityka podpisu
(rfc 3126)
*/

/*------------------------------------------------------------------------------------*/

/*!!<FUNKCJE PRYWATNE >*/

/*--------------------------------------------------------------------*/
/*IDENTYFIKATOR POLITYKI PODPISU - ATRYBUT PODPISANY*/

#define OID_CONTENT_TIMESTAMP 1, 2, 840, 113549, 1, 9, 16, 2, 20


/**
BMDCMS_SP_ATTR - w przypadku, gdy jawnie tworzy sie caly atrybut polityki podpisu
BMDCMS_SP_NULL - w przypadku, gdy stworzony jest atrybut polityki podpisu wskazujacy na to,
 ze polityka podpisu wynika z podpisanych danych; dla tej flagi argument spId jest ignorowany (mozna podac NULL)

funkcja tworzy atrybut podpisany SignaturePolicy:
	dla flagi BMDCMS_SP_ATTR tworzy SignaturePolicy i wrzuca do niej gotowa strukture SignaturePolicyId
	dla flagi BMDCMS_SP_NULL tworzy SignaturePolicy i ustawia ja jako implied - tj. w takiej
	sytaucji polityka podpisu wynika z podpisanych danych (nie wnikam w tej chwili, co to oznacza)
!!! zawartosc spId jest kopiowana, zmienna nalezy zwolnic recznie po wykorzystaniu w tej funkcji
@ARG flag to jedna z dwoch wymienionych flag
@ARG spId to wskaznik do przygotowanej struktury SignaturePolicyId
@ARG newSP to adres wskaznika do struktury SignaturePolicy (pod nim jest zapisana stworzona struktura)
@RETURN 0 w przypadku sukcesu
	-1 gdy problemy z alokacja pamieci
*/

long Create_SignaturePolicyIdentifier(long flag, SignaturePolicyId_t *spId, SignaturePolicyIdentifier_t **newSP)
{
	SignaturePolicyIdentifier_t *SPnowy=NULL;
	SignaturePolicyImplied_t *SPimpl=NULL;

	if (!(newSP != NULL))
	{
		PRINT_ERROR("newSP nie moze być NULL!(WS)\n");
		abort();
	}
	if (!(*newSP == NULL))
	{
		PRINT_ERROR("*newSP musi być NULL!(WS)\n");
		abort();
	}

	if( (flag!=BMDCMS_SP_NULL) && (spId==NULL) )
	{
		PRINT_ERROR("flag musi być rowny BMDCMS_SP_NULL, a spId nie moze być rowny NULL(WS)\n");
		abort();
	}

	SPnowy=(SignaturePolicyIdentifier_t*)calloc(1,sizeof(SignaturePolicyIdentifier_t));
	if(SPnowy==NULL)
	{
		return -1;
	}

	/*jesli polityka podpisu ma wynikac z podpisanych danych*/
	if(flag==BMDCMS_SP_NULL)
	{
		SPimpl=(SignaturePolicyImplied_t*)calloc(1,sizeof(SignaturePolicyImplied_t));
		if(SPimpl==NULL)
		{
			free(SPnowy);
			return -1;
		}
		SPnowy->present=SignaturePolicyIdentifier_PR_signaturePolicyImplied;
		SPnowy->choice.signaturePolicyImplied=*SPimpl;
	}
	/*jesli polityka podpisu jawnie definiowana*/
	else
	{
		SPnowy->present=SignaturePolicyIdentifier_PR_signaturePolicyId;
		SPnowy->choice.signaturePolicyId=*spId;
	}
	*newSP=SPnowy;
	return 0;
}



/*-- ?? */
/**funkcja tworzy strukture SignaturePolicyId na podstawie przygotowanych SigPolicyId i SigPolicyHash i SigPolicyQualifiers
funkcja jedynie umieszcza przygotowane strukturki w tworzonym SignaturePolicyId (nie tworzy kopii strukturek)
!!!! zawartosc sigP_Id jest kopiowana , po wykorzystaniu zmienna nalezy recznie zwolnic
!!!! zawartosc sigP_Hash jest kopiowana, po wykorzystaniu zmienna nalezy recznie zwolnic
@ARG sigP_Id to wskaznik na przygotowana strukture SigPolicyId
@ARG sigP_Hash to wskaznik na przygotowana strukture SigPolicyHash
@ARG sigP_Qualifiers to wskaznik na strukture sigPolicyQualifiers(struktura zagniezdzona w SignaturePolicyId_t)
@ARG SPId to adres wskaznika do struktury SignaturePolicyId (pod nim jest zapisana stworzona struktura)
@RETURN 0 w przypadku sukcesu
	-1 gdy wystapily problemy z alokacja pamieci;
*/

long Create_SignaturePolicyId(SigPolicyId_t *sigP_Id, SigPolicyHash_t *sigP_Hash,
		SigPolicyQualifiers_t *sigP_Qualifiers, SignaturePolicyId_t **SPId)
{
	SignaturePolicyId_t *nowy=NULL;

	if (!(sigP_Id != NULL))
	{
		PRINT_ERROR("sigP_Id nie moze być rowny NULL!(WS)\n");
		abort();
	}
	if (!(sigP_Hash != NULL))
	{
		PRINT_ERROR("sigP_Hash nie moze być rowny NULL!(WS)\n");
		abort();
	}
	if (!(SPId != NULL))
	{
		PRINT_ERROR("SPId nie moze być rowny NULL!(WS)\n");
		abort();
	}
	if (!(*SPId == NULL))
	{
		PRINT_ERROR("*SPId musi byc rowne NULL!(WS)\n");
		abort();
	}

	nowy=(SignaturePolicyId_t*)calloc(1,sizeof(SignaturePolicyId_t));
	if(nowy == NULL)
	{
		return -1;
	}

	nowy->sigPolicyIdentifier=*sigP_Id;
	nowy->sigPolicyHash=*sigP_Hash;
	nowy->sigPolicyQualifiers=sigP_Qualifiers;

	*SPId=nowy;
	return 0;
}


void Destroy_SignaturePolicyId(SignaturePolicyId_t **SPId)
{
	if(!(SPId != NULL))
	{
		PRINT_ERROR("SPId nie moze być rowne NULL!(WS)\n");
		abort();
	}
	if(*SPId != NULL )
	{
		free(*SPId);
		*SPId=NULL;
	}
}

/**
funckja Create_SigPolicyId tworzy SigPolicyId_t (typedef na OBJECT_IDENTIFIER_t)
na podstawie tablicy tablicy liczb skladajacych sie na oid

@ARG oid to tablica long zawierajaca liczby skladajace sie na oid
@ARG N to ilosc komorek tablicy oid
@ARG SPId to wskaznik do modyfikacji (pod nim zapisany jest stworzony SigPolicyId)

@RETURN 0 , jesli wszystko ok
	-1 jesli problemy z alokacja pamieci albo utworzeniem OID z tablicy liczb
*/
long Create_SigPolicyId(long *oid, long N, SigPolicyId_t **SPId)
{
	long ret=0;
	SigPolicyId_t* obj_ident=NULL;

	if (!(oid != NULL))
	{
		PRINT_ERROR("Oid nie moze być rowny NULL!(WS)\n");
		abort();
	}
	if (!(SPId != NULL))
	{
		PRINT_ERROR("SPId nie moze być rowny NULL!(WS)\n");
		abort();
	}
	if (!(*SPId == NULL))
	{
		PRINT_ERROR("*SPId musi być rowny NULL!(WS)\n");
		abort();
	}

	/*SigPolicyId_t jest typedefem na OBJECT_IDENTIFIER_t*/
	obj_ident=(SigPolicyId_t*)calloc(1,sizeof(SigPolicyId_t));
	if(obj_ident==NULL)
	{
		return -1;
	}
	/*SigPolicyId jest typedefem na OBJECT_IDENTIFIER*/
	ret=OBJECT_IDENTIFIER_set_arcs(obj_ident, oid, sizeof(oid[0]),N);
	if(ret!=0)
	{
		free(obj_ident);
		return -1;
	}
	*SPId=obj_ident;
	return 0;
}



/**
dzialanie funkcji Create_SigPolicyId_char jest analogiczne do funkcji Create_SigPolicyId
roznica: OID tworzony jest na podstawie stringa (sekwencji liczb oddzielonych kropkami)

@ARG oid to string z oid
@ARG SPId to wskaznik do modyfikacji (pod nim zapisany jest stworzony SigPolicyId)

@RETURN 0 , jesli wszystko ok
	-1 jesli problemy z alokacja pamieci albo utworzeniem OID z tablicy liczb

*/
long Create_SigPolicyId_char(char *oid, SigPolicyId_t **SPId)
{
	long ret=0;
	SigPolicyId_t *obj_ident=NULL;

	if(!(oid != NULL))
	{
		PRINT_ERROR("oid nie moze być rowny NULL!(WS)\n");
		abort();
	}
	if (!(SPId != NULL))
	{
		PRINT_ERROR("SPId nie moze być rowne NULL!(WS)\n");
		abort();
	}
	if (!(*SPId == NULL))
	{
		PRINT_ERROR("*SPId musi być rowne NULL!(WS)\n");
		abort();
	}


	obj_ident=(OBJECT_IDENTIFIER_t*)calloc(1,sizeof(OBJECT_IDENTIFIER_t));
	if(obj_ident==NULL)
	{
		return -1;
	}
	/*SigPolicyId jest typdefem na OBJECT_IDENTIFIER*/
	ret=String2OID(oid, obj_ident);
	if(ret!=0)
	{
		free(obj_ident);
		return -1;
	}

	*SPId=obj_ident;

	return 0;
}



void Destroy_SigPolicyId(SigPolicyId_t **destroy_SPId)
{
	if (!(destroy_SPId != NULL))
	{
		PRINT_ERROR("destroy_SPId nie moze być rowne NULL!(WS)\n");
		abort();
	}

	if(*destroy_SPId != NULL)
	{
		free(*destroy_SPId);
		*destroy_SPId=NULL;
	}

}


/**
tworzy strukturke SigPolicyQualifiers_t (struktura ta zawiera liste, do ktorej beda dodawane elementy )
@ARG SPq to wskaznik do modyfikacji (pod nim zapisana jest stworzona strukturka SigPolicyQualifiers_t )
@RETURN 0 gdy ok.
	-1 , gdy wystapily problemy z alokacja pamieci

*/
long Create_SigPolicyQualifiers(SigPolicyQualifiers_t **SPq)
{
	SigPolicyQualifiers_t *SPq_nowy=NULL;

	if(!(SPq != NULL))
	{
		PRINT_ERROR("SPq nie moze być rowne NULL!(WS)\n");
		abort();
	}
	if (!(*SPq == NULL))
	{
		PRINT_ERROR("*SPq musi być rowne NULL!(WS)\n");
		abort();
	}

	SPq_nowy=(SigPolicyQualifiers_t*)calloc(1,sizeof(SigPolicyQualifiers_t));
	if(SPq_nowy == NULL)
	{
		return -1;
	}

	*SPq=SPq_nowy;
	return 0;
}


/**
funkcja dodaje do listy (w strukturce SigPolicyQualifiers_t nowa strukturke SigPolicyQualifierInfo_t)
@ARG new_item to wskaznik do przygotowanej strukturki SigPolicyQualifierInfo_t
@ARG SPq to wskaznik do stworzonej struktury SigPolicyQualifiers_t
@RETURN 0 gdy ok.
	-1 w przypadku problemow z dodaniem do listy
*/
long Add2SigPolicyQualifiers(SigPolicyQualifierInfo_t *new_item, SigPolicyQualifiers_t *SPq)
{
	long ret=0;

	if(!(new_item != NULL))
	{
		PRINT_ERROR("new_item nie moze być rowne NULL!(WS)\n");
		abort();
	}
	if (!(SPq != NULL))
	{
		PRINT_ERROR("SPq nie moze być rowne NULL!(WS)\n");
		abort();
	}

	ret=asn_set_add(&(SPq->list),new_item);

	return ret;
}

/**
fukcja tworzy SigPolicyQualifierInfo i wrzuca odpowiednio przygotowana strukturke SPuri_t badz SPUserNotice_t
!!! zawartosc item jest kodowana do DER i w tej postaci wrzucana do strukturki
!!! item podawany jako argument nalezy recznie zwolnic po wywolaniu tej funkcji za pomoca odpowiedniej funkcji
!!! Destroy_SPuri badz Destroy_SPUserNotice
@ARG item to SPuri badz SPUserNotice (o spodziewanym typie decyduje uzyta flaga)
@ARG flag to flaga, ktora mowi, jakiego typu strukturka kryje sie w item:
	BMDCMS_SP_URI dla SPuri_t
	BMDCMS_SP_USERNOTICE dla SPUserNotice_t
@ARG SPqi to wskaznik do modyfikacji, pod ktorym zostanie zapisana stworzona struktura SigPolicyQualifierInfo_t

@RETURN 0 jesli ok.
	-1 w przypadku problemow z alokacja pamieci
	-2 w przypadku problemow z zakodowaniem item
	-3 w przypadku problemow z utworzeniem OID
*/
long Create_SigPolicyQualifierInfo(void *item, long flag, SigPolicyQualifierInfo_t **SPqi)
{
	long ret=0,status;
	OBJECT_IDENTIFIER_t *oid;
	long oid_uri[]= { OID_CMS_URI };
	long oid_unotice[]= { OID_CMS_USER_NOTICE };
	long *oid_tab=NULL;
	ANY_t *Any_nowy=NULL;
	SigPolicyQualifierInfo_t *SPqi_nowy=NULL;

	if(!(item != NULL))
	{
		PRINT_ERROR("item nie moze być rowne NULL (WS)\n");
		abort();
	}
	if (!(SPqi != NULL))
	{
		PRINT_ERROR("SPqi nie moze być rowne NULL!(WS)\n");
		abort();
	}
	if (!(*SPqi == NULL))
	{
		PRINT_ERROR("*SPqi musi być rowne NULL!(WS)\n");
		abort();
	}

	/*alokacja strukturki SigPolicyQualifierInfo_t*/
	SPqi_nowy=(SigPolicyQualifierInfo_t*)calloc(1,sizeof(SigPolicyQualifierInfo_t));
	if(SPqi_nowy == NULL)
	{
		return -1;
	}
	/*alokacja strukturki ANY_t*/
	Any_nowy=(ANY_t*)calloc(1,sizeof(ANY_t));
	if(Any_nowy == NULL)
	{
		free(SPqi_nowy);
		return -1;
	}

	/*w zaleznosci od flagi przygotowywany odpowiedni OID i na podstawie item stworzony odpowiednio ANY */
	if( flag == BMDCMS_SP_URI )
	{
		/*przypisanie odpowiedniej tablicy do zbudowania OID*/
		oid_tab=oid_uri;
		status=ANY_fromType(Any_nowy,&asn_DEF_SPuri,item);
		if( status != 0 )
		{
			free(SPqi_nowy);
			free(Any_nowy);
			return -2;
		}
	}
	else
	{
		/*przypisanie odpowiedniej tablicy do zbudowania OID*/
		oid_tab=oid_unotice;
		status=ANY_fromType(Any_nowy,&asn_DEF_SPUserNotice,item);
		if( status != 0 )
		{
			free(SPqi_nowy);
			free(Any_nowy);
			return -2;
		}
	}

	SPqi_nowy->sigQualifier=*Any_nowy;
	/*wskaznik na bufor przekopiowany jest na SPqi_nowy->sigQualifier dlatego nie wolno go teraz zwolnic*/
	Any_nowy->buf=NULL;
	free(Any_nowy);

	oid=(OBJECT_IDENTIFIER_t*)calloc(1,sizeof(OBJECT_IDENTIFIER_t));
	if(oid == NULL)
	{
		free(SPqi_nowy);
		free(Any_nowy->buf);
		free(Any_nowy);
		return -1;
	}
	ret=OBJECT_IDENTIFIER_set_arcs(oid, oid_tab, sizeof(oid_tab[0]), 9);
	if(ret != 0)
	{
		free(SPqi_nowy);
		free(Any_nowy->buf);
		free(Any_nowy);
		asn_DEF_OBJECT_IDENTIFIER.free_struct(&asn_DEF_OBJECT_IDENTIFIER, oid, 1);
		free(oid);
		return -3;
	}

	/**/
	SPqi_nowy->sigPolicyQualifierId=*oid;

	/*nie mozna zwalniac zawartosci bo jest przekopiowana (zwalniam jedynie "opakowanie")  */
	free(oid);

	*SPqi=SPqi_nowy;

	return ret;
}


/**
funkcja na podstawie string tworzy strukturke SPuri_t
@ARG str to string zrodlowy (zawiera URI pod ktorym mozna odnalezc pelna polityke podpisu)
@ARG SPuri to adres wskaznika, pod ktory zapisana zostanie stworzona strukturka SPuri_t
@RETURN 0 jesli ok.
	-1 w przypadku problemow z alokacja pamieci
*/
long Create_SPuri(char *str, SPuri_t **SPuri)
{
	long ret=0;
	SPuri_t *nowy=NULL;

	if (!(SPuri != NULL))
	{
		PRINT_ERROR("SPuri nie moze być NULL!(WS)\n");
		abort();
	}
	if(!((*SPuri) == NULL))
	{
		PRINT_ERROR("*SPuri musi być rowne NULL!(WS)\n");
		abort();
	}

	nowy=(SPuri_t*)calloc(1, sizeof(SPuri_t));
	if(nowy==NULL)
	{
		return -1;
	}

	/*SPuri_t to typedef na IA5String = OCTET_STRING*/
	ret=OCTET_STRING_fromString(nowy, str);

	if(ret != 0)
	{
		free(nowy);
		return -1;
	}
	*SPuri=nowy;
	return 0;
}

void Destroy_SPuri(SPuri_t **destroy_spuri)
{
	if(!(destroy_spuri != NULL))
	{
		PRINT_ERROR("destroy_spuri nie moze być rowne NULL!(WS)\n");
		abort();
	}

	if(*destroy_spuri != NULL)
	{
		asn_DEF_SPuri.free_struct(&asn_DEF_SPuri, *destroy_spuri,1);
		free(*destroy_spuri);
		*destroy_spuri=NULL;
	}
}

/**
tworzy strukturke SPUserNotice_t i wrzuca DisplayText_t oraz NoticeReference_t
dText oraz nRef moga byc NULL, bowiem sa opcjonalne w NoticeReference_t
@ARG nRef to wskaznik do przygotowanej strukturki NoticeReference_t (lub NULL)
@ARG dText to wskaznik do przygotowanej strukturki DisplayText_t (lub NULL)
@RETURN 0 jesli ok
	-1 jesli wystapily problemy z alokacja pamieci
*/
long Create_SPUserNotice(NoticeReference_t *nRef, DisplayText_t *dText, SPUserNotice_t **SPun)
{
	SPUserNotice_t *nowy=NULL;

	/*dText oraz nRef moga byc NULL, bowiem sa opcjonalne w NoticeReference_t*/
	if (!(SPun != NULL))
	{
		PRINT_ERROR("SPun nie moze być rowne NULL!(WS)\n");
		abort();
	}
	if (!(*SPun == NULL))
	{
		PRINT_ERROR("*SPun musi być rowne NULL!(WS)\n");
		abort();
	}

	nowy=(SPUserNotice_t*)calloc(1, sizeof(SPUserNotice_t));
	if(nowy == NULL)
	{
		return -1;
	}
	nowy->noticeRef=nRef;
	nowy->explicitText=dText;

	*SPun=nowy;
	return 0;
}


void Destroy_SPUserNotice(SPUserNotice_t **SPun)
{
	SPUserNotice_t *destroy_SPun=NULL;

	if (!(SPun != NULL))
	{
		PRINT_ERROR("SPun nie moze być rowny NULL!(WS)\n");
		abort();
	}

	if(*SPun != NULL)
	{
		destroy_SPun=*SPun;
		asn_DEF_SPUserNotice.free_struct(&asn_DEF_SPUserNotice, destroy_SPun,1);
		free(destroy_SPun);
		*SPun=NULL;
	}
}

/**
funkcja tworzy strukturke NoticeReference_t do ktorej wrzuca przygotowana strukturke DisplayText_t
!!! funkcja kopiuje zawartosc dText;
!!! dText musi byc recznie zwolniona na zewnatrz funkcji (za pomoca Destroy_DisplayText)
@ARG dText to wskaznik do przygotowanej strukturki DisplayText_t
@ARG NoticeRef to adres wskaznika pod ktory zapisana jest stworzona strukturka NoticeReference_t
@RETURN 0 jesli ok.
	-1 jesli problem z alokacja pamieci
*/
long Create_NoticeReference(DisplayText_t *dText, NoticeReference_t **NoticeRef)
{
	NoticeReference_t *nowy=NULL;

	if(!(dText != NULL))
	{
		PRINT_ERROR("dText nie moze być rowne NULL!(WS)\n");
		abort();
	}
	if (!(NoticeRef != NULL))
	{
		PRINT_ERROR("NoticeRef nie moze być rowne NULL!(WS)\n");
		abort();
	}
	if (!(*NoticeRef == NULL))
	{
		PRINT_ERROR("*NoticeRef musi być rowne NULL!(WS)\n");
		abort();
	}

	nowy=(NoticeReference_t*)calloc(1,sizeof(NoticeReference_t));
	if(nowy == NULL)
	{
		return -1;
	}
	nowy->organization=*dText;
	*NoticeRef=nowy;
	return 0;
}



/**
(nie mam pojecia, po co to)
funkcja dodaje integer do listy w NoticeRef
@ARG val to dodawana wartosc
@ARG NoticeRef to wskaznik do strukturki, w ktorej wartosc val ma byc dodana
@RETURN 0 jesli ok.
	-1 jesli blad
*/
long AddInt2NoticeReference(long val, NoticeReference_t *NoticeRef)
{
	INTEGER_t *nowy=NULL;
	long ret = 0;

	if(!(NoticeRef != NULL))
	{
		PRINT_ERROR("NoticeRef nie moze być rowne NULL!(WS)\n");
		abort();
	}

	nowy=(INTEGER_t*)calloc(1,sizeof(INTEGER_t));
	if(nowy == NULL)
	{
		return -1;
	}

	ret=asn_long2INTEGER(nowy, val);
	if(ret != 0)
	{
		free(nowy);
		return -1;
	}

	ret=asn_set_add(&(NoticeRef->noticeNumbers.list),nowy);

	return ret;
}

/**
funkcja tworzy strukturke DisplayText_t na podstawie string. Uzytkownik funkcji ma mozliwosc wskazac docelowa
postac Stringu za pomoca odpowiedniej flagi.
@ARG str jest lancuchem, na podstawie ktorego ma byc stworzona struktura DisplayText_T
@ARG flag okresla docelowa postac lancucha:
	BMD_SP_VISIBLE dla VisibleString
	BMD_SP_BMP dla BMPString
	BMD_SP_UTF8 dla UTF8String
@ARG dText to adres wskaznika ktory bedzie przechowywac utworzona strukturke DisplayText_t
@RETURN	0 jesli ok.
	-1 jesli problemy z alokacja pamieci
	-2 jesli problem z utworzeniem OCTET_STRING_t
*/
long Create_DisplayText(char *str, long flag, DisplayText_t **dText)
{
	long ret=0;
	DisplayText_t *nowy=NULL;
	OCTET_STRING_t *nowy_octet=NULL;

	if (!(str != NULL))
	{
		PRINT_ERROR("str nie moze być rowny NULL!(WS)\n");
		abort();
	}
	if(!(dText != NULL))
	{
		PRINT_ERROR("dText nie moze być NULL!(WS)\n");
		abort();
	}
	if(!(*dText == NULL))
	{
		PRINT_ERROR("*dText musi być NULL!(WS)\n");
		abort();
	}


	nowy=(DisplayText_t*)calloc(1, sizeof(DisplayText_t));

	if(nowy == NULL )
	{
		return -1;
	}
	nowy_octet=(OCTET_STRING_t*)calloc(1, sizeof(OCTET_STRING_t));
	if(nowy_octet == NULL )
	{
		free(nowy);
		return -1;
	}

	ret=OCTET_STRING_fromString(nowy_octet, str);
	if(ret != 0)
	{
		free(nowy);
		free(nowy_octet);
		return -2;
	}
	/*VisibleString, BMPString, oraz UTF8String sa typedefami na OCTET_STRING*/
	switch(flag)
	{
		case BMD_SP_VISIBLE:
		{
			ret=asn_check_constraints(&asn_DEF_VisibleString, nowy_octet, NULL, NULL);
			if(ret==0)
			{
				nowy->present=DisplayText_PR_visibleString;
				nowy->choice.visibleString=*nowy_octet;
			}
			break;
		}
		case BMD_SP_BMP:
		{
			ret=asn_check_constraints(&asn_DEF_BMPString, nowy_octet, NULL, NULL);
			if(ret==0)
			{
				nowy->present=DisplayText_PR_bmpString;
				nowy->choice.bmpString=*nowy_octet;
			}
			break;
		}
		case BMD_SP_UTF8:
		{
			ret=asn_check_constraints(&asn_DEF_UTF8String, nowy_octet, NULL, NULL);
			if(ret==0)
			{
				nowy->present=DisplayText_PR_utf8String;
				nowy->choice.utf8String=*nowy_octet;
			}
			break;
		}
		default:
		{
			ret=-1;
		}
	}

	/*jesli nie spelnione ograniczenia na dlugosc Stringu*/
	if(ret != 0)
	{
		free(nowy);
		asn_DEF_OCTET_STRING.free_struct(&asn_DEF_OCTET_STRING, nowy_octet, 1);
		free(nowy_octet);
		return ret;
	}

	*dText=nowy;
	free(nowy_octet);
	return 0;
}


void Destroy_DisplayText(DisplayText_t **dText)
{
	if(!(dText != NULL))
	{
		PRINT_ERROR("dText nie moze być NULL!(WS)\n");
		abort();
	}
	if(*dText!=NULL)
	{
		free(*dText);
		*dText=NULL;
	}
}

/**
funkcja tworzy strukturke SigPolicyHash_t i wrzuca do niej przygotowany identyfikator algorytmu hashujacego oraz OtherHashValue
!!!!!!!!!!!!!!    SigPolicyHash_t jest typedef na OtherHashAlgorithmAndValue_t !!!!!!!!!!!!!!!!!!!!!!!!!!
!!! zawartosc algId jest kopiowana, nalezy recznie zwolnic po wywolaniu funkcji
@ARG algId to wskaznik do przygotowanej strukturki AlgorithmIdentifier_t
@ARG othHashVal to wskaznik do przygotowanej strukturki OtherHashValue_t
@ARG SPh to adres wskaznika, ktory bedzie przechowywal utworzona strukturke SigpolicyHash_t
@RETURN 0 w przypadku sukcesu
	-1 gdy problemy z alokacja pamieci
*/
long Create_SigPolicyHash(AlgorithmIdentifier_t *algId, OtherHashValue_t* othHashVal, SigPolicyHash_t **SPh)
{
	SigPolicyHash_t *nowy=NULL;

	if (!(algId != NULL))
	{
		PRINT_ERROR("algId nie moze być NULL(WS)\n");
		abort();
	}
	if(!(othHashVal != NULL))
	{
		PRINT_ERROR("othHashVal nie moze być NULL!(WS)\n");
		abort();
	}
	if (!(SPh != NULL))
	{
		PRINT_ERROR("SPh nie moze być NULL!(WS)\n");
		abort();
	}
	if (!(*SPh == NULL))
	{
		PRINT_ERROR("*SPh musi być NULL!(WS)\n");
		abort();
	}

	nowy=(SigPolicyHash_t*)calloc(1,sizeof(SigPolicyHash_t));
	if(nowy==NULL)
	{
		return -1;
	}

	nowy->hashAlgorithm=*algId;
	nowy->hashValue=*othHashVal;

	*SPh=nowy;
	return 0;
}

void Destroy_SigPolicyHash(SigPolicyHash_t **SPh)
{
	if (!(SPh != NULL))
	{
		PRINT_ERROR("SPh nie moze być NULL!(WS)\n");
		abort();
	}

	if( *SPh != NULL )
	{
		free(*SPh);
		*SPh=NULL;
	}
}

/**
funkcja Create_AlgorithmIdentifier tworzy strukturke AlgorithmIdentifier_t, ustawia wewnatrz niej odpowiedni OID i wstawia parametry algorytmu, jesli te sa wymagane
AlgorithmIdentifier sklada sie z OIDu i ANY
mimo zaimplementowanych wrapperow AC_AlgorithmIdentifier wrzucam wlasna funkcyjke
@ARG oid to tablica z wartosciami OIDu
@ARG N to dlugosc tablicy oid
@ARG any to parametry algorytmu; pole jest opcjonalne, wiec moze byc podane jako NULL
@ARG algId to adres wskaznika pod ktorym przechowywana bedzie utworzona strukturka
@RETURN 0 jesli ok.
	-1 jesli wystapily problemy przy alokacji pamieci
	-2 jesli nie mozna bylo utworzyc OIDu
*/

long Create_AlgorithmIdentifier(long *oid, long N, ANY_t* any, AlgorithmIdentifier_t **algId)
{
	long ret=0;
	OBJECT_IDENTIFIER_t *obj_ident=NULL;
	AlgorithmIdentifier_t *nowy_alg=NULL;

	/*any jest OPTIONAL*/
	if (!(oid != NULL))
	{
		PRINT_ERROR("oid nie moze być NULL!(WS)\n");
		abort();
	}
	if (!(algId != NULL))
	{
		PRINT_ERROR("algId nie moze być NULL!(WS)\n");
		abort();
	}
	if (!(*algId == NULL))
	{
		PRINT_ERROR("*algId musi być NULL!(WS)\n");
		abort();
	}

	nowy_alg=(AlgorithmIdentifier_t*)calloc(1, sizeof(AlgorithmIdentifier_t));
	if(nowy_alg==NULL)
	{
	 return -1;
	}

	obj_ident=(OBJECT_IDENTIFIER_t*)calloc(1, sizeof(OBJECT_IDENTIFIER_t));
	if(obj_ident==NULL)
	{
		free(nowy_alg);
		return -1;
	}
	ret=OBJECT_IDENTIFIER_set_arcs(obj_ident, oid, sizeof(oid[0]), N);
	if(ret != 0)
	{
		free(nowy_alg);
		free(obj_ident);
		return -2;
	}

	nowy_alg->algorithm=*obj_ident;
	/*tylko zwalnianie "opakowania"*/
	free(obj_ident);

	nowy_alg->parameters=any;
	*algId=nowy_alg;

	return 0;
}

/* juz jest funkcja z rodziny Destroy ponizej */

long Create_AlgorithmIdentifier_char(char* oid, ANY_t* any, AlgorithmIdentifier_t **algId)
{
	long ret=0;
	OBJECT_IDENTIFIER_t *obj_ident=NULL;
	AlgorithmIdentifier_t *nowy_alg=NULL;

	if (!(oid != NULL))
	{
		PRINT_ERROR("oid nie moze być NULL!(WS)\n");
		abort();
	}
	if(!(algId != NULL))
	{
		PRINT_ERROR("algId nie moze być NULL(WS)\n");
		abort();
	}
	if (!(*algId == NULL))
	{
		PRINT_ERROR("*algId musi być NULL!(WS)\n");
		abort();
	}

	nowy_alg=(AlgorithmIdentifier_t*)calloc(1, sizeof(AlgorithmIdentifier_t));
	if(nowy_alg==NULL)
	{
	 return -1;
	}

	obj_ident=(OBJECT_IDENTIFIER_t*)calloc(1, sizeof(OBJECT_IDENTIFIER_t));
	if(obj_ident==NULL)
	{
		free(nowy_alg);
		return -1;
	}
	ret=String2OID(oid, obj_ident);
	if(ret != 0)
	{
		free(nowy_alg);
		free(obj_ident);
		return -2;
	}

	nowy_alg->algorithm=*obj_ident;
	free(obj_ident);

	nowy_alg->parameters=any;
	*algId=nowy_alg;

	return 0;
}

void Destroy_AlgorithmIdentifier(AlgorithmIdentifier_t **algId)
{
	AlgorithmIdentifier_t *destroy_alg=NULL;

	if (!(algId != NULL))
	{
		PRINT_ERROR("algId nie moze być NULL!(WS)\n");
		abort();
	}

	destroy_alg=*algId;

	if(*algId != NULL)
	{
		free(destroy_alg);
		*algId=NULL;
	}
}

/**
 OtherHashValue_t jest typedefem na OCTET_STRING_t, dlatego nie bede prototypowal odrebnej funkcji na jego
 tworzenie. Ze wzgledu na specyfike wykorzystania OtherHashValue w identyfikatorze polityki podpisu dodaje funkcje Destroy_OtherHashValue_spec wywolywana po funkcji Create_SigPolicyHash, ktora zwalnia tylko "opakowanie", bowiem bufor zawarty w OtherHashValue jest wykorzystywany przez SignaturePolicyIdentifier
*/
void Destroy_OtherHashValue_spec(OtherHashValue_t **ohv)
{
	if (!(ohv != NULL))
	{
		PRINT_ERROR("ovh nie moze być NULL!(WS)\n");
		abort();
	}
	if(*ohv != NULL)
	{
		free(*ohv);
		*ohv=NULL;
	}
}

/**ta funkcja w pelni zwalnia ohv - opakowanie jak i zawartosc*/
void Destroy_OtherHashValue(OtherHashValue_t **ohv)
{
	if (!(ohv != NULL))
	{
		PRINT_ERROR("ovh nie moze być NULL!(WS)\n");
		abort();
	}
	if(*ohv != NULL)
	{
		asn_DEF_OtherHashValue.free_struct(&asn_DEF_OtherHashValue, *ohv, 1);
		free(*ohv);
		*ohv=NULL;
	}
}

/*-----------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------*/

/*!! <FUNKCJE PUBLICZNE> */

/**
funkcja zgodnie ze struktura identyfikatora polityki podpisu dodaje URI wskazujacy na polityke podpisu
dodawanie uri mozliwe jest na bazie identyfikatora polityki podpisu utworzonego funkcja SPId_SPIdentifier_Create()
@ARG uri to string z uri wskazujacym polityke podpisu
@ARG dest to identyfikator polityki podpisu stworzony funkcja SPId_SPIdentifier_Create()
@RETURN 0 jesli ok.
	-1 jesli funkcja zawiodla
*/
long SPId_Uri_Add(char *uri, SignaturePolicyIdentifier_t *dest)
{
	SPuri_t *spuri=NULL;
	SigPolicyQualifierInfo_t *spqi=NULL;
	long ret=0;

	if(uri == NULL)
		{ return BMD_ERR_PARAM1; }
	if(dest == NULL)
		{ return BMD_ERR_PARAM2; }

	ret=Create_SPuri(uri, &spuri);
	if(ret != 0)
	{
		return BMD_ERR_OP_FAILED;
	}

	ret=Create_SigPolicyQualifierInfo( (void*)spuri, BMDCMS_SP_URI, &spqi);
	if(ret != 0)
	{
		asn_DEF_SPuri.free_struct(&asn_DEF_SPuri, spuri, 1);
		free(spuri);
		return BMD_ERR_OP_FAILED;
	}
	Destroy_SPuri(&spuri);

	ret=Add2SigPolicyQualifiers(spqi, dest->choice.signaturePolicyId.sigPolicyQualifiers);
	if(ret != 0)
	{
		asn_DEF_SigPolicyQualifierInfo.free_struct(&asn_DEF_SigPolicyQualifierInfo, spqi, 1);
		free(spqi);
		return BMD_ERR_OP_FAILED;
	}

	return 0;
}

/**
funkcja zgodnie ze struktura identyfikatora polityki podpisu dodaje informacje, ktore maja byc wyswietlane podczas walidacji podpisu
dodawanie notice reference mozliwe jest na bazie identyfikatora polityki podpisu utworzonego funkcja SPId_SPIdentifier_Create()
@ARG text to string z tekstem, ktory ma byc wyswietlany podczas walidacji podpisu
@ARG noticeNums to tablica z licbami (nie ma pojecia do czego to potrzebne ale zaimplementowalem)
@ARG N to ilosc komorek tablicy noticeNums
@ARG dest to identyfikator polityki podpisu stworzony funkcja SPId_SPIdentifier_Create()
@RETURN 0 jesli ok.
	-1 jesli funkcja zawiodla
*/
long SPId_UserNotice_Add(char* text, char *organization, long* noticeNums, long N,
								SignaturePolicyIdentifier_t* dest)
{
	long ret=0, iter=0;
	DisplayText_t* expText=NULL; /*explicitText*/
	DisplayText_t* org=NULL;
	NoticeReference_t* noticeref=NULL;
	SPUserNotice_t* spusrnot=NULL;
	SigPolicyQualifierInfo_t *spqi=NULL;

	/*text oraz organization sa opcjonalne*/
	if(N<0)
		{ return BMD_ERR_PARAM4; }
	if(dest == NULL)
		{ return BMD_ERR_PARAM5; }
	/*jesli podano niezerowy rozmiar tablicy, to noticeNums nie moze być NULL, musi być tez
	  podane organization*/
	if(N>0)
	{
		if(noticeNums == NULL)
			{ return BMD_ERR_PARAM3; }
	}
	/****/

	/*explicitText w SPUserNotice*/
	if(text != NULL)
	{
		ret=Create_DisplayText(text, BMD_SP_UTF8, &expText);
		if(ret != 0)
		{
			return BMD_ERR_OP_FAILED;
		}
	}

	/*organization w NoticeReference*/
	if(organization != NULL)
	{
		ret=Create_DisplayText(organization, BMD_SP_UTF8, &org);
		if(ret!=0)
		{
			asn_DEF_DisplayText.free_struct( &asn_DEF_DisplayText, expText, 1);
			free(expText); expText=NULL;
			return BMD_ERR_OP_FAILED;
		}

		ret=Create_NoticeReference(org, &noticeref);
		if(ret != 0)
		{
			asn_DEF_DisplayText.free_struct( &asn_DEF_DisplayText, expText, 1);
			free(expText); expText=NULL;
			asn_DEF_DisplayText.free_struct( &asn_DEF_DisplayText, org, 1);
			free(org); org=NULL;
			return BMD_ERR_OP_FAILED;
		}
		/*zwolnione samo opakowanie*/
		Destroy_DisplayText(&org);

		/*jesli tablica liczb przekazana do funkcji*/
		if(noticeNums != NULL)
		{
			for(iter=0; iter<N; iter++)
			{
				ret=AddInt2NoticeReference(noticeNums[iter], noticeref);
				if(ret != 0)
				{
					asn_DEF_DisplayText.free_struct( &asn_DEF_DisplayText, expText, 1);
					free(expText); expText=NULL;
					asn_DEF_NoticeReference.free_struct( &asn_DEF_NoticeReference, noticeref, 1);
					free(noticeref);
					return BMD_ERR_OP_FAILED;
				}
			}
		}

	}


	ret=Create_SPUserNotice(noticeref, expText, &spusrnot);
	if(ret != 0)
	{
		if(expText != NULL)
		{
			asn_DEF_DisplayText.free_struct( &asn_DEF_DisplayText, expText, 1);
			free(expText); expText=NULL;
		}
		if(noticeref != NULL)
		{
			asn_DEF_NoticeReference.free_struct( &asn_DEF_NoticeReference, noticeref, 1);
			free(noticeref);
		}
		return BMD_ERR_OP_FAILED;
	}

	ret=Create_SigPolicyQualifierInfo((void*)spusrnot, BMDCMS_SP_USERNOTICE, &spqi);
	if(ret != 0)
	{
		Destroy_SPUserNotice(&spusrnot);
		return BMD_ERR_OP_FAILED;
	}
	Destroy_SPUserNotice(&spusrnot);

	ret=Add2SigPolicyQualifiers(spqi, dest->choice.signaturePolicyId.sigPolicyQualifiers);
	if(ret != 0)
	{
		asn_DEF_SigPolicyQualifierInfo.free_struct(&asn_DEF_SigPolicyQualifierInfo, spqi, 1);
		free(spqi);
		return BMD_ERR_OP_FAILED;
	}

	return 0;
}

/**
funkcja alokuje i inicjuje identyfikator polityki podpisu
@ARG SP_oid to string z oidem polityki podpisu (w formacie liczb separowanych kropkami )
@ARG hashalg_oid to OID funkcji skrotu uzywanej do wyliczenia skrutu z polityki podpisu (w formacie liczb separowanych kropkami )
@ARG params moze byc NULL, jesli nie chcemy podawac parametrow funkcji skrotu
	jesli chcemy podac te parametry, wowczas params jest przygotowana strukturka ANY ze zDERowanymi parametrami
@ARG ohv to watosc skrotu z wartosci polityki podpisu; typ OtherHashValue_t jest typedefem na OCTET_STRING_t;
	OCTET_STRING ze skrotem moze byc przygotowany za pomoca np. OCTET_STRING_fromBuf();
	Po wywolaniu tej funkcji nalezy zwolnic pamiec po ohv - mozna wykorzystać w tym celu
	funkcje Destroy_OtherHashValue()
@ARG SPId to adres wskaznika, pod ktorym zapisany zostanie stworzony identyfikator polityki podpisu
@RETURN 0 jesli ok.
	-1 jesli funkcja zawiodla
*/
long SPId_SPIdentifier_Create(char* SP_oid, char* hashalg_oid, ANY_t* params, OtherHashValue_t* ohv, SignaturePolicyIdentifier_t** SPId)
{
	long ret=0,status;
	SigPolicyQualifiers_t* spqs=NULL;
	SigPolicyId_t* sigpolid=NULL;
	SigPolicyHash_t* sigpolhash=NULL;
	SignaturePolicyId_t* signaturepolid=NULL;
	AlgorithmIdentifier_t* algid=NULL;
	OtherHashValue_t *kopia_ohv=NULL;
	ANY_t *kopia_params=NULL;

	if(SP_oid == NULL)
		{ return BMD_ERR_PARAM1; }
	if(hashalg_oid == NULL)
		{ return BMD_ERR_PARAM2; }
	/*params jest OPTIONAL i moze byc NULL*/
	if(ohv == NULL)
		{ return BMD_ERR_PARAM4; }
	if(SPId == NULL)
		{ return BMD_ERR_PARAM5; }
	if(*SPId != NULL)
		{ return BMD_ERR_PARAM5; }

	/*FIXED :tworzenie kopii ohv, aby nie niszczyć argumentu wewnatrz tej funkcji, bo to paranoja*/
	status=asn_cloneContent(&asn_DEF_OtherHashValue,ohv,(void **)&kopia_ohv);
	if( status != 0 )
		return BMD_ERR_OP_FAILED;

	if(params != NULL)
	{
		status=asn_cloneContent(&asn_DEF_ANY,params,(void **)&kopia_params);
		if( status != 0 )
		{
			asn_DEF_OtherHashValue.free_struct( &asn_DEF_OtherHashValue, kopia_ohv, 1);
			free(kopia_ohv);
			return BMD_ERR_OP_FAILED;
		}
	}


	ret=Create_SigPolicyQualifiers(&spqs);
	if(ret != 0)
	{
		asn_DEF_OtherHashValue.free_struct( &asn_DEF_OtherHashValue, kopia_ohv, 1);
		free(kopia_ohv);
		if(kopia_params != NULL)
		{
			asn_DEF_OtherHashValue.free_struct( &asn_DEF_ANY, kopia_params, 1);
			free(kopia_params);
		}
		return BMD_ERR_OP_FAILED;
	}

	ret=Create_SigPolicyId_char(SP_oid, &sigpolid);
	if(ret != 0)
	{
		free(spqs);
		asn_DEF_OtherHashValue.free_struct( &asn_DEF_OtherHashValue, kopia_ohv, 1);
		free(kopia_ohv);
		if(kopia_params != NULL)
		{
			asn_DEF_OtherHashValue.free_struct( &asn_DEF_ANY, kopia_params, 1);
			free(kopia_params);
		}
		return BMD_ERR_OP_FAILED;
	}

	ret=Create_AlgorithmIdentifier_char(hashalg_oid, kopia_params, &algid);
	if(ret != 0)
	{
		free(spqs);
		asn_DEF_OBJECT_IDENTIFIER.free_struct(&asn_DEF_OBJECT_IDENTIFIER, sigpolid, 1);
		free(sigpolid);

		asn_DEF_OtherHashValue.free_struct( &asn_DEF_OtherHashValue, kopia_ohv, 1);
		free(kopia_ohv);
		if(kopia_params != NULL)
		{
			asn_DEF_OtherHashValue.free_struct( &asn_DEF_ANY, kopia_params, 1);
			free(kopia_params);
		}
		return BMD_ERR_OP_FAILED;
	}

	ret=Create_SigPolicyHash(algid, kopia_ohv, &sigpolhash);
	if(ret != 0)
	{
		free(spqs);
		asn_DEF_OBJECT_IDENTIFIER.free_struct(&asn_DEF_OBJECT_IDENTIFIER, sigpolid, 1);
		free(sigpolid);
		asn_DEF_AlgorithmIdentifier.free_struct(&asn_DEF_AlgorithmIdentifier, algid, 1);
		free(algid);

		asn_DEF_OtherHashValue.free_struct( &asn_DEF_OtherHashValue, kopia_ohv, 1);
		free(kopia_ohv);
		/*tutaj kopia_params jest podlaczona do AlgorithmIdentifier, wiec nie jest reczni zwalniana */
		return BMD_ERR_OP_FAILED;
	}
	Destroy_AlgorithmIdentifier(&algid);
	/*zwalniane opakowanie kopia_ohv*/
	Destroy_OtherHashValue_spec(&kopia_ohv);

	ret=Create_SignaturePolicyId(sigpolid, sigpolhash, spqs, &signaturepolid);
	if(ret != 0)
	{
		free(spqs);
		asn_DEF_OBJECT_IDENTIFIER.free_struct(&asn_DEF_OBJECT_IDENTIFIER, sigpolid, 1);
		free(sigpolid);
		asn_DEF_SigPolicyHash.free_struct(&asn_DEF_SigPolicyHash, sigpolhash, 1);
		free(sigpolhash);
		/*w tym miejscu juz nie trzeba zwalniać kopia_ohv
			struktutra zostala wykorzystana i zwolniona funkcja Destroy_OtherHashValue_spec()*/
		return BMD_ERR_OP_FAILED;
	}
	Destroy_SigPolicyId(&sigpolid);
	Destroy_SigPolicyHash(&sigpolhash);

	ret=Create_SignaturePolicyIdentifier(BMDCMS_SP_ATTR, signaturepolid, SPId);
	if(ret != 0)
	{
		asn_DEF_SignaturePolicyIdentifier.free_struct(&asn_DEF_SignaturePolicyIdentifier,
			signaturepolid, 1);
		free(signaturepolid);

		return BMD_ERR_OP_FAILED;
	}
	Destroy_SignaturePolicyId(&signaturepolid);

	return 0;
}

/**
funkcja tworzy atrybut - identyfikator polityki podpisu
!!! sigPol nalezy recznie zwolnic po tej funkcji (za pomoca Destroy_SignaturePolicy)
@ARG sigPol to wskaznik do przygotowanej struktury Signature Policy
@ARG attribute to adres wskaznika do tworzonego atrybutu
@RETURN 0 w przypadku sukcesu
	-1 gdy alokacja nowego atrybutu zakonczona fiaskiem
	-2 gdy zawiodlo kodowanie do DER
*/
long SPId_SPIdentifier_CreateAttribute(SignaturePolicyIdentifier_t *sigPol, Attribute_t **attribute)
{
	long OID_SP[]={ OID_CMS_SIG_POLICY }; /*OID SignaturePolicyIdentifier*/
	Attribute_t *new_attribute=NULL;
	ANY_t *any_val=NULL;
	long status;

	if(sigPol == NULL)
		{ return BMD_ERR_PARAM1; }
	if(attribute == NULL)
		{ return BMD_ERR_PARAM2; }
	if(*attribute != NULL)
		{ return BMD_ERR_PARAM2; }

	/*alokacja z wyzerowaniem obszaru pamieci*/
	new_attribute=(Attribute_t*)calloc(1,sizeof(Attribute_t));
	if(new_attribute==NULL)
	{
		return BMD_ERR_MEMORY;
	}

	/*nadawanie OID*/
	OBJECT_IDENTIFIER_set_arcs(&(new_attribute->attrType),(const void*)OID_SP, sizeof(OID_SP[0]), 9);
	/*albo przy uzuciu funkcji Szu:
	String2OID("1.2.840.113549.1.9.16.2.14", &(new_attribute->attrType) );*/

	any_val=(ANY_t*)calloc(1, sizeof(ANY_t));
	if(any_val == NULL)
	{
		asn_DEF_OBJECT_IDENTIFIER.free_struct(&asn_DEF_OBJECT_IDENTIFIER, &(new_attribute->attrType), 1);
		free(new_attribute);
		return BMD_ERR_MEMORY;
	}

	status=ANY_fromType(any_val,&asn_DEF_SignaturePolicyIdentifier,sigPol);
	if( status != 0 )
	{
		asn_DEF_OBJECT_IDENTIFIER.free_struct(&asn_DEF_OBJECT_IDENTIFIER, &(new_attribute->attrType), 1);
		free(new_attribute);
		return BMD_ERR_OP_FAILED;
	}

	asn_set_add(&(new_attribute->attrValues.list), any_val );
	*attribute=new_attribute;

	return 0;
}

/**
funckja niszczy identyfikator polityki podpisu (zwalnia pamiec)
@ARG SP do adres wskaznika, pod ktorym zapisany jest identyfikator polityki podpisu przeznaczony do niszczenia
@RETURN funkcja nie zwraca wartosci
*/
void SPId_SPIdentifier_Destroy(SignaturePolicyIdentifier_t **SP)
{

	if(SP==NULL)
		{ return; }
	if(*SP != NULL)
	{
		asn_DEF_SignaturePolicyIdentifier.free_struct(&asn_DEF_SignaturePolicyIdentifier, *SP, 1);
		free(*SP);
		*SP=NULL;
	}

}

/**
funkcja wyodrebnia z atrybutu identyfikator polityki podpisu; wyodrebnianie przeprowadzane zostanie tylko
w sytuacji, gdy OID atrybutu swiadczy o tym, ze jest to identyfikator polityki podpisu;
Wyodrebniony identyfikator polityki podpisu musi zostac pozniej zwolniony za pomoca funkcji SPId_SPIdentifier_Destroy()
@ARG attr to wskaznik do atrybutu, z ktorego ma byc wyodrebniony identyfikator polityki podpisu
@ARG SPId to adres wskaznika, pod ktorym zapisany zostanie wyodrebniony identyfikator polityki podpisu
@RETURN 0 jesli ok.
	-1 jesli funkcja zawiodla (przekazany atrybut nie przechowuje identyfikatora polityki podpisu)
*/
long SPId_GetSPIdentifierfromAttribute(Attribute_t *attr, SignaturePolicyIdentifier_t** SPId)
{
	long oid_attr[9];
	long oid_SPId[]={ OID_CMS_SIG_POLICY };
	asn_dec_rval_t wynik;
	long arcs_num=0, iter=0;
	SignaturePolicyIdentifier_t *SPId_decoded=NULL;

	if(attr == NULL)
		{ return BMD_ERR_PARAM1; }
	if(SPId == NULL)
		{ return BMD_ERR_PARAM2; }
	if(*SPId != NULL)
		{ return BMD_ERR_PARAM2; }

	/*zwraca rzeczywista ilosc slotow;  argumenty:
	OBJECT_IDENTIFIER_t*,
	do jakiej tablicy wpisac,
	rozmiar komorki tablicy,
	ilosc slotow do odczytu */
	arcs_num=OBJECT_IDENTIFIER_get_arcs(&(attr->attrType), oid_attr, sizeof(long), 9);
	//printf("ilosc slotow: %i\n", arcs_num);
	if(arcs_num != 9)
	{
		return BMD_ERR_OP_FAILED;
	}

	for(iter=0; iter<arcs_num; iter++)
	{
		if(oid_attr[iter] != oid_SPId[iter])
		{
			return BMD_ERR_OP_FAILED;
		}
	}
	/*w tym miejscu wiadomo, ze oid sie zgadza i mozna decodowac*/

	/*jesli mimo oidu brakuje wartosci identyfikatora polityki podpisu*/
	if(attr->attrValues.list.count==0)
	{
		return BMD_ERR_OP_FAILED;
	}

	SPId_decoded=(SignaturePolicyIdentifier_t*)calloc(1, sizeof(SignaturePolicyIdentifier_t));
	if(SPId_decoded == NULL)
	{
		return BMD_ERR_MEMORY;
	}

	wynik=ber_decode(0, &asn_DEF_SignaturePolicyIdentifier, (void**)&SPId_decoded, (attr->attrValues.list.array[0])->buf, (attr->attrValues.list.array[0])->size );
	/*jesli dekodowanie zakonczone niepowodzeniem*/

	if(wynik.code != RC_OK)
	{
		asn_DEF_SignaturePolicyIdentifier.free_struct(&asn_DEF_SignaturePolicyIdentifier, SPId_decoded, 1);
		free(SPId_decoded);
		return BMD_ERR_OP_FAILED;
	}

	*SPId=SPId_decoded;
	return 0;
}


/**
funkcja wyodrebnia z atrybutow podpisanych identyfikator polityki podpisu; wyodrebnianie przeprowadzane zostanie tylko
w sytuacji, gdy OID jednego z atrybutow swiadczy o tym, ze jest to identyfikator polityki podpisu;
Wyodrebniony identyfikator polityki podpisu musi zostac pozniej zwolniony za pomoca funkcji SPId_SPIdentifier_Destroy()
@ARG sigattr to wskaznik do atrybutow podpisanych, z ktorych ma byc wyodrebniony identyfikator polityki podpisu
@ARG SPId to adres wskaznika, pod ktorym zapisany zostanie wyodrebniony identyfikator polityki podpisu
@RETURN 0 jesli ok.
	-1 jesli funkcja zawiodla (w przekazanych atrybutach nie ma identyfikatora polityki podpisu)
*/
long SPId_GetSPIdentifierfromSignedAttributes(SignedAttributes_t* sigattr, SignaturePolicyIdentifier_t **SPId)
{
	long iter=0, ret=0;

	if(sigattr == NULL)
		{ return BMD_ERR_PARAM1; }
	if(SPId == NULL)
		{ return BMD_ERR_PARAM2; }
	if(*SPId != NULL)
		{ return BMD_ERR_PARAM2; }

	for(iter=0; iter<sigattr->list.count; iter++)
	{
		ret=SPId_GetSPIdentifierfromAttribute(sigattr->list.array[iter], SPId);
		if(ret==0)
		{
			return 0;
		}
	}
	return BMD_ERR_OP_FAILED;
}


/**
na wszelki wypadek
funkcja niszczy atrybut z identyfikatorem polityki podpisu (zwalnia pamiec)
@ARG attr do adres wskaznika, pod ktorym zapisany jest atrybut przeznaczony do niszczenia
@RETURN 0 jesli niszczenie powiodlo sie
	-1 jesli przekazany atrybut nie zawiera identyfikatora polityki podpisu (wowczas niszczenie nie jest przeprowadzone)

*/
long SPId_SPIdentifierAttribute_Destroy(Attribute_t **attr)
{
	long oid_attr[9];
	long oid_SPId[]={ OID_CMS_SIG_POLICY };
	long arcs_num=0, iter=0;
	Attribute_t *attr_wsk=*attr;

	if(attr == NULL)
		{ return BMD_ERR_PARAM1; }
	if(*attr == NULL)
		{ return BMD_ERR_PARAM1; }

	/*zwraca rzeczywista ilosc slotow;  argumenty:
	OBJECT_IDENTIFIER_t*,
	do jakiej tablicy wpisac,
	rozmiar komorki tablicy,
	ilosc slotow do odczytu */
	arcs_num=OBJECT_IDENTIFIER_get_arcs(&(attr_wsk->attrType), oid_attr, sizeof(long), 9);
	if(arcs_num != 9)
	{
		return BMD_ERR_OP_FAILED;
	}

	for(iter=0; iter<arcs_num; iter++)
	{
		if(oid_attr[iter] != oid_SPId[iter])
		{
			return BMD_ERR_OP_FAILED;
		}
	}
	/*w tym miejscu wiadomo, ze oid sie zgadza i mozna decodowac*/

	asn_DEF_Attribute.free_struct(&asn_DEF_Attribute, attr_wsk, 1);
	free(attr_wsk);
	*attr=NULL;

	return 0;
}

/**
funckja dekoduje polityke podpisu do truktury SignaturePolicy_t (alokuje takze SignaturePolicy_t )
@ARG src_der W tym GenBufie znajduje sie polityka podpisu zakodowana w der
@ARG SP jest to adres wskaznika pod ktorym zapisana zostanie zdekodowana polityka podpisu
@RETURN 0 jesli poprawnie zdekodowana
	-1 jesli dekodowanie nie powiodlo sie
*/
long Decode_spol(GenBuf_t *src_der, SignaturePolicy_t **SP)
{
	asn_dec_rval_t wynik;

	if(src_der == NULL)
		{ return BMD_ERR_PARAM1; }
	if(src_der->buf == NULL)
		{ return BMD_ERR_PARAM1; }
	if(SP == NULL)
		{ return BMD_ERR_PARAM2; }
	/*dla porzadku*/
	if(*SP != NULL)
		{ return BMD_ERR_PARAM2; }

	*SP=(SignaturePolicy_t*)calloc(1, sizeof(SignaturePolicy_t));
	if(*SP == NULL)
		{ return BMD_ERR_MEMORY; }

	wynik=ber_decode(0, &asn_DEF_SignaturePolicy, (void**)SP, src_der->buf, src_der->size);

	/*jesli dekodowanie zakonczone niepowodzeniem*/
	if(wynik.code != RC_OK)
	{
		asn_DEF_SignaturePolicy.free_struct(&asn_DEF_SignaturePolicy,*SP,1);
		free(*SP);
		*SP=NULL;
		return BMD_ERR_OP_FAILED;
	}
	return 0;
}

/**
funkcja niszczy polityke podpisu i zwalnia pamiec
@ARG SP to adres wskaznika do polityki podpisu, ktora ma zostac zniszczona
@RETURN nie zwraca zadnej wartosci
*/
void Destroy_spol(SignaturePolicy_t **SP)
{
	if(SP == NULL)
		{ return; }

	if(*SP != NULL)
	{
		asn_DEF_SignaturePolicy.free_struct(&asn_DEF_SignaturePolicy,*SP,1);
		free(*SP);
		*SP=NULL;
	}

}

/**------------------------------------------------------------------------------------*/
/**------------------------------------------------------------------------------------*/
/**------------------------------------------------------------------------------------*/

/**tutaj zaczyna sie API do atrybutow wymaganych w podpisie kwalifikowanym*/

/*funkcja alokuje pamiec i tworzy atrybut podpisany SigningTime*/
long SigAttr_Create_SigningTime(Attribute_t **sigtime_attr)
{
	time_t time_czas;
	struct tm *tm_czas=NULL;
	GeneralizedTime_t *gentime=NULL;
	SigningTime_t *sigtime=NULL;
	Attribute_t *attr=NULL;
	OBJECT_IDENTIFIER_t *oid=NULL;
	ANY_t *der=NULL;
	long status=0;
	long sigtime_oid_tab[]= { SIGNING_TIME_OID };

	if(sigtime_attr == NULL)
		{ return BMD_ERR_PARAM1; }
	if(*sigtime_attr != NULL)
		{ return BMD_ERR_PARAM1; }

	time(&time_czas);
	//wstawiany czas powinien być wyrazony w UTC... localtime pod Win uwzglednia przesuniecie a to niedobrze
	//przesuniecie ma byc dopasowywane na danje stacji (przy przegladaniu podpisu)
	//zgodnie z jej ustawieniem strefy
	//tm_czas=localtime(&time_czas);
	//gmtime daje czysty UTC
	tm_czas=gmtime(&time_czas);

	if(tm_czas==NULL)
	{
		return BMD_ERR_OP_FAILED;
	}


	gentime=(GeneralizedTime_t*)calloc(1, sizeof(GeneralizedTime_t));
	if(gentime==NULL)
	{
		return BMD_ERR_MEMORY;
	}
	if( (asn_time2GT(gentime, (const struct tm *)tm_czas, 0)) == NULL)
	{
		free(gentime); gentime=NULL;
		return BMD_ERR_OP_FAILED;
	}

	sigtime=(SigningTime_t*)calloc(1, sizeof(SigningTime_t));
	if(sigtime==NULL)
	{
		asn_DEF_GeneralizedTime.free_struct( &asn_DEF_GeneralizedTime, gentime, 1);
		free(gentime); gentime=NULL;
		return BMD_ERR_MEMORY;
	}
	sigtime->present=Time_PR_generalTime;
	sigtime->choice.generalTime=*gentime; /*przypisywana jest zawartosc gentime*/
	free(gentime); gentime=NULL; /*opakowanie gentime moze zostac zwolnione*/

	/*opakowywanie SigningTime w atrybut*/
	attr=(Attribute_t *)calloc(1, sizeof(Attribute_t));
	if(attr==NULL)
	{
		/*opakowanie gentime jest juz zwolnione a zawartosc przypisana wewnatrz sigtime*/
		asn_DEF_SigningTime.free_struct( &asn_DEF_SigningTime, sigtime, 1);
		free(sigtime); sigtime=NULL;
		return BMD_ERR_MEMORY;
	}
	oid=(OBJECT_IDENTIFIER_t*)calloc(1, sizeof(OBJECT_IDENTIFIER_t));
	if(oid==NULL)
	{
		asn_DEF_SigningTime.free_struct( &asn_DEF_SigningTime, sigtime, 1);
		free(sigtime); sigtime=NULL;
		free(attr); attr=NULL;
		return BMD_ERR_MEMORY;
	}
	status=OBJECT_IDENTIFIER_set_arcs(oid, sigtime_oid_tab, sizeof(sigtime_oid_tab[0]), 7);
	if(status != 0)
	{
		asn_DEF_SigningTime.free_struct( &asn_DEF_SigningTime, sigtime, 1);
		free(sigtime); sigtime=NULL;
		free(attr); attr=NULL;
		free(oid); oid=NULL;
		return BMD_ERR_OP_FAILED;
	}
	attr->attrType=*oid; /*przypisanie zawartosci oid*/
	free(oid); oid=NULL; /*zwalniane tylko opakowanie (bebechy przechodza dalej)*/

	/*derowanie SigningTime*/
	der=(ANY_t*)calloc(1, sizeof(ANY_t));
	if(der==NULL)
	{
		asn_DEF_SigningTime.free_struct( &asn_DEF_SigningTime, sigtime, 1);
		free(sigtime); sigtime=NULL;
		/*opakowanie oid jest juz zwolnione a zawartosc przypisana wewnatrz attr*/
		asn_DEF_Attribute.free_struct( &asn_DEF_Attribute, attr, 1);
		free(attr); attr=NULL;
		return BMD_ERR_MEMORY;
	}
	status=ANY_fromType(der,&asn_DEF_SigningTime,sigtime);
	if( status != 0 )
	{
		asn_DEF_SigningTime.free_struct( &asn_DEF_SigningTime, sigtime, 1);
		free(sigtime); sigtime=NULL;
		/*opakowanie oid jest juz zwolnione a zawartosc przypisana wewnatrz attr*/
		asn_DEF_Attribute.free_struct( &asn_DEF_Attribute, attr, 1);
		free(attr); attr=NULL;
		return BMD_ERR_OP_FAILED;
	}
	/* SigningTime jest juz zderowany, wiec mozna zniszczyc*/
	asn_DEF_SigningTime.free_struct( &asn_DEF_SigningTime, sigtime, 1);
	free(sigtime); sigtime=NULL;

	if( (asn_set_add((void*)&(attr->attrValues.list), (void*)der)) != 0)
	{
		asn_DEF_Attribute.free_struct( &asn_DEF_Attribute, attr, 1);
		free(attr); attr=NULL;
		asn_DEF_ANY.free_struct(&asn_DEF_ANY, der, 1);
		free(der); der=NULL;
		return BMD_ERR_OP_FAILED;
	}

	*sigtime_attr=attr;
	return 0;
}


/*
cert_hash to bufor zawierajacy hash certyfikatu
cert to struktura Certificate_t
*/
long SigAttr_Create_SigningCertificate(GenBuf_t *cert_hash, Certificate_t *cert, Attribute_t **sigcert_attr)
{
	long status=0;
	Hash_t *hash=NULL;
	ESSCertID_t *ESS=NULL;
	CertificateSerialNumber_t *sn=NULL;
	Name_t *issuer=NULL;
	IssuerSerial_t *issuer_serial=NULL;
	GeneralNames_t *general_list=NULL;
	GeneralName_t *general_name=NULL;
	SigningCertificate_t *signingCert=NULL;
	Attribute_t *attr=NULL;
	OBJECT_IDENTIFIER_t *oid=NULL;
	ANY_t *der=NULL;
	long sigcert_oid_tab[]= { SIGNING_CERT_OID };

	if(cert_hash == NULL)
		{ return BMD_ERR_PARAM1; }
	if(cert_hash->buf == NULL)
		{ return BMD_ERR_PARAM1; }
	if(cert == NULL)
		{ return BMD_ERR_PARAM2; }
	if(sigcert_attr == NULL)
		{ return BMD_ERR_PARAM3; }
	if(*sigcert_attr != NULL)
		{ return BMD_ERR_PARAM3; }

	/*Hash_t jest typedefem na OCTET_STRING_t*/
	hash=OCTET_STRING_new_fromBuf(&asn_DEF_OCTET_STRING, (const char*)cert_hash->buf, cert_hash->size);
	if(hash == NULL)
	{
		return BMD_ERR_OP_FAILED;
	}

	ESS=(ESSCertID_t*)calloc(1, sizeof(ESSCertID_t));
	if(ESS == NULL)
	{
		asn_DEF_Hash.free_struct(&asn_DEF_Hash, hash, 1);
		free(hash); hash=NULL;
		return BMD_ERR_MEMORY;
	}
	ESS->certHash=*hash;
	free(hash); hash=NULL; /*opakowanie moze zostac zwolnione (zawartosc jest przypis. do wnetrza ESS)*/

	/*klonowanie seriala certyfikatu z Certificate_t*/
	status=asn_cloneContent( &asn_DEF_CertificateSerialNumber, &(cert->tbsCertificate.serialNumber), (void **)&(sn));
	if(status != 0)
	{
		/*opakowanie hash jest zwonione a zawartosc jest wewnatrz ESS*/
		asn_DEF_ESSCertID.free_struct(&asn_DEF_ESSCertID, ESS, 1);
		free(ESS); ESS=NULL;
		/*z mojego punktu widzenia asn_cloneContent powinna sprzatac po sobie,
			tak wiec nie sprzatam nadgorliwie*/
		return BMD_ERR_OP_FAILED;
	}

	status=asn_cloneContent( &asn_DEF_Name, &(cert->tbsCertificate.issuer), (void **)&(issuer));
	if(status != 0)
	{
		asn_DEF_ESSCertID.free_struct(&asn_DEF_ESSCertID, ESS, 1);
		free(ESS); ESS=NULL;
		asn_DEF_CertificateSerialNumber.free_struct(&asn_DEF_CertificateSerialNumber, sn, 1);
		free(sn); sn=NULL;
		return BMD_ERR_OP_FAILED;
	}

	general_name=(GeneralName_t*)calloc(1, sizeof(GeneralName_t));
	if(general_name == NULL)
	{
		asn_DEF_ESSCertID.free_struct(&asn_DEF_ESSCertID, ESS, 1);
		free(ESS); ESS=NULL;
		asn_DEF_CertificateSerialNumber.free_struct(&asn_DEF_CertificateSerialNumber, sn, 1);
		free(sn); sn=NULL;
		asn_DEF_Name.free_struct(&asn_DEF_Name, issuer, 1);
		free(issuer); issuer=NULL;
		return BMD_ERR_MEMORY;
	}

	general_name->present=GeneralName_PR_directoryName;
	general_name->choice.directoryName=*issuer;
	free(issuer); issuer=NULL; /*opakowanie mozna zwolnic (bebechy przypisane wewnatrz general_name)*/

	general_list=(GeneralNames_t*)calloc(1, sizeof(GeneralNames_t));
	if(general_list == NULL)
	{
		asn_DEF_ESSCertID.free_struct(&asn_DEF_ESSCertID, ESS, 1);
		free(ESS); ESS=NULL;
		asn_DEF_CertificateSerialNumber.free_struct(&asn_DEF_CertificateSerialNumber, sn, 1);
		free(sn); sn=NULL;
		/*opakowanie issuer zostalo zwolnione a bebechy odpowiednio przypisane do general_name*/
		asn_DEF_GeneralName.free_struct(&asn_DEF_GeneralName, general_name, 1);
		free(general_name); general_name=NULL;
		return BMD_ERR_MEMORY;
	}

	/*binduje wskaznik, wiec juz z general_name nic nie trzeba juz robic*/
	if( (asn_set_add((void*)&(general_list->list), (void*)general_name)) != 0 )
	{
		asn_DEF_ESSCertID.free_struct(&asn_DEF_ESSCertID, ESS, 1);
		free(ESS); ESS=NULL;
		asn_DEF_CertificateSerialNumber.free_struct(&asn_DEF_CertificateSerialNumber, sn, 1);
		free(sn); sn=NULL;
		asn_DEF_GeneralName.free_struct(&asn_DEF_GeneralName, general_name, 1);
		free(general_name); general_name=NULL;
		free(general_list); general_list=NULL;
		return BMD_ERR_OP_FAILED;
	}

	issuer_serial=(IssuerSerial_t*)calloc(1, sizeof(IssuerSerial_t));
	if(issuer_serial == NULL)
	{
		asn_DEF_ESSCertID.free_struct(&asn_DEF_ESSCertID, ESS, 1);
		free(ESS); ESS=NULL;
		asn_DEF_CertificateSerialNumber.free_struct(&asn_DEF_CertificateSerialNumber, sn, 1);
		free(sn); sn=NULL;
		asn_DEF_GeneralNames.free_struct(&asn_DEF_GeneralNames, general_list, 1);
		free(general_list); general_list=NULL;
		return BMD_ERR_MEMORY;
	}

	issuer_serial->issuer=*general_list;
	free(general_list); general_list=NULL; /*opakowanie mozna zwolnic, bebechy odpowiendio przypisane*/

	issuer_serial->serial=*sn;
	free(sn); sn=NULL; /*opakowanie mozna zwolnic, bebechy odpowiendio przypisane*/

	/*binduje wskazniki*/
	ESS->issuerSerial=issuer_serial;

	signingCert=(SigningCertificate_t*)calloc(1, sizeof(SigningCertificate_t));
	if(signingCert==NULL)
	{
		asn_DEF_ESSCertID.free_struct(&asn_DEF_ESSCertID, ESS, 1);
		free(ESS); ESS=NULL;
		return BMD_ERR_MEMORY;
	}
	/*bindowanie wskaznika */
	if( (asn_set_add((void *)&(signingCert->certs.list), (void *)ESS)) != 0 )
	{
		asn_DEF_ESSCertID.free_struct(&asn_DEF_ESSCertID, ESS, 1);
		free(ESS); ESS=NULL;
		free(signingCert); signingCert=NULL;
		return BMD_ERR_OP_FAILED;
	}

	/*opakowywanie w atrybut*/
	attr=(Attribute_t*)calloc(1,sizeof(Attribute_t));
	if(attr==NULL)
	{
		asn_DEF_SigningCertificate.free_struct(&asn_DEF_SigningCertificate, signingCert, 1);
		free(signingCert); signingCert=NULL;
		return BMD_ERR_MEMORY;
	}

	oid=(OBJECT_IDENTIFIER_t*)calloc(1, sizeof(OBJECT_IDENTIFIER_t));
	if(oid == NULL)
	{
		asn_DEF_SigningCertificate.free_struct(&asn_DEF_SigningCertificate, signingCert, 1);
		free(signingCert); signingCert=NULL;
		free(attr); attr=NULL;
		return BMD_ERR_MEMORY;
	}

	status=OBJECT_IDENTIFIER_set_arcs(oid, sigcert_oid_tab, sizeof(sigcert_oid_tab[0]), 9);
	if(status != 0)
	{
		asn_DEF_SigningCertificate.free_struct(&asn_DEF_SigningCertificate, signingCert, 1);
		free(signingCert); signingCert=NULL;
		free(attr); attr=NULL;
		free(oid); oid=NULL;
		return BMD_ERR_OP_FAILED;
	}

	attr->attrType=*oid;
	free(oid); oid=NULL; /*zwalniane tylko opakowanie (bebechy przechodza dalej)*/


	/*derowanie SigningCertificate*/
	der=(ANY_t*)calloc(1, sizeof(ANY_t));
	if(der == NULL)
	{
		asn_DEF_SigningCertificate.free_struct(&asn_DEF_SigningCertificate, signingCert, 1);
		free(signingCert); signingCert=NULL;
		asn_DEF_Attribute.free_struct(&asn_DEF_Attribute, attr, 1);
		free(attr); attr=NULL;
		return BMD_ERR_MEMORY;
	}
	status=ANY_fromType(der,&asn_DEF_SigningCertificate,signingCert);
	if( status != 0 )
	{
		asn_DEF_SigningCertificate.free_struct(&asn_DEF_SigningCertificate, signingCert, 1);
		free(signingCert); signingCert=NULL;
		asn_DEF_Attribute.free_struct(&asn_DEF_Attribute, attr, 1);
		free(attr); attr=NULL;
		return BMD_ERR_OP_FAILED;
	}

	/* SigningCertificate jest juz zderowany, wiec mozna zniszczyc*/
	asn_DEF_SigningCertificate.free_struct(&asn_DEF_SigningCertificate, signingCert, 1);
	free(signingCert); signingCert=NULL;

	/*bindowanie wskaznika der z atrybutem*/
	if( (asn_set_add((void*)&(attr->attrValues.list), (void*)der)) != 0)
	{
		asn_DEF_Attribute.free_struct(&asn_DEF_Attribute, attr, 1);
		free(attr); attr=NULL;
		asn_DEF_ANY.free_struct(&asn_DEF_ANY, der, 1);
		free(der); der=NULL;
		return BMD_ERR_OP_FAILED;
	}

	*sigcert_attr=attr;
	return 0;
}



/*
wrapperek umozliwiajacy stworzenie identyfikatora polityki podpisu z jednym uri wskazujacym
docelowa polityke podpisu (hash z polityki podpisu wyliczany jest zawsze funkcja sha1)
*/

long SigAttr_Create_SignaturePolicyId(char *spol_oid, char *uri, GenBuf_t *spol_hash, Attribute_t **sigpolid_attr)
{
	OtherHashValue_t *skrot=NULL;
	SignaturePolicyIdentifier_t *spol_id=NULL;
	long status=0;

	if(spol_oid == NULL)
		{ return BMD_ERR_PARAM1; }
	if(uri == NULL)
		{ return BMD_ERR_PARAM2; }
	if(spol_hash == NULL)
		{ return BMD_ERR_PARAM3; }
	if(spol_hash->buf == NULL)
		{ return BMD_ERR_PARAM3; }
	if(sigpolid_attr == NULL)
		{ return BMD_ERR_PARAM4; }
	if(*sigpolid_attr != NULL)
		{ return BMD_ERR_PARAM4; }

	//OtherHashValue_t to typedef na OCTET_STRING_t
	skrot=OCTET_STRING_new_fromBuf(&asn_DEF_OCTET_STRING, (const char*)spol_hash->buf, spol_hash->size);
	if(skrot == NULL)
	{
		return BMD_ERR_OP_FAILED;
	}

	status=SPId_SPIdentifier_Create(spol_oid, "1.3.14.3.2.26", NULL, skrot, &spol_id);
	if( status != 0)
	{
		asn_DEF_OCTET_STRING.free_struct(&asn_DEF_OCTET_STRING, skrot, 1);
		free(skrot); skrot=NULL;
		return BMD_ERR_OP_FAILED;
	}
	Destroy_OtherHashValue(&skrot);
	status=SPId_Uri_Add(uri, spol_id);
	if( status != 0)
	{
		SPId_SPIdentifier_Destroy(&spol_id);
		return BMD_ERR_OP_FAILED;
	}
	status=SPId_SPIdentifier_CreateAttribute(spol_id, sigpolid_attr);
	if(status != 0)
	{
		SPId_SPIdentifier_Destroy(&spol_id);
		return BMD_ERR_OP_FAILED;
	}

	SPId_SPIdentifier_Destroy(&spol_id);
	return 0;
}


long SignedAtributes_init(SignedAttributes_t **signed_attrs)
{
	if(signed_attrs == NULL)
		{ return BMD_ERR_PARAM1; }
	if(*signed_attrs != NULL)
		{ return BMD_ERR_PARAM1; }

	*signed_attrs=(SignedAttributes_t*)calloc(1, sizeof(SignedAttributes_t));
	if(*signed_attrs == NULL)
		{ return BMD_ERR_MEMORY; }
	else
		{ return 0; }
}

long SignedAtributes_add(Attribute_t *new_attr, SignedAttributes_t *signed_attrs)
{
	if(new_attr == NULL)
		{ return BMD_ERR_PARAM1; }
	if(signed_attrs == NULL)
		{ return BMD_ERR_PARAM2; }

	if( (asn_set_add( (void*)&(signed_attrs->list), new_attr)) == 0)
		{ return 0; }
	else
		{ return BMD_ERR_OP_FAILED; }
}

long UnsignedAtributes_init(UnsignedAttributes_t **unsigned_attrs)
{
	if(unsigned_attrs == NULL)
		{ return BMD_ERR_PARAM1; }
	if(*unsigned_attrs != NULL)
		{ return BMD_ERR_PARAM1; }

	*unsigned_attrs=(UnsignedAttributes_t*)calloc(1, sizeof(UnsignedAttributes_t));
	if(*unsigned_attrs == NULL)
		{ return BMD_ERR_MEMORY; }
	else
		{ return 0; }
}

long UnsignedAtributes_add(Attribute_t *new_attr, UnsignedAttributes_t *unsigned_attrs)
{
	if(new_attr == NULL)
		{ return BMD_ERR_PARAM1; }
	if(unsigned_attrs == NULL)
		{ return BMD_ERR_PARAM2; }

	if( (asn_set_add( (void*)&(unsigned_attrs->list), new_attr)) == 0 )
		{ return 0; }
	else
		{ return BMD_ERR_OP_FAILED; }
}

long SigningParams_Create(SignedAttributes_t *sig_attrs, UnsignedAttributes_t *unsig_attrs, bmd_signature_params_t **params)
{
	bmd_signature_params_t *par=NULL;

	if(params == NULL)
		{ return BMD_ERR_PARAM3; }
	if(*params != NULL)
		{ return BMD_ERR_PARAM3; }

	par=(bmd_signature_params_t *)calloc(1, sizeof(bmd_signature_params_t));
	if(par == NULL)
	{
		return BMD_ERR_MEMORY;
	}
	par->signature_flags=BMD_CMS_SIG_INTERNAL;
	par->signed_attributes=sig_attrs;
	par->unsigned_attributes=unsig_attrs;
	*params=par;
	return 0;
}

void SigningParams_Destroy(bmd_signature_params_t **params)
{
	bmd_signature_params_t *par=NULL;

	if(params == NULL)
	{ return; }
	if(*params == NULL )
	{ return; }

	par=*params;

	if(par->signed_attributes != NULL)
	{
		asn_DEF_SignedAttributes.free_struct(&asn_DEF_SignedAttributes, par->signed_attributes, 1);
		free(par->signed_attributes); par->signed_attributes=NULL;
	}
	if(par->unsigned_attributes != NULL)
	{
		asn_DEF_UnsignedAttributes.free_struct(&asn_DEF_UnsignedAttributes, par->unsigned_attributes, 1);
		free(par->unsigned_attributes); par->unsigned_attributes=NULL;
	}
	free(par);
	*params=NULL;
}

/*
cert_num ustawiac najlepiej jako 0
dla contextu z pliku wartosc jest ignorowana
dla PKCS11 okresla numer certyfikatu (dla wielu czytnikow)- numeracja od 0;*/
long Certificate_from_ctx(bmd_crypt_ctx_t *ctx, Certificate_t **cert, long cert_num)
{
	asn_dec_rval_t dec_result;

	if(ctx == NULL)
		{ return BMD_ERR_PARAM1; }
	if(cert == NULL)
		{ return BMD_ERR_PARAM2; }
	if(*cert != NULL)
		{ return BMD_ERR_PARAM2; }


	if(ctx->ctx_source == BMD_CTX_SOURCE_FILE)
	{
		if(ctx->file != NULL)
		{
			if(ctx->file->cert != NULL)
			{
				if(ctx->file->cert->buf != NULL)
				{
					/*dekodowane Certificate_t*/
					dec_result=ber_decode(0, &asn_DEF_Certificate, (void**)cert,
					ctx->file->cert->buf, ctx->file->cert->size);
					if(dec_result.code != RC_OK)
					{
						return BMD_ERR_OP_FAILED;
					}
					return BMD_OK;
				}
				return BMD_ERR_PARAM1;
			}
			return BMD_ERR_PARAM1;
		}
		return BMD_ERR_PARAM1;
	}
	else
	{
		if(ctx->ctx_source == BMD_CTX_SOURCE_PKCS11)
		{
			if(ctx->pkcs11 != NULL)
			{
				if(ctx->pkcs11->all_certs != NULL)
				{
					if(ctx->pkcs11->all_certs[cert_num]->gb_cert != NULL)
					{
						if(ctx->pkcs11->all_certs[cert_num]->gb_cert->buf != NULL)
						{
							/*dekodowane Certificate_t*/
							dec_result=ber_decode(0, &asn_DEF_Certificate, (void**)cert, ctx->pkcs11->all_certs[cert_num]->gb_cert->buf, ctx->pkcs11->all_certs[cert_num]->gb_cert->size);
							if(dec_result.code != RC_OK)
								{ return BMD_ERR_OP_FAILED; }
							return BMD_OK;
						}
						return BMD_ERR_PARAM1;
					}
					return BMD_ERR_PARAM1;
				}
				return BMD_ERR_PARAM1;
			}
			return BMD_ERR_PARAM1;
		}
		else
		{
			return BMD_ERR_UNIMPLEMENTED;
		}
	}
}

void Destroy_Certificate(Certificate_t **cert)
{
	if(cert != NULL)
	{
		if(*cert != NULL)
		{
			asn_DEF_Certificate.free_struct(&asn_DEF_Certificate, *cert, 1);
			free(*cert); *cert=NULL;
		}
	}
}

/*zwraca wskaznik na GenBuf_t z certyfikatem znajdujacy sie w kontekscie*/
long certPtr_from_ctx(bmd_crypt_ctx_t *ctx, GenBuf_t **cert, long cert_num)
{
	if(ctx == NULL)
		{ return BMD_ERR_PARAM1; }
	if(cert == NULL)
		{ return BMD_ERR_PARAM2; }
	if(*cert != NULL)
		{ return BMD_ERR_PARAM2; }


	if(ctx->ctx_source == BMD_CTX_SOURCE_FILE)
	{
		if(ctx->file != NULL)
		{
			*cert=ctx->file->cert;
			return BMD_OK;
		}
		return BMD_ERR_PARAM1;
	}
	else
	{
		if(ctx->ctx_source == BMD_CTX_SOURCE_PKCS11)
		{
			if(ctx->pkcs11 != NULL)
			{
				if(ctx->pkcs11->all_certs != NULL)
				{
					*cert=ctx->pkcs11->all_certs[cert_num]->gb_cert;
					return BMD_OK;
				}
				return BMD_ERR_PARAM1;
			}
			return BMD_ERR_PARAM1;
		}
		else
		{
			return BMD_ERR_UNIMPLEMENTED;
		}
	}
}




//TODO porownywanie skrotow (wyslany i zwrocony w odpowiedzi znacznika - message imprint)
/*na wejsciu pobiera GenBuf z odpowiedzia znacznika czasu i tworzy atrybut podpisany contentTimestamp
operacja zostanie przerwana, jesli nie zgadzaja sie skroty wyslany - otrzymany w odpowiedzi TS
*/
long SigAttr_Create_ContentTimestamp(GenBuf_t *ts_resp, GenBuf_t *sent_hash, Attribute_t **attr)
{
	long oid_contentTS[]= { OID_CONTENT_TIMESTAMP };
	Attribute_t *new_attr=NULL;
	ANY_t *any=NULL;
	long ret_val=0;
	asn_dec_rval_t dec_result;
	TimeStampResp_t *resp=NULL;
	TSRespFeatures_t *features=NULL;
	long status=0;


	//inicjalizacja konteksu
	if(ts_resp == NULL)
		{ return BMD_ERR_PARAM1; }
	if(ts_resp->buf == NULL)
		{ return BMD_ERR_PARAM1; }
	if(attr == NULL)
		{ return BMD_ERR_PARAM3; }
	if(*attr != NULL)
		{ return BMD_ERR_PARAM3; }
	//sent_hash moze byc NULL

	ret_val=Get_GenBuf_status(ts_resp, &status);
	if(ret_val != BMD_OK || status>1)
	{
		return BMD_ERR_OP_FAILED;
	}

	if(sent_hash != NULL)
	{//
	//sprawdzanie,czy prawidlowy znacznik czasu
		ret_val=Parse_TimeStampResponse(ts_resp, &features);
		if(ret_val != BMD_OK)
			{ return BMD_ERR_OP_FAILED; }
		if(features->hash_val->size == sent_hash->size)
		{
			//porownanie skrotu wysylaniego i otrzymanego w odpowiedzi znacznika
			ret_val=memcmp(features->hash_val->buf, sent_hash->buf, sent_hash->size);
			Destroy_TSRespFeatures(&features);
			if(ret_val != 0)
				{ return BMD_ERR_OP_FAILED; }
		}
		else
		{
			Destroy_TSRespFeatures(&features);
			return BMD_ERR_OP_FAILED;
		}
	//koniec sprawdzania prawidlowosci
	}//

	//alokowanie atrybutu
	new_attr=(Attribute_t*)calloc(1, sizeof(Attribute_t));
	if(new_attr == NULL)
		{ return BMD_ERR_MEMORY; }
	//ustawianie oidu
	ret_val=OBJECT_IDENTIFIER_set_arcs( &(new_attr->attrType), oid_contentTS, sizeof(oid_contentTS[0]), sizeof(oid_contentTS)/sizeof(oid_contentTS[0]));
	if(ret_val != 0 )
	{
		free(new_attr);
		return BMD_ERR_OP_FAILED;
	}

	dec_result=ber_decode(0, &asn_DEF_TimeStampResp, (void**)&resp, ts_resp->buf, ts_resp->size);
	if(dec_result.code != RC_OK)
	{
		asn_DEF_Attribute.free_struct(&asn_DEF_Attribute, new_attr, 1);
		free(new_attr);
		return BMD_ERR_OP_FAILED;
	}
	//alokowanie any na DER z TimeStampToken
	any=(ANY_t*)calloc(1, sizeof(ANY_t));
	if(any == NULL)
	{
		asn_DEF_TimeStampResp.free_struct(&asn_DEF_TimeStampResp, resp, 1);
		free(resp);
		asn_DEF_Attribute.free_struct(&asn_DEF_Attribute, new_attr, 1);
		free(new_attr);
		return BMD_ERR_OP_FAILED;
	}
	ret_val=ANY_fromType(any, &asn_DEF_TimeStampToken, resp->timeStampToken);
	//timestamptoken nie jest juz potrzze
	asn_DEF_TimeStampResp.free_struct(&asn_DEF_TimeStampResp, resp, 1);
	free(resp);
	if(ret_val != 0)
	{
		asn_DEF_Attribute.free_struct(&asn_DEF_Attribute, new_attr, 1);
		free(new_attr);
		return BMD_ERR_OP_FAILED;
	}

	if( (asn_set_add((void*)&(new_attr->attrValues.list), (void*)any)) != 0)
	{
		asn_DEF_ANY.free_struct(&asn_DEF_ANY, any, 1);
		free(any);
		asn_DEF_Attribute.free_struct(&asn_DEF_Attribute, new_attr, 1);
		free(new_attr);
		return BMD_ERR_OP_FAILED;
	}

	*attr=new_attr;
	return BMD_OK;
}

/*jesli nie udalo sie stworzyc SigningParams, to za pomoca tej funkcji mozna zniszczyc SignedAttributes
	w ramach obslugi bledow*/
long SignedAttributes_destroy(SignedAttributes_t **sig_attrs)
{
	if(sig_attrs != NULL)
	{
		if(*sig_attrs != NULL)
		{
			asn_DEF_SignedAttributes.free_struct(&asn_DEF_SignedAttributes, *sig_attrs, 1);
			free(*sig_attrs); *sig_attrs=NULL;
		}
	}
	return 0;
}

/*jesli nie udalo sie stworzyc SigningParams, to za pomoca tej funkcji mozna zniszczyc UnsignedAttributes
	w ramach obslugi bledow*/
long UnsignedAttributes_destroy(UnsignedAttributes_t **unsig_attrs)
{
	if(unsig_attrs != NULL)
	{
		if(*unsig_attrs != NULL)
		{
			asn_DEF_UnsignedAttributes.free_struct(&asn_DEF_UnsignedAttributes, *unsig_attrs, 1);
			free(*unsig_attrs); *unsig_attrs=NULL;
		}
	}
	return 0;
}

/*jesli nie udalo sie dodac atrybutu do SignedAttributes badz UnsignedAttributes, to za pomoca tej funkcji
	mozna zniszczyc atrybut w ramach obsugi bledow*/
long Attribute_destroy(Attribute_t **attr)
{
	if(attr != NULL)
	{
		if(*attr != NULL)
		{
			asn_DEF_Attribute.free_struct(&asn_DEF_Attribute, *attr, 1);
			free(*attr); *attr=NULL;
		}
	}
	return 0;
}

/**
funkcja get_SignedAttribute_MessageDigest() pozwala wydobyc ze struktury podpisu CMS,
wartosc skrotu z danych, na ktorych zostal wykonany podpis.

Uwaga: programista musi sam sprawdzic, jakiej funkcji skrotu uzyto do wyliczenia skrotu z danych

Argumenty wejsciowe:
@arg signature to bufor z podpisem CMS (w postaci DER)
Argumenty wyjsciowe:
@arg hash to adres wskaznika na alokowany bufor z wydobyta wartoscia skrotu

Wartosci zwracane:
@retval 0 w przypaku sukcesu operacji
Odpowiednia wartosc ujemna w przypadku bledu:
@retval -1 jesli za signature podano NULL
@retval -2 jesli za hash podano NULL
@retval -3 jesli wartosc wyluskana z hash nie jest wyNULLowana
@retval -4 jesli nie mozna zdekowoac podpisu
@retval -5 jesli nie mozna wydobyc struktury SignedData z podpisu
@retval -6 jesli nie ma zadnej struktury SignerInfo
@retval -7, -8 jesli brak atrybutow podpisanych
@retval -9 jesli brak wartosci atrybutu MessageDigest
@retval -10 jesli nie mozna zdekodowac atrybutu MessageDigest
@retval -11 jesli wystapil problem z alokacja pamieci
@retval -12 jesli nie odnaleziono atrybutu MessageDigest wsrod atrybutow podpisanych
*/
long get_SignedAttribute_MessageDigest(GenBuf_t *signature, GenBuf_t** hash)
{
	long ret_val=0;
	ContentInfo_t* content_info=NULL;
	SignedData_t* signedData=NULL;
	SignedAttributes_t *signedAttrs_ptr=NULL;
	long attributes_num=0;
	long iter=0;
	char messageDigest_oid[]= { OID_CMS_ID_MESSAGE_DIGEST };
	Attribute_t *messageDigestAttr_ptr=NULL;
	MessageDigest_t *messageDigest=NULL;
	asn_dec_rval_t dec_result;

	if(signature == NULL)
		{ return -1; }
	if(hash == NULL)
		{ return -2; }
	if(*hash != NULL)
		{ return -3; }

	ret_val=get_CMScontent_from_GenBuf(signature, &content_info);
	if(ret_val != BMD_OK)
		{ return -4; }

	ret_val=ContentInfo_get_SignedData(content_info, &signedData);
	asn_DEF_ContentInfo.free_struct(&asn_DEF_ContentInfo, content_info, 1); //content_info juz niepotrzebne
	free(content_info); content_info=NULL;
	if(ret_val != BMD_OK)
		{ return -5; }

	if(signedData->signerInfos.list.count <= 0)
	{
		asn_DEF_SignedData.free_struct(&asn_DEF_SignedData, signedData, 1);
		free(signedData); signedData=NULL;
		return -6;
	}

	//zakladam, ze jeden element w zbiorze
	//przypisanie wskaznika
	signedAttrs_ptr=signedData->signerInfos.list.array[0]->signedAttrs;
	//jesli brak atrybutow podpisanych  (glupota, ale sprawdzam)
	if(signedAttrs_ptr == NULL)
	{
		asn_DEF_SignedData.free_struct(&asn_DEF_SignedData, signedData, 1);
		free(signedData); signedData=NULL;
		return -7;
	}

	attributes_num=signedAttrs_ptr->list.count;
	//jesli pusty zbior atrybutow podpisanych (glupota, ale sprawdzam)
	if(attributes_num <= 0)
	{
		asn_DEF_SignedData.free_struct(&asn_DEF_SignedData, signedData, 1);
		free(signedData); signedData=NULL;
		return -8;
	}
	//do tego miejsca mam zaalokowany tylko signedData


	for(iter=0; iter< attributes_num; iter++)
	{

		ret_val=OBJECT_IDENTIFIER_cmp(&(signedAttrs_ptr->list.array[iter]->attrType), messageDigest_oid);
		//znalazl atrybut podpisany messagedigest
		if(ret_val == BMD_OK)
		{
			//przypisanie wskaznika atrybutu
			messageDigestAttr_ptr=signedAttrs_ptr->list.array[iter];
			//brak wartosci atrybutu messageDigest
			if(messageDigestAttr_ptr->attrValues.list.count <= 0)
			{
				asn_DEF_SignedData.free_struct(&asn_DEF_SignedData, signedData, 1);
				free(signedData); signedData=NULL;
				return -9;
			}

			dec_result=ber_decode(0, &asn_DEF_MessageDigest, (void**)&messageDigest, messageDigestAttr_ptr->attrValues.list.array[0]->buf, messageDigestAttr_ptr->attrValues.list.array[0]->size);
			if(dec_result.code != RC_OK)
			{
				asn_DEF_SignedData.free_struct(&asn_DEF_SignedData, signedData, 1);
				free(signedData); signedData=NULL;
				return -10;
			}
			ret_val=set_gen_buf2((char *)(messageDigest->buf), messageDigest->size, hash);
			//signedData i messageDigest nie sa juz potrzebne
			asn_DEF_SignedData.free_struct(&asn_DEF_SignedData, signedData, 1);
			free(signedData); signedData=NULL;
			asn_DEF_MessageDigest.free_struct(&asn_DEF_MessageDigest, messageDigest, 1);
			free(messageDigest); messageDigest=NULL;
			if(ret_val == BMD_OK)
				{ return 0; }
			else
				{ return -11; }
		}
		//else sprawdzam kolejny atrybut podpisany

	}

	asn_DEF_SignedData.free_struct(&asn_DEF_SignedData, signedData, 1);
	free(signedData); signedData=NULL;
	return -12; //brak atrybutu podpisanego messageDigest
}

