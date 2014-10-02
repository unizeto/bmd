#include<bmd/libbmdcms/libbmdcms.h>

int main()
{
bmd_signature_params_t *parametry=NULL;
//GenBuf_t *cert_marua=NULL;
GenBuf_t *marua_hash=NULL;
GenBuf_t *spol_hash=NULL;
GenBuf_t *signed_data=NULL;

GenBuf_t *test_file=NULL;
GenBuf_t *cck002=NULL;

GenBuf_t *hash_for_req=NULL;
GenBuf_t *request=NULL;
GenBuf_t *response=NULL;
Attribute_t *attr_contentTS=NULL;

bmd_crypt_ctx_t *sign_context=NULL;
bmd_crypt_ctx_t *hash_context=NULL;

SignedAttributes_t *sig_attrs=NULL;
Attribute_t *attr_SigPolID=NULL;
Attribute_t *attr_cert=NULL;
Certificate_t *certyfikat=NULL;
int status=0;

/*
printf("oid polityki podpisu\n");
SignaturePolicy_t *spol=NULL;
GenBuf_t *polityka=NULL;
load_binary_content("cck002kwal.spol", &polityka);
Decode_spol(polityka, &spol);
int oid_spol[10];
int i=0;
int num_slots= OBJECT_IDENTIFIER_get_arcs( &(spol->signPolicyInfo.signPolicyIdentifier), oid_spol, sizeof(oid_spol[0]), 10);
for(i=0; i<num_slots; i++)
{
	printf(" %i: %i;\n", i, oid_spol[i]);
}
Destroy_spol(&spol);
*/
printf("Ten test wymaga w bierzacym katalogu plików:\n test.txt\n cck002.spol\n marua.pfx \n\n");

bmd_init();

printf("wynik ustawiania sign_context: %i\n", bmd_set_ctx_file("marua.pfx","12345678", 8, &sign_context));
printf("wynik ustawienia hash_context: %i\n", bmd_set_ctx_hash(&hash_context,BMD_HASH_ALGO_SHA1));

load_binary_content("test.txt", &test_file);

/*liczenie skrótu z  polityki podpisu   -->  cck002.spol   */
load_binary_content("cck002.spol", &cck002);
printf("wynik liczenia skrotu z cck002.spol: %i\n", bmd_hash_data(cck002, &hash_context, &spol_hash, NULL));
free_gen_buf(&cck002);
printf("status SigAttr_Create_SignaturePolicy(): %i\n", SigAttr_Create_SignaturePolicyId("1.2.616.1.113527.2.10.3.2", "www.certyfikat.pl/repozytorium/cck002.spol", spol_hash, &attr_SigPolID));
free_gen_buf(&spol_hash); //zwalnianie pamieci ze skrotu
/*atrybut signingcertificate*/


/*signingcertificate*/
/*liczenie skrotu z certyfikatu*/
printf("liczenie hashu : %i\n", bmd_hash_data(sign_context->file->cert, &hash_context, &marua_hash, NULL));

/*Dekodowanie certyfikatu podpisujacego do Certificate_t*/
Certificate_from_ctx(sign_context, &certyfikat);

printf("status SigAttr_Create_SigningCertificate(): %i\n", SigAttr_Create_SigningCertificate(marua_hash, certyfikat, &attr_cert));
free_gen_buf(&marua_hash);
Destroy_Certificate(&certyfikat);
/*/signingcertificate*/



/*signingTime*/
Attribute_t *attr_time=NULL;
printf("status SigAttr_Create_SigningTime(): %i\n", SigAttr_Create_SigningTime(&attr_time));
/*/signingTime*/


/*contentTimestamp*/
bmd_hash_data(test_file, &hash_context, &hash_for_req, NULL);
printf("wynik requesta ts: %i\n", _bmd_create_ts_request(hash_for_req, BMD_HASH_ALGO_SHA1, &request));
printf("odpowiedz (naszego) timestampa: %i\n", bmd_generate_timestamp(request, sign_context, &response, NULL));
printf("SigAttr_Create_ContentTimestamp() :%i\n", SigAttr_Create_ContentTimestamp(response, hash_for_req, &attr_contentTS));
free_gen_buf(&hash_for_req);
free_gen_buf(&request);
free_gen_buf(&response);
/*/contentTimeStamp*/


/*tworzenie listy atrybutow podpisanych*/
SignedAtributes_init(&sig_attrs);
/*dodawanie do listy atrybutow podisanych*/
status=SignedAtributes_add(attr_SigPolID, sig_attrs);
printf("SignedAtributes_add() --> SigPolId:  %i\n",status);
status=SignedAtributes_add(attr_cert, sig_attrs);
printf("SignedAtributes_add() --> SigningCertificate: %i\n", status);
status=SignedAtributes_add(attr_time, sig_attrs);
printf("SignedAtributes_add() --> SigningTime: %i\n", status);
status=SignedAtributes_add(attr_contentTS, sig_attrs);
printf("SignedAtributes_add() --> contentTimestamp: %i\n", status);

/*przygotowaywanie struktury bmd_signature_params_t*/
SigningParams_Create(sig_attrs, NULL, &parametry);

printf("Tworzenie podpisu\n");
status=bmd_sign_data(test_file, sign_context, &signed_data, (void*)parametry); /*void *opt*/
printf("status bmd_sign_data(): %i\n", status);

/*sprzatanie*/
SigningParams_Destroy(&parametry);
bmd_ctx_destroy(&sign_context);
bmd_ctx_destroy(&hash_context);
free_gen_buf(&test_file);

printf("Zapisuje podpis do pliku\n");
bmd_save_buf(signed_data, "wlodzimierz5.sig");
free_gen_buf(&signed_data); /*niszczenie podpisu*/



bmd_end();
return 0;

}
