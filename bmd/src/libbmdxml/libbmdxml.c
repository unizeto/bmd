#include <libxml/parser.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdbase64/libbmdbase64.h>
#include <bmd/libbmdxml/libbmdxml.h>
#include <bmd/libbmdsql/common.h>

#include <bmd/libbmd/libbmd.h>
#include <bmd/common/bmd-sds.h>
#include <libxml/parser.h>

#include <bmd/libbmdpr/bmd_datagram/bmd_datagram.h>
#include <bmd/libbmddb/libbmddb.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifndef WIN32
	#include <unistd.h>
#endif
#include <bmd/libbmdpki/libbmdpki.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmdglobals/libbmdglobals.h>


#ifdef WIN32
typedef long (__cdecl *gewc_f)(dictionary_data_t *,long,char *,long,char **);
#endif

#define MALLOC_CHECK_ 2
#define MAX_CHILD_STACK 1024

long bmdxml_parse_ex(void);
long bmdxml_destroy(void);
long bmdxml_findTag(char *buffer, long bufflen, const char *tag,
		   char **tag_start, char **tag_end, char **prev_tag_end);
long bmdxml_gettag(xmlNodePtr root, char *val, xmlNodePtr * dst);

long bmdxml_gettag(xmlNodePtr root, char *val, xmlNodePtr * dst)
{
	xmlNodePtr cur;
	xmlNodePtr childstack[MAX_CHILD_STACK];
	long childstackptr = 0;

	cur = root;
	if (cur == NULL) {
		PRINT_DEBUG("empty root node\n");
		return (-1);
	}
	childstack[childstackptr] = cur;
	childstackptr++;
	while (childstackptr > 0) {
		childstackptr--;
		cur = childstack[childstackptr];
		while (cur != NULL) {
			if ((cur->type == XML_ELEMENT_NODE)
			    && (cur->children)) {
				if (childstackptr < MAX_CHILD_STACK) {
					childstack[childstackptr] =
					    cur->children;
					childstackptr++;
				}
			}
			if ((!xmlStrcmp(cur->name, (xmlChar*) val))) {
				(*dst) = cur;
				return (0);
			}
			cur = cur->next;
		}
	}
	return (-1);
}

/** czy xml zainicjalizowany? */
static long __bmdxml_initialized = 0;

/**
    Inicjalizacja biblioteki libbmdxml.

    @warning Ta funkcja nie jest bezpieczna dla wątków; powinna być wywoływana tylko
    raz na początku wykonania programu.

    @return 0 on success, !0 on error
*/
long bmdxml_init(void)
{
	if (__bmdxml_initialized == 0) {
		xmlDefaultSAXHandlerInit();
		xmlSAXDefaultVersion(2);
	}

	__bmdxml_initialized ++;

	return 0;
}

/**
    Destroy libbmdxml internal objects
*/

long bmdxml_destroy()
{
	if (!(__bmdxml_initialized > 0))
	{
		PRINT_ERROR("__bmdxml_initialized musi byc większe od zera!\n");
		abort();
	}
	__bmdxml_initialized  --;
	if (__bmdxml_initialized != 0) {
		return 0;
	}

	return 0;
}

long bmdxml_load_oids(char * oid_file)
{
#if 0
	long status = 0;
	bmd_assert(oid_file != NULL, "LK");
	bmd_assert(parsed_oid_file == NULL
		   && parsed_oid_file_size == 0, "LK");

    /** @todo zrobic pliki  oidów i inne takie ;] ładowane tylko raz przy bootowaniu */
	status =
	    parse_oid_file(oid_file, NULL, &parsed_oid_file,
			   &parsed_oid_file_size, PARSE_OIDS_FOR_USER);
	if (status != 100) {
		PRINT_DEBUG("Błąd %i podczas parsowania pliku %s\n",
			    status, oid_file);
		return ERR(ERR_conf, "Cannot load OID file", "LK");
	}

	return 0;
#endif
	bmd_assert(0,"MSil usunalem slady po oids.txt");
}

/** Convert OID to name of key.
@return malloc()ed string; you must call free() on it

@todo implement this function
@bug temporary implementation !
*/

xmlChar *bmd_oid2xmltag(	char *OID,
				char **metadataName)
{
xmlChar *ret		= NULL;
char *metadataOid	= NULL;

	DatabaseDictionaryTypeToName(	OID, metadataName);
	if (*metadataName==NULL)
	{
		PRINT_VDEBUG("Oid'a nie znaleziono w słowniku. OID: %s \n", OID);
		return NULL;
	}

	ret = xmlCharStrdup(*metadataName);

	/************/
	/* porzadki */
	/************/
	free0(metadataOid);

	return ret;
}

/** Convert metadata to XML node */

xmlNodePtr bmd_metadata2xml(	const MetaDataBuf_t * const metadata,
				EnumWithBase64Coding_t base64Coding)
{
xmlNodePtr node;
char *tag		= NULL;
char *data		= NULL;
char *metadataName	= NULL;
char *basedData		= NULL;

	/** @fixme poprawka kompatybilności - dzielenie przez sizeof(ulong) */
	tag = (char*)bmd_oid2xmltag(metadata->OIDTableBuf, &metadataName);
	if (tag != NULL)
	{
		node = xmlNewNode(NULL, (xmlChar*) tag);
	}
	else
	{
		node = xmlNewNode(NULL, (xmlChar*) "UnrecognisedTag");  /** @fixme wymyśleć jakieś sensowne
													zachowanie w wypadku nie
													znalezienia OID w słowniku */
	}
	xmlFree(tag);

	if (!(node != NULL))
	{
		PRINT_ERROR("Node musi być różne od NULL!\n");
		abort();
	}
	/** @fixme poprawka kompatybilności - dzielenie przez sizeof(ulong) */

	if (metadataName!=NULL)
	{
		if (!(xmlNewProp(node, (xmlChar*) "name", (xmlChar*) metadataName) != NULL))
		{
			PRINT_ERROR("Error while executing xmlNewProp()\n");
			abort();
		}
	}


	if (!(xmlNewProp(node, (xmlChar*) "oid", (xmlChar*) metadata->OIDTableBuf) != NULL))
	{
		PRINT_ERROR("Error while executing xmlNewProp()\n");
		abort();
	}

	if (base64Coding==WITH_BASE64)
	{
		basedData=(char *)spc_base64_encode((unsigned char*)metadata->AnyBuf, metadata->AnySize, 0);

		data = malloc(strlen(basedData) + 1);
		if(data == NULL)
		{
			PRINT_ERROR("Error No Memory\n");
			abort();
		}

		memset(data, 0, strlen(basedData) + 1);
		memcpy(data, basedData, strlen(basedData));
	}
	else
	{

		data = malloc(metadata->AnySize + 1);
		if(data == NULL)
		{
			PRINT_ERROR("Error No Memory\n");
			abort();
		}

		memset(data, 0, metadata->AnySize + 1);
		memcpy(data, metadata->AnyBuf, metadata->AnySize);

	}

	xmlNodeSetContent(node, (xmlChar*) data);

	free(data);
	free0(basedData);

	return node;
}


/** Convert BMD datagram set to XML node */
/**
*@param allowedOids to lista oidow metadanych, ktorych umieszczenie w powiadomieniu jest dozwolone
* jesli NULL, to wszystkie dostepne w datagramie metadane sa zamieszczane w powiadomieniu
*/
xmlNodePtr bmd_datagramset2xml(	bmdDatagramSet_t * set,
				EnumWithBase64Coding_t base64Coding,
				bmdStringArray_t *allowedOids)
{
long i;
xmlNodePtr dset;
xmlNodePtr data;
xmlNodePtr new_node;
struct tm czas;
char *date_time	= NULL;

	/************************/
	/* walidacja paramterow */
	/************************/
	if (set == NULL)	{	return NULL;	}

	dset = xmlNewNode(NULL, (xmlChar*) "DatagramSet");
	if (!(dset != NULL))
	{
		PRINT_ERROR("Cannot create new xml node!\n");
		abort();
	}

	GetActualTime(&czas);
	asprintf(&date_time,"%02i-%02i-%02i %02i:%02i:%02i",czas.tm_year+1900, czas.tm_mon+1, czas.tm_mday,czas.tm_hour, czas.tm_min, czas.tm_sec);

	if (date_time!=NULL)
	{
		data = xmlNewNode(NULL, (xmlChar*) "DatagramSetTime");
		if (!(data != NULL))
		{
			PRINT_ERROR("Data cannot be null!\n");
			abort();
		}
		xmlNodeAddContentLen(	data,
					(unsigned char*)date_time,
					(int)strlen(date_time));
		if (!(xmlAddChild(dset, data) != NULL))
		{
			PRINT_ERROR("Cannot add new child!\n");
			abort();
		}
		free0(date_time);
	}

	for (i = 0; i < set->bmdDatagramSetSize; i++)
	{
		new_node = bmd_datagram2xml(set->bmdDatagramSetTable[i], base64Coding, allowedOids);
		if (new_node == NULL)
		{
			PRINT_ERROR("Unable to create new XML node\n");
			xmlFreeNode(dset);
			return NULL;
		}
		if (!(xmlAddChild(dset, new_node) != NULL))
		{
			PRINT_ERROR("Cannot add new child to dset!\n");
			abort();
		}

	}

	return dset;
}

/** Convert BMD datagram to XML node */
/**
*@param allowedOids to lista oidow metadanych, ktorych umieszczenie w powiadomieniu jest dozwolone
* jesli NULL, to wszystkie dostepne w datagramie metadane sa zamieszczane w powiadomieniu
*/
xmlNodePtr bmd_datagram2xml(	const bmdDatagram_t * bmdDatagram,
				EnumWithBase64Coding_t base64Coding,
				bmdStringArray_t *allowedOids)
{
long i		= 0;
char *tmp	= NULL;
xmlNodePtr node;
xmlNodePtr data;
xmlNodePtr metadata;
xmlNodePtr new_node;

long oidsCount	= 0;
long iter	= 0;
char* oidPtr	= NULL;
long allowedMetadataFlag	= 0;

	/************************/
	/* walidacja parametrow */
	/************************/
	if(!(bmdDatagram != NULL))
	{
		PRINT_ERROR("bmdDatagram cannot be null!\n");
		abort();
	}

	/****************/
	/* wezel glowny */
	/****************/
	node = xmlNewNode(NULL, (xmlChar*) "Datagram");
	if (!(node != NULL))
	{
		PRINT_ERROR("Node cannot be NULL!\n");
		abort();
	}


	/* content (protocolData) */
	if (bmdDatagram->protocolData)
	{
		data = xmlNewNode(NULL, (xmlChar*) "Content");
		if (!(data != NULL))
		{
			PRINT_ERROR("Data cannot be null!\n");
			abort();
		}
		xmlNodeAddContentLen(	data,
					(unsigned char*)bmdDatagram->protocolData->buf,
					(int)bmdDatagram->protocolData->size);
		if (!(xmlAddChild(node, data) != NULL))
		{
			PRINT_ERROR("Cannot add new child!\n");
			abort();
		}
	}

	/* filename */
	if (bmdDatagram->protocolDataFilename)
	{
		data = xmlNewNode(NULL, (xmlChar*) "Filename");
		if (!(data != NULL))
		{
			PRINT_ERROR("Cannot create new node!\n");
			abort();
		}
		xmlNodeAddContentLen(	data,
					(unsigned char*)bmdDatagram->protocolDataFilename->buf,
					bmdDatagram->protocolDataFilename->size);
		if (!(xmlAddChild(node, data) != NULL))
		{
			PRINT_ERROR("Cannot add child node!\n");
			abort();
		}
	}


	/* fileID */
	if (bmdDatagram->protocolDataFileId)
	{
		data = xmlNewNode(NULL, (xmlChar*) "FileID");
		if (!(data != NULL))
		{
			PRINT_ERROR("Cannot add new node!\n");
			abort();
		}
		xmlNodeAddContentLen(data, (unsigned char*)bmdDatagram->protocolDataFileId->buf, bmdDatagram->protocolDataFileId->size);
		if (!(xmlAddChild(node, data) != NULL))
		{
			PRINT_ERROR("Cannot add child to node (FileID)\n");
			abort();
		}
	}

	/* file location id */
	if (bmdDatagram->protocolDataFileLocationId)
	{
		data = xmlNewNode(NULL, (xmlChar*) "FileLocationID");
		if (!(data != NULL))
		{
			PRINT_ERROR("Cannot add new node!\n");
			abort();
		}
		xmlNodeAddContentLen(data,
				(unsigned char*)bmdDatagram->protocolDataFileLocationId->buf,
				bmdDatagram->protocolDataFileLocationId->size);
		if (!(xmlAddChild(node, data) != NULL))
		{
			PRINT_ERROR("Cannot add child to node (FileID)\n");
			abort();
		}
	}

	// bmdDatagram->datagramStatus
	{
		asprintf(&tmp, "%li", bmdDatagram->datagramStatus);
		data = xmlNewNode(NULL, (xmlChar*) "DatagramStatus");
		if (!(data != NULL))
		{
			PRINT_ERROR("Cannot add new node!\n");
			abort();
		}
		xmlNodeAddContentLen(data,
				     (unsigned char*)tmp,
				     strlen(tmp));
		if (!(xmlAddChild(node, data) != NULL))
		{
			PRINT_ERROR("Cannot add child to node (DatagramStatus)\n");
			abort();
		}
		free0(tmp);
	}

	// bmdDatagram->datagramType
	{
		asprintf(&tmp, "%li", bmdDatagram->datagramType);
		data = xmlNewNode(NULL, (xmlChar*) "DatagramType");
		if (!(data != NULL))
		{
			PRINT_ERROR("Cannot add new node!\n");
			abort();
		}
		xmlNodeAddContentLen(data,
				     (unsigned char*)tmp,
				     strlen(tmp));
		if (!(xmlAddChild(node, data) != NULL))
		{
			PRINT_ERROR("Cannot add child to node (DatagramStatus)\n");
			abort();
		}
		free0(tmp);
	}

	/* owner */
	if (bmdDatagram->protocolDataOwner)
	{
		data = xmlNewNode(NULL, (xmlChar*) "Owner");
		if(!(data != NULL))
		{
			PRINT_ERROR("Cannot add Owner node!\n");
			abort();
		}
		xmlNodeAddContentLen(data,
				     (unsigned char*)bmdDatagram->protocolDataOwner->buf,
				     bmdDatagram->protocolDataOwner->size);
		if(!(xmlAddChild(node, data) != NULL))
		{
			PRINT_ERROR("Cannot add child to Owner node!\n");
			abort();
		}
	}


	/* action metadata */
	if (bmdDatagram->actionMetaData)
	{
		metadata = xmlNewNode(NULL, (xmlChar*) "ActionMetadata");
		if(!(metadata != NULL))
		{
			PRINT_ERROR("Cannot create ActionMetadata node!\n");
			abort();
		}

		if(!(xmlAddChild(node, metadata) != NULL))
		{
			PRINT_ERROR("Cannot add ActionMetadata child!\n");
			abort();
		}
		for (i = 0; i < bmdDatagram->no_of_actionMetaData; i++)
		{
			xmlNodePtr new_node;

			allowedMetadataFlag = 0;
			if(allowedOids != NULL)
			{
				bmdStringArray_GetItemsCount(allowedOids, &oidsCount);
				for(iter=0; iter<oidsCount; iter++)
				{
					bmdStringArray_GetItemPtr(allowedOids, iter, &oidPtr);
					if(strcmp(oidPtr, bmdDatagram->actionMetaData[i]->OIDTableBuf ) == 0)
					{
						allowedMetadataFlag = 1;
						oidPtr = NULL;
						break;
					}
					oidPtr = NULL;
				}
			}
			else
			{
				allowedMetadataFlag = 1;
			}

			if(allowedMetadataFlag == 1)
			{
				new_node = bmd_metadata2xml(bmdDatagram->actionMetaData[i], base64Coding);
	
				if(!(new_node != NULL))
				{
					PRINT_ERROR("Cannot create new node!\n");
					abort();
				}
				if(!(xmlAddChild(metadata, new_node) != NULL))
				{
					PRINT_ERROR("Cannot add new metadata node!\n");
					abort();
				}
			}
		}
	}

	/* sys metadata */
	if (bmdDatagram->sysMetaData)
	{
		metadata = xmlNewNode(NULL, (xmlChar*) "SystemMetadata");
		if(!(metadata != NULL))
		{
			PRINT_ERROR("Cannot create SystemMetadata node!\n");
			abort();
		}

		if (!(xmlAddChild(node, metadata) != NULL))
		{
			PRINT_ERROR("Cannot add new SystemMetadata child!\n");
			abort();
		}

		for (i = 0; i < bmdDatagram->no_of_sysMetaData; i++)
		{
			// w powiadomieniu dla bezpieczenistwa nigdy nie bedzie umieszczany klucz symetryczny
			if(strcmp(bmdDatagram->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_DTG_SYMKEY) == 0)
			{
				continue;
			}
// 			if (	(strcmp(bmdDatagram->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CERT_LOGIN_AS)==0) ||
// 				(strcmp(bmdDatagram->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_DTG_SYMKEY_HASH)==0) ||
// 				(strcmp(bmdDatagram->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_DTG_FORM)==0) ||
// 				(strcmp(bmdDatagram->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_DTG_SYMKEY)==0) ||
// 				(strcmp(bmdDatagram->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_DTG_K)==0) ||
// 				(strcmp(bmdDatagram->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_DTG_I)==0) ||
// 				(strcmp(bmdDatagram->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_DTG_N)==0) ||
// 				(strcmp(bmdDatagram->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_DTG_KI_MOD_N)==0))
// 			{
// 				continue;
// 			}


			allowedMetadataFlag = 0;
			if(allowedOids != NULL)
			{
				bmdStringArray_GetItemsCount(allowedOids, &oidsCount);
				for(iter=0; iter<oidsCount; iter++)
				{
					bmdStringArray_GetItemPtr(allowedOids, iter, &oidPtr);
					if(strcmp(oidPtr, bmdDatagram->sysMetaData[i]->OIDTableBuf ) == 0)
					{
						allowedMetadataFlag = 1;
						oidPtr = NULL;
						break;
					}
					oidPtr = NULL;
				}
				
			}
			else
			{
				allowedMetadataFlag = 1;
			}

			if(allowedMetadataFlag == 1)
			{
				new_node = bmd_metadata2xml(bmdDatagram->sysMetaData[i], base64Coding);
	
				if(!(new_node != NULL))
				{
					PRINT_ERROR("Cannot create new node!\n");
					abort();
				}
				if (!(xmlAddChild(metadata, new_node) != NULL))
				{
					PRINT_ERROR("Cannot add new child node to SystemMetadata node!\n");
					abort();
				}
			}
		}
	}

	/* PKI metadata */
	if (bmdDatagram->pkiMetaData)
	{
		metadata = xmlNewNode(NULL, (xmlChar*) "PKIMetadata");
		if(!(metadata != NULL))
		{
			PRINT_ERROR("Cannot create new PKIMetadata node!\n");
			abort();
		}

		if(!(xmlAddChild(node, metadata) != NULL))
		{
			PRINT_ERROR("Cannot add new node!\n");
			abort();
		}
		for (i = 0; i < bmdDatagram->no_of_pkiMetaData; i++)
		{
			xmlNodePtr new_node;

			// w powiadomieniu nie powinien sie znalezc podpisany pdf ze wzgledu na potencjalne duze rozmiary
			if(strcmp(bmdDatagram->pkiMetaData[i]->OIDTableBuf, OID_PKI_METADATA_PADES_NEW_CRYPTO_OBJECT) == 0)
			{
				continue;
			}

			allowedMetadataFlag = 0;
			if(allowedOids != NULL)
			{
				bmdStringArray_GetItemsCount(allowedOids, &oidsCount);
				for(iter=0; iter<oidsCount; iter++)
				{
					bmdStringArray_GetItemPtr(allowedOids, iter, &oidPtr);
					if(strcmp(oidPtr, bmdDatagram->pkiMetaData[i]->OIDTableBuf ) == 0)
					{
						allowedMetadataFlag = 1;
						oidPtr = NULL;
						break;
					}
					oidPtr = NULL;
				}
				
			}
			else
			{
				allowedMetadataFlag = 1;
			}

			if(allowedMetadataFlag == 1)
			{
				new_node = bmd_metadata2xml(bmdDatagram->pkiMetaData[i], base64Coding);
	
				if(!(new_node != NULL))
				{
					PRINT_ERROR("Cannot create new node!\n");
					abort();
				}
				if(!(xmlAddChild(metadata, new_node) != NULL))
				{
					PRINT_ERROR("Cannot add new node!\n");
					abort();
				}
			}
		}
	}

	/* additional metadata */
	if (bmdDatagram->additionalMetaData)
	{
		metadata = xmlNewNode(NULL, (xmlChar*) "AdditionalMetadata");
		if(!(metadata != NULL))
		{
			PRINT_ERROR("Cannot create AdditionalMetadata node!\n");
			abort();
		}

		if(!(xmlAddChild(node, metadata) != NULL))
		{
			PRINT_ERROR("Cannot add AdditionalMetadata node!\n");
			abort();
		}
		for (i = 0; i < bmdDatagram->no_of_additionalMetaData; i++)
		{
			allowedMetadataFlag = 0;
			if(allowedOids != NULL)
			{
				bmdStringArray_GetItemsCount(allowedOids, &oidsCount);
				for(iter=0; iter<oidsCount; iter++)
				{
					bmdStringArray_GetItemPtr(allowedOids, iter, &oidPtr);
					if(strcmp(oidPtr, bmdDatagram->additionalMetaData[i]->OIDTableBuf ) == 0)
					{
						allowedMetadataFlag = 1;
						oidPtr = NULL;
						break;
					}
					oidPtr = NULL;
				}
				
			}
			else
			{
				allowedMetadataFlag = 1;
			}

			if(allowedMetadataFlag == 1)
			{
				xmlNodePtr new_node;
	
				new_node = bmd_metadata2xml(bmdDatagram->additionalMetaData[i], base64Coding);
	
				if(!(new_node != NULL))
				{
					PRINT_ERROR("Cannot create new node!\n");
					abort();
				}
				if(!(xmlAddChild(metadata, new_node) != NULL))
				{
					PRINT_ERROR("Cannod add new node!\n");
					abort();
				}
			}
		}
	}

	return node;
}


/* **************************** CONVERT XML TO DATAGRAM **************************** */

/**
Funkcja szuka w danym buforze tagu. W wypadku, gdy znajdzie, zwraca 0 i
ustawia \c tag_start na znacznik \< a \v tag_end na znacznik \>. Ponadto ustawia
\c prev_tag_end=tag_end


Jeśli tag nie zostanie znaleziony, zawartość \c tag_start i \c tag_end nie są
zmieniane. \c prev_tag_end jest zaś ustawiany na ostatnie wystąpienie znaku \> lub \<
w przeszukiwanym buforze.

Jeśli w buforze nie znaleziono znaku \>, to \c prev_tag_end nie jest zmieniany.

@param tag poszukiwany tag, na przykład "Document" lub "/Document".
@param buffer bufor do przeszukania
@param bufflen długość bufora
@param tag_start w wypadku powodzenia zostanie tutaj wpisany adres początku
danego tagu
@param tag_end w wypadku powodzenia zostanie tutaj wpisany adres końca danego
tagu
@param prev_tag_end zostanie tu wpisany wskaźnik na ostatni znak \> lub \<
występujący w buforze

@retval 0 w wypadku sukcesu
@retval ERR(ERR_parser("Tag not found","LK")

@test szukamy Document podajemy <Document1> <Documen> i spacje itd.
\internal

Funkcja korzysta z maszyny stanów o następujących stanach:
- \c tag_state=-5 -- nie ten tag, szukaj \>
- \c tag_state=-4 -- tag zaakcpetowany
- \c tag_state=-3 -- jesteśmy w pojedynczym apostrofie "
- \c tag_state=-2 -- jesteśmy w podwójnym apostrofie '
- \c tag_state=-1 -- jesteśmy poza wszelkimi tagami lub
- \c tag_state=0 - rozpoczął się jakiś tag
- \c tag_state>0 - zgodność z \c tag_state 'ym znakiem szukanego tagu

*/
long bmdxml_findTag(char *buffer, long bufflen, const char *tag,
		   char **tag_start, char **tag_end, char **prev_tag_end)
{
	char *ptr = NULL;
	char *start = *tag_start;

	long tag_state = -1;/**< stan w maszynie stanów */

	bmd_assert(tag_start != NULL && tag_end != NULL
		   && *tag_start != NULL && *tag_end != NULL
		   && prev_tag_end != NULL, "LK");
	bmd_assert(tag != NULL && *tag != 0 && buffer != NULL
		   && bufflen >= 0, "LK");

	if (bufflen < 0) {
		return ERR(ERR_arg, "Invalid argument 3", "LK");
	}

	for (ptr = buffer; ptr < buffer + bufflen; ptr++) {
		if (tag_state == -1) {	/* szukamy znaku < */
			if (*ptr == '<') {
				tag_state = 0;	/* znaleźliśmy */
				start = ptr;
				while (ptr + 1 < buffer + bufflen && (*(ptr + 1) == ' ' || *(ptr + 1) == '\t')) {	/*strip white tags */
					ptr++;
				}
			}
		} else if (tag_state >= 0) {	/* jesteśmy wewnątrz tagu */
			if (*(tag + tag_state) == 0) {
				tag_state = -4;	/* tag prawie zaakceptowany */
			} else if (*ptr == *(tag + tag_state)) {	/* jesteśmy wewnątrz tagu */
				tag_state++;
			} else {	/* to nie ten tag :(  */
				tag_state = -5;
			}
		} else if (tag_state == -5) {	/* szukamy  */


		}
		if (*(tag + tag_state) == 0) {
			if (*(ptr + 1) != ' ' && *(ptr + 1) != '>'
			    && *(ptr + 1) != '\t') {
				tag_state = -1;
			} else {	/* Yeah! Mamy nasz tag! */
				while ((((ptr + 1) < (buffer + bufflen)) && (*(ptr + 1) != '>')) || (*(ptr + 1) == '\t')) {	/* znajdź tag > */
					ptr++;
				}
				*tag_start = start;
			}
		}
	}
	return (0);
}

