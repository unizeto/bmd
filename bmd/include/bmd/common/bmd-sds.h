#ifndef _BMD_SDS_INCLUDED_
#define _BMD_SDS_INCLUDED_

#ifndef _WIN32
#include<netinet/in.h>
#endif

#ifdef _WIN32
typedef unsigned int uint32_t;
#endif
#include <bmd/common/bmd-os_specific.h>
/*#include <bmd/libbmd/libbmd.h>
#include <bmd/common/bmd-crypto_ds.h>*/
#if defined(__cplusplus)
extern "C" {
#endif
/* bufor ogolnego przeznaczenia */

#define OWNER_TYPE FILE		0
#define OWNER_TYPE_SIGNATURE	1
#define OWNER_TYPE_DVCS		2

typedef struct GenBuf {
	char *buf;
	long size;
} GenBuf_t;

/* lista buforów ogolnego przeznaczenia */
typedef struct GenBufList {
	GenBuf_t **gbufs;
	long size;
} GenBufList_t;

/** tablica stringow */
typedef struct bmdStringArray
{
	char** array;
	long itemsCounter;
} bmdStringArray_t;


/* bufor ogolnego przeznaczenia dla substringow */
typedef struct SubstringsGenBuf {
	char **substrings_table;
	long no_of_substrings;
} SubstringsGenBuf_t;

typedef GenBuf_t BMD_attr_value;	/* typedef dla wygody */

/* struktura opisujaca OID atrybutu */

typedef struct BMD_attr {
	GenBuf_t *label;
	long wxType;
	long visibility;
	long targetForm;
	long seqNumber;
	BMD_attr_value **valuesList;
	long *priorityList;
	long sizeX;
	long sizeY;
	GenBuf_t *oid;
	char *oid_str;
	long contentType;
	GenBuf_t *regexp;
	long superiorControl;
	long inferiorControl;
	long connectType;
	long valueSource;
	GenBuf_t *tooltip;
	GenBuf_t *depValueList;
	long inheritancePresent;
	GenBuf_t *controlName;

	long splitToWords;
	long rangeEnabled;

	long mandatory;

	long isUpdateEnabled;		/* czy mozliwy jest update tej formatki */
	long isHistoryEnabled;		/* czy mozliwy jest history tej formatki */

	long maxLength;			/* dlugosc wartosci zwracana klientowi */

	long _internalValuesSourceFlag;
	long _internalPrioritySourceFlag;
} BMD_attr_t;

typedef struct MetaDataBuf {
	/*	nowe pola metadanych	*/
	long myId;			/* id obiektu (z bazy danych) */
	long ownerType;
	long ownerId;		/* id obiektu opisywanego przez bieżący obiekt */
	/*\	nowe pola metadanych	*/
	char *OIDTableBuf;
	char *AnyBuf;
	long AnySize;
} MetaDataBuf_t;

/*typedef struct OID_attr {*/
/*pkl	long *oid_table;	*//* tablica slotow OIDa */
/*pkl	long oid_table_size;*/	/* rozmiar powyzszej tablicy w B */
	/*char *oid_table;*/
/*} OID_attr_t;*/

/* struktura BMD Datagram */
typedef struct bmdDatagram
{
	long protocolVersion;
	long datagramType;
	MetaDataBuf_t **actionMetaData;
	long no_of_actionMetaData;
	MetaDataBuf_t **sysMetaData;
	long no_of_sysMetaData;
	MetaDataBuf_t **pkiMetaData;
	long no_of_pkiMetaData;
	MetaDataBuf_t **additionalMetaData;
	long no_of_additionalMetaData;

	long additionalMetaData_string_count;	/*Ilosc metadanych dodatkowych w additionalMetaData_string*/
	char *additionalMetaData_string;     	/*Metadane dodatkowe przechowywane w dlugim ciagu char* oddzielone NULL*/
	long *additionalMetaData_string_len;	/*Dlugosc poszczegolnych metadanych w bajtach przechowywanych w
						additionalMetaData_string bez rozdzielajacego NULL*/

	GenBuf_t *protocolData;
	GenBuf_t *protocolDataFilename;
	GenBuf_t *protocolDataFileLocationId;
	GenBuf_t *protocolDataFileId;
	GenBuf_t *protocolDataOwner;
	long randId;
	long filesRemaining;
	long datagramStatus;
} bmdDatagram_t;

typedef struct bmdDatagramSet {
	bmdDatagram_t **bmdDatagramSetTable;
	long bmdDatagramSetSize;
} bmdDatagramSet_t;

typedef struct bmd_proxy_info {
	long proxy_type;
	char *proxy_url;
	long proxy_port;
	char *proxy_username;
	char *proxy_password;
	char *proxy_auth_type;
} bmd_proxy_info_t;

typedef struct bmd_form_info {
	BMD_attr_t **user_form;
	BMD_attr_t **actions_form;
	BMD_attr_t **user_send_form;
	BMD_attr_t **user_search_form;
	BMD_attr_t **user_view_form;
	BMD_attr_t **user_unvisible_form;
	BMD_attr_t **user_update_form;
	BMD_attr_t **user_history_form;
} bmd_form_info_t;

typedef struct bmdKeyIV {
	long keyEncAlgo;
	GenBuf_t *encrypted_sym_key;
	long symAlgo;
	GenBuf_t *IV;
} bmdKeyIV_t;

struct progress_data {
	double total_size;		    /* calkowity rozmiar danych do wyslania */
	double total_cur_size;	    /* calkowity rozmiar danych juz przetworzonych */
	double now_size;		    /* ilosc bajtow pojedynczego pliku */
	double now_cur_size;	    /* ilosc bajtow przetworzonych z pojedynczego pliku */
	long total_object_count;	    /* ile pozycji do wyslania */
	long current_object_count;	    /* ktora pozycja jest wysylana aktualnie */
	void *prog_data;		    /* progress dialog */
	void *prog_text;		    /* tekst do wyswietlenia w progress dialogu */
	void *func;			    /* funkcja do wyswietlania progressu */
	void *func_setTitle;	/* funkcja do ustawiania tytulu progressa
							(potrzebne przy wysylce w oczekiwaniu na odpowiedz serwera) */
};

typedef struct group_pair {
	long src;
	long dest;
} group_pair_t;

typedef struct group_info {
	long id;
	group_pair_t **levels;
} group_info_t;


typedef struct client_group_pair {
	char *parent_group;
	char *child_group;
	char *child_group_oid;
} client_group_pair_t;

typedef struct client_group_info {
	char *group_name;
	char *group_name_oid;
	client_group_pair_t **levels;
} client_group_info_t;



/*msilewicz - slownik dla tabeli groups_graph  */
typedef struct groups_graph_dict {
	char **main_groups;
	char ***dependent_groups;
} groups_graph_dict_t;

typedef struct certificate_attributes
{
	GenBuf_t *id;
	GenBuf_t *name;
	GenBuf_t *identityId;
	GenBuf_t *serial;
	GenBuf_t *cert_dn;
	GenBuf_t *accepted;

	GenBufList_t *groups;
	GenBufList_t *roles;
	GenBufList_t *security;

	GenBuf_t *user_chosen_role;
	GenBuf_t *user_chosen_group;

} certificate_attributes_t;

typedef struct translate
{
	char *from;
	char *to;

} translate_t;


struct CertExtConfigStruct
{
	long debug_raport_error;					/*Czy mamy wyswietlic okienko z numerem bledu, ktory zdecydowal o odrzuceniu certyfikatu*/

	long check_cert_validity_existence;			/*Czy mamy sprawdzac istnienie pola z czase, waznosci*/
	long check_time_validity;					/*Czy mamy sprawdzac waznosc certyfikatu*/

	long check_non_repudation;					/*Czy mamy sprawdzac w certyfikacie podpisywania istnienie bitu non repudation*/
	long non_repudation_bit_only;				/*Czy mamy zezwalac w certyfikacie tylko na bit non repudation*/


	long check_certificate_version;				/*Czy mamy sprawdzac czy wersja certyfikatu to 2*/


	long check_certificate_sign_algorithm;		/*Czy mamy sprawdzac algorythm uzyty do podpisania certyfikatu*/


	long check_public_key_alghoritm;			/*Czy mamy sprawdzać algorytm klucza publicznego w certyfikacie*/


	long chcek_certificate_issuer;				/*Czy mamy sprawdzac pole wystawcy w certyfikacie*/
	long check_issuer_country;					/*Czy mamy sprawdzac istnienie pola kraj w wystawcy*/
	long check_issuer_organisation;				/*Czy mamy sprawdzac istnienie pola organizacji w wystawcy*/
	long check_issuer_cn_and_sn;				/*Czy mamy sprawdzać czy istnieje pole z nazwa powszechna lub numerem seryjnym wystawcy*/


	long check_certificate_subject;				/*Czy mamy sprawdzac pola zwiazane z podmiotem certyfikatu*/
	long check_subject_name_with_pseudonym;		/*Czy mamy sprawdzac istnienie jednoczesnie pola pseudonim z polami imie lub nazwisko. Rozporzadzenie zabrania tego.*/
	long check_subject_org_with_address;		/*Czy mamy sprawdzac istnienie pola organizacji jednoczesnie z adresem. Rozporzadzenie wymaga ze w przypadku podania
												organizacji trzeba tez podac adress.*/
	long check_certificate_category;			/*Czy mamy sprawdzic kategorie certyfikatu. Rozporzadzenie dopuszcza 3 kategorie certyfikatow.*/

	long check_certificate_extensions;			/*Czy mamy sprawdzic sprawy zwiazane z rozszerzeniami certyfikatow*/

	long check_extensions_existence;			/*Czy w przypadku braku rozszerzen mamy zwroci blad*/
	long check_forbidden_extensions;			/*Czy mamy sprawdzac istnienie rozszerzen zakazanych przez rozporzadzenie - na razie chodzi tylko o subjectKeyIdentifier)*/
	long check_critical_extensions;				/*Czy mamy sprawdzac rozszerzenia krytyczne czy naleza do obslugiwanych przez nas typow*/
	long check_wrongly_non_critical;			/*Czy mamy sprawdzac czy pewne rozszerzenia ktore powinny byc wedlug ustawy krytyczne przez przypadek nie sa ustawione jako niekrytyczne*/

	long raport_non_supported_critical;			/*Czy mam zglosic blad jezeli zostanie znalezione nierozpoznane rozszerzenie krytyczne*/
	long check_extended_key_usage;				/*Czy w przypadku pola extended key usage mam zglosic blad jezeli jest inne uzycie niz anyExtendedKeyUsage Oid=2.5.29.37.0*/

	long check_with_crl;						/*Czy certyfikat ma zostac sprawdzony pod katem wystepowania na liscie crl*/
	long download_crl;							/*Okresla, czy CRL ma byc zciagnieta*/
	char* url_of_crl;							/*Adres url wskaujacy na liste crl dla sprawdzanego certyfikatu*/
	long use_crl_file;							/*Okresla, czy ma byc wykorzystany plik CRL*/
	char* crl_file_path;						/*Sciezka do pliku z lista crl dla sprawdzanego certyfikatu*/

};
typedef struct CertExtConfigStruct CertExtConfig_t;

struct ConservationConfig
{
	long round_size;
	long check_timestamp_validity;
	long round_sleep;
	long max_conservation_rounds_count;
	long no_tsa_cert_number;
};
typedef struct ConservationConfig ConservationConfig_t;

typedef long (*role_selection_callback)(GenBufList_t *,long );


typedef struct command_metadata_citeria {
	char **oid;
	GenBuf_t **crit;
	GenBuf_t **crit_from;
	GenBuf_t **crit_to;
} command_metadata_citeria_t;


#if defined(__cplusplus)
}
#endif
#endif
