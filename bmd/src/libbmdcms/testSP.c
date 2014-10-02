#include <bmd/libbmdcms/libbmdcms.h>

/*#define kwalifikowany*/

#ifndef kwalifikowany
void print_test(char *func_name, int invoke_result)
{
	printf("-------\nNazwa funkcji: %s\n",func_name);
	if(invoke_result==0)
	{
		printf("%s --> STATUS: OK\n",func_name);
	}
	else
	{
		printf("%s --> STATUS: FAILED; returned value = %i\n", func_name,invoke_result);
	}
}

int main()
{

SignaturePolicyIdentifier_t *SP_wsk=NULL;
SignaturePolicyIdentifier_t *zdekodowany=NULL;
Attribute_t* attr=NULL;
long int nums[]={ 1, 2, 3, 4, 5, 6, 7, 8, 9};
OCTET_STRING_t *oct_wsk=(OCTET_STRING_t*)calloc(1,sizeof(OCTET_STRING_t));

print_test("SPId_SPIdentifier_Create", SPId_SPIdentifier_Create("1.2.840.113549.1.9.16.5.1", "1.2.840.113549.1.9.16.5.20", NULL, oct_wsk, &SP_wsk));
/*FIXED niszczecnie OtherHashValue_t czyli OCTET_STRING_t wewnatrz funkcji SPId_SPIdentifier_Create()*/
Destroy_OtherHashValue(&oct_wsk); //nie zwraca wartosci
/*!!!parameters zamienione na NULL*/
print_test("SPId_Uri_Add", SPId_Uri_Add("bla/blebla", SP_wsk));
print_test("SPId_NoticeRef_Add", SPId_UserNotice_Add("wyswietlany text",
		"Unizeto", nums, 9, SP_wsk));
print_test("SPId_SPIdentifier_CreateAttribute", SPId_SPIdentifier_CreateAttribute(SP_wsk, &attr));
SPId_SPIdentifier_Destroy(&SP_wsk); //nie zwraca wartosci
print_test("SPId_GetSPIdentifierfromAttribute",SPId_GetSPIdentifierfromAttribute(attr, &zdekodowany));

/*przykladowe sprawdzenie, czy mozna dobrac sie do wewnetrznej struktury*/
uint8_t *bu=(zdekodowany->choice.signaturePolicyId.sigPolicyQualifiers->list.array[0]->sigQualifier).buf;
int sajz=(zdekodowany->choice.signaturePolicyId.sigPolicyQualifiers->list.array[0]->sigQualifier).size;
SPuri_t *extracted_uri=(SPuri_t*)calloc(1,sizeof(SPuri_t));
ber_decode(0,&asn_DEF_SPuri,(void**)&extracted_uri,bu,sajz);
printf("uri: %s\n",extracted_uri->buf);
Destroy_SPuri(&extracted_uri);
/*end: przykladowe...*/

SPId_SPIdentifier_Destroy(&zdekodowany); //nie zwraca wartosci
print_test("SPId_SPIdentifierAttribute_Destroy",SPId_SPIdentifierAttribute_Destroy(&attr));

return 0;
}

#else /* #ifdef kwalifikowany*/


/*dobieranie sie do OIDow atrybutow podpisanych i niepodpisanych w strukturze podpisu*/
int main()
{
asn_dec_rval_t dec_result;
GenBuf_t *buff=NULL;
load_binary_content("test.txt..zew.cck002.sig", &buff);
SignedData_t *struktura_podpisu=NULL;
ContentInfo_t *podpis=NULL;


podpis=(ContentInfo_t*)calloc(1, sizeof(ContentInfo_t));
dec_result=ber_decode(0, &asn_DEF_ContentInfo, (void**)&podpis, buff->buf, buff->size);
free(buff->buf);
if(dec_result.code != RC_OK)
{
	asn_DEF_ContentInfo.free_struct(&asn_DEF_ContentInfo, podpis, 1);
	free(podpis);
	printf("nie udalo sie\n");
	return -1;
}
/*printf("udalo sie\n");
asn_DEF_ContentInfo.free_struct(&asn_DEF_ContentInfo, podpis, 1);
free(podpis);*/

dec_result=ber_decode(0, &asn_DEF_SignedData, (void**)&struktura_podpisu, podpis->content.buf,podpis->content.size);
if(dec_result.code != RC_OK)
{
	asn_DEF_ContentInfo.free_struct(&asn_DEF_ContentInfo, podpis, 1);
	free(podpis);
	asn_DEF_SignedData.free_struct(&asn_DEF_SignedData, struktura_podpisu, 1);
	free(struktura_podpisu);
	printf("1 nie udalo sie\n");
	return -1;
}

SignedAttributes_t *sigattrs=struktura_podpisu->signerInfos.list.array[0]->signedAttrs;
UnsignedAttributes_t *unsigattrs=struktura_podpisu->signerInfos.list.array[0]->unsignedAttrs;
printf("czy sa niepodpisane atrybuty: %p\n", sigattrs);
printf("czy sa podpisane: %p;\n", unsigattrs);

printf("ile atrybutow podpissanych: %i\n",sigattrs->list.count );
int i=0, j=0;
int tab_oid[10];
int num_slot=0;
for(i=0; i<sigattrs->list.count; i++)
{
	num_slot=OBJECT_IDENTIFIER_get_arcs( &(sigattrs->list.array[i]->attrType), tab_oid, sizeof(tab_oid[0]), 10);
	for(j=0; j<num_slot; j++)
	{
		printf("%i:%i; ", j, tab_oid[j]);
	}
	printf("\n");
}

printf("\nniepodpisane\n");
for(i=0; i<unsigattrs->list.count; i++)
{
	num_slot=OBJECT_IDENTIFIER_get_arcs( &(unsigattrs->list.array[i]->attrType), tab_oid, sizeof(tab_oid[0]), 10);
	for(j=0; j<num_slot; j++)
	{
		printf("%i:%i; ", j, tab_oid[j]);
	}
	printf("\n");
}



printf("udalo sie");
asn_DEF_ContentInfo.free_struct(&asn_DEF_ContentInfo, podpis, 1);
free(podpis);
asn_DEF_SignedData.free_struct(&asn_DEF_SignedData, struktura_podpisu, 1);
free(struktura_podpisu);


return 0;
}

#endif /*ifndef kwalifikowany*/
