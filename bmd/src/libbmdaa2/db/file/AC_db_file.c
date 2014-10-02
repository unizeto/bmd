#include <bmd/libbmdaa2/db/file/AC_db_file.h>
#include <bmd/libbmderr/libbmderr.h>
/************************************************/
/* Odczyt atrybutów z pliku			*/
/************************************************/

/* Funkcja parsuje plik filename w poszukiwaniu atrybutów,
   dla wskazanego certyfikatu klucza publicznego.
   Certyfikat ten wskazuje jego numer seryjny i nazwa wystawcy.
   W efekcie generowana jest struktura AttributesSequence_t,
   którą można bezpośrednio wstawic do certyfikatu atrybutów
   lub zserializować i wysłać do AAauthority by zrobiło z tego
   certyfikat atrybutów */
long AA_ReadAttributesFromFile(const char *filename,
					const char *CertificateSerialNr,
					const char *CertificateIssuerDN,
					const ConfigData_t *ConfigData,
					AttributesSequence_t **AttributesSequence)
{
long err = 0;
GenBuf_t *AttributesDB = NULL;
GenBuf_t *AttributesDBUserLine = NULL;

	PRINT_INFO("LIBBMDAACONFINF Reading attributes from file %s\n",filename);
	if(filename == NULL)
	{
		PRINT_ERROR("LIBBMDAA2ERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(CertificateSerialNr == NULL)
	{
		PRINT_ERROR("LIBBMDAA2ERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
	if(CertificateIssuerDN == NULL)
	{
		PRINT_ERROR("LIBBMDAA2ERR Invalid third parameter value. Error=%i\n",BMD_ERR_PARAM3);
		return BMD_ERR_PARAM3;
	}
	if(ConfigData == NULL)
	{
		PRINT_ERROR("LIBBMDAA2ERR Invalid fourth parameter value. Error=%i\n",BMD_ERR_PARAM4);
		return BMD_ERR_PARAM4;
	}
	if(AttributesSequence == NULL || *AttributesSequence != NULL)
	{
		PRINT_ERROR("LIBBMDAA2ERR Invalid fifth parameter value. Error=%i\n",BMD_ERR_PARAM5);
		return BMD_ERR_PARAM5;
	}

	err=bmd_load_binary_content(filename,&AttributesDB);
	if(err < 0)
	{
		PRINT_ERROR("LIBBMDAADBFILEERR Error in reading %s file. Please check if file exists in location specified in uniac.conf. Error=%li\n", filename, err);
		return err;
	}
	PRINT_INFO("LIBBMDAADBFILEINF %s file read.\n", filename);

	err = AA_FindUserInAttributesDB(AttributesDB, CertificateSerialNr,CertificateIssuerDN, &AttributesDBUserLine);
	if(err<0)
	{
		PRINT_ERROR("LIBBMDAADBFILEERR Error in finding and converting user [%s][%s]in %s. Error=%li\n",CertificateSerialNr, CertificateIssuerDN, filename, err);
		return ERR_AA_ATTRIBUTESDB_RECORD_NOT_FOUND;
	}
	free_GenBuf(&AttributesDB);free(AttributesDB);AttributesDB=NULL;
	PRINT_INFO("LIBBMDAADBFILEINF User [%s][%s] found in %s.\n",CertificateSerialNr, CertificateIssuerDN, filename);

	err = AA_ReadAttributesFromLine(AttributesDBUserLine, AttributesSequence, ConfigData);
	if(err<0)
	{
		PRINT_ERROR("LIBBMDAADBFILEERR Error in reading attributes from file database. User [%s][%s]in %s. Error= %li\n",CertificateSerialNr, CertificateIssuerDN, filename, err);
		return err;
	}
	free_GenBuf(&AttributesDBUserLine);free(AttributesDBUserLine);AttributesDBUserLine=NULL;
	return 0;
}

long AA_FindUserInAttributesDB(GenBuf_t *AttributesDB,
			const char *CertificateSerialNr,
			const char *CertificateIssuerDN,
			GenBuf_t **AttributesDBUserLine)
{
long err = 0;
char *PatternUserline = NULL;
SubstringsGenBuf_t UserlineSubstrings;

	PRINT_INFO("LIBBMDAACONFINF Finding user in attributes\n");
	if(AttributesDB == NULL)
	{
		PRINT_ERROR("LIBBMDAA2ERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(CertificateSerialNr == NULL)
	{
		PRINT_ERROR("LIBBMDAA2ERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
	if(CertificateIssuerDN == NULL)
	{
		PRINT_ERROR("LIBBMDAA2ERR Invalid third parameter value. Error=%i\n",BMD_ERR_PARAM3);
		return BMD_ERR_PARAM3;
	}
	if(AttributesDBUserLine == NULL || *AttributesDBUserLine != NULL)
	{
		PRINT_ERROR("LIBBMDAA2ERR Invalid fourth parameter value. Error=%i\n",BMD_ERR_PARAM4);
		return BMD_ERR_PARAM4;
	}

	/* Szukam klienta o danym serialu + DN i pobieram odpowiednia linie z bazy  */
	asprintf(&PatternUserline, "^%s:%s:.*$", CertificateSerialNr, CertificateIssuerDN);
	if(PatternUserline == NULL)
	{
		PRINT_ERROR("LIBBMDAA2ERR Memory error. Error=%i\n",NO_MEMORY);
		return NO_MEMORY;
	}

/*
	long i=0;
	printf("AttributesDB->size=%i\n",AttributesDB->size);
	printf("\n\n %s \n\n",AttributesDB->buf);
*/

	err = find_substrings(AttributesDB, PatternUserline,&UserlineSubstrings, MULTILINE, NO);
	if(err != 0)
	{
		PRINT_ERROR("LIBBMDAADBFILEERR Certificate with serial: [%s] and issuer: [%s] not found by find_substrings. Error=%li\n",CertificateSerialNr, CertificateIssuerDN, err);
		return err;
	}
	if(PatternUserline){ free(PatternUserline); PatternUserline = NULL; }

	err = String2GenBuf_alloc(UserlineSubstrings.substrings_table[0], AttributesDBUserLine);
	if(err<0)
	{
		PRINT_ERROR("LIBBMDAADBFILEERR Error in converting string to GenBuf_t. Error=%li\n", err);
		return err;
	}
	free_substrings(&UserlineSubstrings);
	return err;
}

long AA_ReadAttributesFromLine(GenBuf_t *AttributesLine, AttributesSequence_t **AttributesSequence,
				const ConfigData_t *ConfigData)
{
long err = 0;

	PRINT_INFO("LIBBMDAACONFINF Reading attributes from line\n");
	if(AttributesLine == NULL || AttributesLine->buf == NULL || AttributesLine->size < 0)
	{
		PRINT_ERROR("LIBBMDAA2ERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(AttributesSequence == NULL || *AttributesSequence != NULL)
	{
		PRINT_ERROR("LIBBMDAA2ERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
	if(ConfigData == NULL)
	{
		PRINT_ERROR("LIBBMDAA2ERR Invalid third parameter value. Error=%i\n",BMD_ERR_PARAM3);
		return BMD_ERR_PARAM3;
	}

	(*AttributesSequence)=(AttributesSequence_t *)malloc(sizeof(AttributesSequence_t));
	if( (*AttributesSequence) == NULL )
		BMD_FOK(BMD_ERR_MEMORY);
	memset(*AttributesSequence,0,sizeof(AttributesSequence_t));
	err = AA_ReadGroupsFromLine(AttributesLine, *AttributesSequence, ConfigData);
	if(err<0 && err != ERR_AA_ATTRIBUTESDB_GROUPS_NOT_FOUND)
	{
		PRINT_ERROR("LIBBMDAADBFILEERR Error in reading groups from database file. Error=%li\n", err);
		return err;
	}
	err = AA_ReadRolesFromLine(AttributesLine, *AttributesSequence, ConfigData);
	if(err<0 && err != ERR_AA_ATTRIBUTESDB_ROLES_NOT_FOUND)
	{
		PRINT_ERROR("LIBBMDAADBFILEERR Error in reading roles from database file. Error=%li\n", err);
		return err;
	}
	err = AA_ReadClearancesFromLine(AttributesLine, *AttributesSequence, ConfigData);
	if(err<0 && err != ERR_AA_ATTRIBUTESDB_CLEARANCES_NOT_FOUND)
	{
		PRINT_ERROR("LIBBMDAADBFILEERR Error in reading clearances from database file. Error= %li\n", err);
		return err;
	}
	err = AA_ReadUnizetosFromLine(AttributesLine, *AttributesSequence, ConfigData);
	if(err<0 && err != ERR_AA_ATTRIBUTESDB_UNIZETOS_NOT_FOUND)
	{
		PRINT_ERROR("LIBBMDAADBFILEERR Error in reading unizeto attributes from database file. Error=%li\n", err);
		return err;
	}
	err = 0;
	/*asn_fprint(stdout, &asn_DEF_AttributesSequence, *AttributesSequence);*/
	return err;
}

/********************************/
/* Grupy			*/
/********************************/
long AA_ReadGroupsFromLine(GenBuf_t *AttributesLine, AttributesSequence_t *AttributesSequence,
				const ConfigData_t *ConfigData)
{
long err = 0;
char *group_pattern     = "group=([^:]*)";
SubstringsGenBuf_t tmpSubstrings;
GenBufList_t *GenBufList = NULL;

	PRINT_INFO("LIBBMDAACONFINF Reading group from line\n");
	if(AttributesLine == NULL || AttributesLine->buf == NULL || AttributesLine->size < 0)
	{
		PRINT_ERROR("LIBBMDAA2ERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(AttributesSequence == NULL)
	{
		PRINT_ERROR("LIBBMDAA2ERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
	if(ConfigData == NULL)
	{
		PRINT_ERROR("LIBBMDAA2ERR Invalid third parameter value. Error=%i\n",BMD_ERR_PARAM3);
		return BMD_ERR_PARAM3;
	}

	err = find_substrings(AttributesLine, group_pattern,&tmpSubstrings, SINGLELINE, YES);
	if(err != 0)
	{
		PRINT_ERROR("LIBBMDAADBFILEINF No group substrings found. Error=%li\n", err);
		return ERR_AA_ATTRIBUTESDB_GROUPS_NOT_FOUND;
	}
	err = Substrings2GenBufList(&tmpSubstrings, &GenBufList, 2, 1, 1);
	if(err<0)
	{
		PRINT_ERROR("LIBBMDAADBFILEERR Error in converting Substrings into GenBuflist. Error=%li\n", err);
		return err;
	}
	free_substrings(&tmpSubstrings);
	err = AC_MakeAttribute_IetfAttrSyntax(GenBufList, AttributesSequence, ConfigData);
	if(err<0)
	{
		PRINT_ERROR("LIBBMDAADBFILEERR Error in making attribute IetfAttrSyntax. Error=%li\n", err);
		return err;
	}
	free_GenBufList(&GenBufList);free(GenBufList);GenBufList=NULL;
	return err;
}

/********************************/
/* Role				*/
/********************************/
long AA_ReadRolesFromLine(GenBuf_t *AttributesLine, AttributesSequence_t *AttributesSequence,
				const ConfigData_t *ConfigData)
{
long err = 0;
char *role_pattern     = "role=([^:]*)";
SubstringsGenBuf_t tmpSubstrings;
GenBufList_t *GenBufList = NULL;

	PRINT_INFO("LIBBMDAACONFINF Reading role from line\n");
	if(AttributesLine == NULL || AttributesLine->buf == NULL || AttributesLine->size < 0)
	{
		PRINT_ERROR("LIBBMDAA2ERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(AttributesSequence == NULL)
	{
		PRINT_ERROR("LIBBMDAA2ERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
	if(ConfigData == NULL)
	{
		PRINT_ERROR("LIBBMDAA2ERR Invalid third parameter value. Error=%i\n",BMD_ERR_PARAM3);
		return BMD_ERR_PARAM3;
	}

	err = find_substrings(AttributesLine, role_pattern,&tmpSubstrings, SINGLELINE, YES);
	if(err != 0)
	{
		PRINT_ERROR("LIBBMDAADBFILEINF No role substrings found. Error=%li\n", err);
		return ERR_AA_ATTRIBUTESDB_ROLES_NOT_FOUND;
	}
	err = Substrings2GenBufList(&tmpSubstrings, &GenBufList, 2, 1, 1);
	if(err<0)
	{
		PRINT_ERROR("LIBBMDAADBFILEERR Error in converting Substrings into GenBuflist. Error=%li\n", err);
		return err;
	}
	free_substrings(&tmpSubstrings);
	err = AC_MakeAttribute_RoleSyntax(GenBufList, AttributesSequence, ConfigData);
	if(err<0)
	{
		PRINT_ERROR("LIBBMDAADBFILEERR Error in making attribute RoleSyntax. Error=%li\n", err);
		return err;
	}
	free_GenBufList(&GenBufList);
	return err;
}

/********************************/
/* Clearance			*/
/********************************/
long AA_ReadClearancesFromLine(	GenBuf_t *AttributesLine,
						AttributesSequence_t *AttributesSequence,
						const ConfigData_t *ConfigData)
{
long err				= 0;
GenBufList_t *GenBufList	= NULL;
char *clearance_pattern		= "clearance=([^:]*),([^:]*),([^:]*)";
SubstringsGenBuf_t tmpSubstrings;
long i=0;

	PRINT_INFO("LIBBMDAACONFINF Reading clearance from line\n");
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(AttributesLine == NULL || AttributesLine->buf == NULL || AttributesLine->size < 0)
	{
		PRINT_ERROR("LIBBMDAA2ERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(AttributesSequence == NULL)
	{
		PRINT_ERROR("LIBBMDAA2ERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
	if(ConfigData == NULL)
	{
		PRINT_ERROR("LIBBMDAA2ERR Invalid third parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	err = find_substrings(AttributesLine, clearance_pattern, &tmpSubstrings, SINGLELINE, YES);
	if(err != 0)
	{
		PRINT_ERROR("LIBBMDAADBFILEINF No clearance substrings found. Error=%li\n", err);
		return ERR_AA_ATTRIBUTESDB_CLEARANCES_NOT_FOUND;
	}
	err = Substrings2GenBufList(&tmpSubstrings, &GenBufList, 4, 1, 3);
	if(err<0)
	{
		PRINT_ERROR("LIBBMDAADBFILEERR Error in converting Substrings into GenBuflist. Error=%li\n", err);
		return err;
	}
	free_substrings(&tmpSubstrings);

	for(i=0; i<GenBufList->size; i++)
	{
		/*PRINT_TEST("[Gbuf %d]:%s\n", i, GenBufList->gbufs[i]->buf);*/
	}




	err = AC_MakeAttribute_Clearance(GenBufList, AttributesSequence, ConfigData);
	if(err<0)
	{
		PRINT_ERROR("LIBBMDAADBFILEERR Error in making attribute Clearance. Error=%li\n", err);
		return err;
	}
	free_GenBufList(&GenBufList);
	return err;
}

/********************************/
/* UnizetoAttrSyntax		*/
/********************************/
long AA_ReadUnizetosFromLine(GenBuf_t *AttributesLine, AttributesSequence_t *AttributesSequence,
				const ConfigData_t *ConfigData)
{
long err = 0;
char *unizeto_pattern     = "UnizetoAttrSyntax=\\{(\\|[^:]*\\([^:]*\\))*\\|\\}";
SubstringsGenBuf_t tmpSubstrings;
GenBufList_t *GenBufList = NULL;
UnizetoAttrSyntaxInList_t *UnizetoAttrSyntaxInList = NULL;

	PRINT_INFO("LIBBMDAACONFINF Reading unizetos from line\n");
	if(AttributesLine == NULL || AttributesLine->buf == NULL || AttributesLine->size < 0)
	{
		PRINT_ERROR("LIBBMDAA2ERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(AttributesSequence == NULL)
	{
		PRINT_ERROR("LIBBMDAA2ERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
	if(ConfigData == NULL)
	{
		PRINT_ERROR("LIBBMDAA2ERR Invalid third parameter value. Error=%i\n",BMD_ERR_PARAM3);
		return BMD_ERR_PARAM3;
	}

	err = find_substrings(AttributesLine, unizeto_pattern,
			&tmpSubstrings, SINGLELINE, YES);
	if(err != 0)
	{
		PRINT_ERROR("LIBBMDAADBFILEINF No unizeto attribute syntax substrings found. Error=%li\n", err);
		return ERR_AA_ATTRIBUTESDB_UNIZETOS_NOT_FOUND;
	}
	/*for(i=0; i<tmpSubstrings.no_of_substrings; i++){
		printf("Substring[%d] = %s\n", i , tmpSubstrings.substrings_table[i]);
	}*/
	err = Substrings2GenBufList(&tmpSubstrings, &GenBufList, 2, 1, 1);
	if(err<0)
	{
		PRINT_ERROR("LIBBMDAADBFILEERR Error in converting Substrings into GenBuflist. Error=%li\n", err);
		return err;
	}
	free_substrings(&tmpSubstrings);
	/*for(i=0; i<GenBufList->size; i++){
		printf("[Gbuf %d]:%s\n", i, GenBufList->gbufs[i]->buf);
	}*/
	err = GenBufList2UnizetoAttrSyntaxInList(GenBufList, &UnizetoAttrSyntaxInList);
	if(err<0)
	{
		PRINT_ERROR("LIBBMDAADBFILEERR Error in converting GenBuflist into UnizetoAttrSyntaxInList. Error=%li\n", err);
		return err;
	}
	err = AC_MakeAttribute_UnizetoAttrSyntax(UnizetoAttrSyntaxInList,AttributesSequence,ConfigData);
	if(err<0)
	{
		PRINT_ERROR("LIBBMDAADBFILEERR Error in making attribute UnizetoAttrSyntax. Error=%li\n", err);
		return err;
	}
	UnizetoAttrSyntaxInList_free_ptr(&UnizetoAttrSyntaxInList);
	free_GenBufList(&GenBufList);
	return err;
}

/********************************/
/* Utilities			*/
/********************************/
/* Funkcja konwertuję strukturę SubstringsGenBuf_t do GenBufList_t
   wybierając elementy począwszy od "offset" z krokiem "step". W paczce znajduje
   się "setsize" kolejnych elementów. Np. dla elementow od 0 do 9, jesli offset=1
   step=4 i setsize=2 to wybrane beda elementy 1,2,5,6,9
   Elementy są kopiowane. */
long Substrings2GenBufList(SubstringsGenBuf_t *SubstringsGenBuf, GenBufList_t **GenBufList, long step, long offset, long setsize)
{
long err = 0;
long i = 0;
long m = 0;
long counter = 0;
long NoOfGenBufs= 0;

	PRINT_INFO("LIBBMDAACONFINF Converting substrings to genbuf list\n");
	if(SubstringsGenBuf == NULL)
	{
		PRINT_ERROR("LIBBMDAA2ERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(GenBufList == NULL || *GenBufList != NULL)
	{
		PRINT_ERROR("LIBBMDAA2ERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
	if(step<1)
	{
		PRINT_ERROR("LIBBMDAA2ERR Invalid third parameter value. Error=%i\n",BMD_ERR_PARAM3);
		return BMD_ERR_PARAM3;
	}
	if(setsize<1)
	{
		PRINT_ERROR("LIBBMDAA2ERR Invalid fourth parameter value. Error=%i\n",BMD_ERR_PARAM4);
		return BMD_ERR_PARAM4;
	}

	*GenBufList = (GenBufList_t *) malloc (sizeof(GenBufList_t));
	if(*GenBufList == NULL)
	{
		PRINT_ERROR("LIBBMDAA2ERR Memory error. Error=%i\n",NO_MEMORY);
		return NO_MEMORY;
	}
	memset(*GenBufList, 0, sizeof(GenBufList_t));

	if(SubstringsGenBuf->no_of_substrings < 0)
	{
		(*GenBufList)->gbufs = 0;
		(*GenBufList)->size = 0;
		PRINT_ERROR("LIBBMDAA2ERR No substrings to convert\n");
		return err;
	}
	NoOfGenBufs = SubstringsGenBuf->no_of_substrings / step * setsize;
	(*GenBufList)->size = NoOfGenBufs;
	(*GenBufList)->gbufs = (GenBuf_t **) malloc (sizeof(GenBuf_t *) * NoOfGenBufs);
	if((*GenBufList)->gbufs == NULL)
	{
		PRINT_ERROR("LIBBMDAA2ERR Memory error. Error=%i\n",NO_MEMORY);
		return NO_MEMORY;
	}
	for(i=0; i<NoOfGenBufs; i++)
	{
		(*GenBufList)->gbufs[i] = (GenBuf_t *) malloc (sizeof(GenBuf_t));
		if((*GenBufList)->gbufs[i] == NULL)
		{
			PRINT_ERROR("LIBBMDAA2ERR Memory error. Error=%i\n",NO_MEMORY);
			return NO_MEMORY;
		}
		memset((*GenBufList)->gbufs[i], 0, sizeof(GenBuf_t));
	}
	for(i=offset; i<SubstringsGenBuf->no_of_substrings; i=i+step)
	{
		for(m=0; m<setsize; m++)
		{
			if(i+m >= SubstringsGenBuf->no_of_substrings)
			{
				break;
			}
			(*GenBufList)->gbufs[counter]->buf = (char *) malloc(sizeof(char) * (1+strlen(SubstringsGenBuf->substrings_table[i+m])));
			if((*GenBufList)->gbufs[counter]->buf == NULL)
			{
				PRINT_ERROR("LIBBMDAA2ERR Memory error. Error=%i\n",NO_MEMORY);
				return NO_MEMORY;
			}
			memmove((*GenBufList)->gbufs[counter]->buf, SubstringsGenBuf->substrings_table[i+m],1+strlen(SubstringsGenBuf->substrings_table[i+m]));
			(*GenBufList)->gbufs[counter]->size = 1+(int)strlen(SubstringsGenBuf->substrings_table[i+m]);
			counter++;
		}
	}
	return err;
}

/* Funkcja konwertujaca string z pliku attributes.db do struktury podawanej na wejscie	*/
/* kompozytora UnizetoAttrSyntax	*/
long GenBuf2UnizetoAttrSyntaxInput(GenBuf_t *GenBuf, UnizetoAttrSyntaxIn_t **UnizetoAttrSyntaxIn)
{
long err = 0;
char *unizeto_pattern_detailed = "\\|([^\\|]*)\\(([^\\)]*)\\)";
SubstringsGenBuf_t tmpSubstrings;
GenBufList_t *SlotsGenBufList = NULL;

	PRINT_INFO("LIBBMDAACONFINF Converting string from db file to structure\n");
	if(GenBuf == NULL || GenBuf->size < 0)
	{
		PRINT_ERROR("LIBBMDAA2ERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(UnizetoAttrSyntaxIn == NULL || *UnizetoAttrSyntaxIn != NULL)
	{
		PRINT_ERROR("LIBBMDAA2ERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}


	err = find_substrings(GenBuf, unizeto_pattern_detailed,
			&tmpSubstrings, SINGLELINE, YES);	
	if(err<0)
	{
		PRINT_ERROR("LIBBMDAADBFILEINF No unizeto slots found. Error=%li\n", err);
		return ERR_AA_ATTRIBUTESDB_UNIZETO_SLOTS_NOT_FOUND;
	}
	/*for(i=0; i<tmpSubstrings.no_of_substrings; i++)
		printf("Slot %d: %s\n", i, tmpSubstrings.substrings_table[i]);*/
	err = Substrings2GenBufList(&tmpSubstrings, &SlotsGenBufList, 3, 1, 2);
	if(err<0)
	{
		PRINT_ERROR("LIBBMDAADBFILEERR Error in converting Substrings into GenBuflist. Error=%li\n", err);
		return err;
	}
	err = CreateEmptyUnizetoAttrSyntaxIn(SlotsGenBufList, UnizetoAttrSyntaxIn);
	if(err<0)
	{
		PRINT_ERROR("LIBBMDAADBFILEERR Error in creating empty UnizetoAttrSyntaxIn_t. Error=%li\n", err);
		return err;
	}
	err = UnizetoAttrSyntaxIn_add_SlotsIn(SlotsGenBufList, *UnizetoAttrSyntaxIn);
	if(err<0)
	{
		PRINT_ERROR("LIBBMDAADBFILEERR Error in adding SlotsIn_t into UnizetoAttrSyntaxIn_t. Error=%li\n", err);
		return err;
	}
	/*for(i=0; i<SlotsGenBufList->size; i++){
		printf("[Result %d]:%s\n", i, SlotsGenBufList->gbufs[i]->buf);
	}*/
	free_substrings(&tmpSubstrings);
	free_GenBufList(&SlotsGenBufList);
	return err;
}

long CreateEmptyUnizetoAttrSyntaxIn(GenBufList_t *SlotsGenBufList, UnizetoAttrSyntaxIn_t **UnizetoAttrSyntaxIn)
{
long err=0;

	PRINT_INFO("LIBBMDAACONFINF Creating empty unizeto attribute syntax\n");
	if(SlotsGenBufList == NULL)
	{
		PRINT_ERROR("LIBBMDAA2ERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(UnizetoAttrSyntaxIn == NULL || *UnizetoAttrSyntaxIn != NULL)
	{
		PRINT_ERROR("LIBBMDAA2ERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	*UnizetoAttrSyntaxIn = (UnizetoAttrSyntaxIn_t *) malloc (sizeof(UnizetoAttrSyntaxIn_t));
	if(*UnizetoAttrSyntaxIn == NULL)
	{
		PRINT_ERROR("LIBBMDAA2ERR Memory error. Error=%i\n",NO_MEMORY);
		return NO_MEMORY;
	}
	memset(*UnizetoAttrSyntaxIn, 0, sizeof(UnizetoAttrSyntaxIn_t));

	(*UnizetoAttrSyntaxIn)->SyntaxOid = (GenBuf_t *) malloc (sizeof(GenBuf_t));
	if((*UnizetoAttrSyntaxIn)->SyntaxOid == NULL)
	{
		PRINT_ERROR("LIBBMDAA2ERR Memory error. Error=%i\n",NO_MEMORY);
		return NO_MEMORY;
	}
	memset((*UnizetoAttrSyntaxIn)->SyntaxOid, 0, sizeof(GenBuf_t));

	asprintf((char **)&((*UnizetoAttrSyntaxIn)->SyntaxOid->buf), "%s", SlotsGenBufList->gbufs[1]->buf);
	(*UnizetoAttrSyntaxIn)->SyntaxOid->size = SlotsGenBufList->gbufs[1]->size;
	return err;
}

long UnizetoAttrSyntaxIn_add_SlotsIn(GenBufList_t *SlotsGenBufList, UnizetoAttrSyntaxIn_t *UnizetoAttrSyntaxIn)
{
long err = 0;
long i = 0;
long no_of_Slots = 0;
long counter = 0;

	PRINT_INFO("LIBBMDAACONFINF Adding unizeto syntax to slot\n");
	if(SlotsGenBufList == NULL || SlotsGenBufList->size < 2)
	{
		PRINT_ERROR("LIBBMDAA2ERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(UnizetoAttrSyntaxIn == NULL)
	{
		PRINT_ERROR("LIBBMDAA2ERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	/* Pierwsze dwa GenBuf'y na liscie zawierają string "OID" i wartość OIDa	*/
	/* symbolizującego składnię atrybutu UnizetoAttrSyntax.				*/
	/* Pozostałe GenBufy zawierają na przemian OIDy typu Slotu i oddzielone		*/
	/* przecinkami listy wartości slotów.						*/
	UnizetoAttrSyntaxIn->SlotList = (SlotListIn_t *) malloc (sizeof(SlotListIn_t));
	if(UnizetoAttrSyntaxIn->SlotList == NULL)
	{
		PRINT_ERROR("LIBBMDAA2ERR Memory error. Error=%i\n",NO_MEMORY);
		return NO_MEMORY;
	}
	memset(UnizetoAttrSyntaxIn->SlotList, 0, sizeof(SlotListIn_t));

	/* Utworz tablice slotów */
	no_of_Slots = (SlotsGenBufList->size - 2)/2;
	UnizetoAttrSyntaxIn->SlotList->size = no_of_Slots;
	UnizetoAttrSyntaxIn->SlotList->Slots = (SlotIn_t **) malloc (sizeof(SlotIn_t *) * no_of_Slots);
	if(UnizetoAttrSyntaxIn->SlotList->Slots == NULL)
	{
		PRINT_ERROR("LIBBMDAA2ERR Memory error. Error=%i\n",NO_MEMORY);
		return NO_MEMORY;
	}
	memset(UnizetoAttrSyntaxIn->SlotList->Slots, 0, sizeof(SlotIn_t *) * no_of_Slots);

	/* Wypelnij sloty danymi */
	for(i=2; i<SlotsGenBufList->size; i=i+2)
	{
		/*printf("[Result %d]:%s\n", i, SlotsGenBufList->gbufs[i]->buf);*/
		err = InsertDataIntoSlotIn(SlotsGenBufList->gbufs[i], 			/* OID slotu		*/
					SlotsGenBufList->gbufs[i+1],			/* lista wartosci slotu */
					&(UnizetoAttrSyntaxIn->SlotList->Slots[counter])); /* slot do wypelnienia*/
		if(err<0)
		{
			PRINT_ERROR("LIBBMDAADBFILEERR Error in creating SlotIn inside UnizetoAttrSyntaxIn_t. Error=%li\n", err);
			return err;
		}
		counter++;
	}
	return err;
}

long InsertDataIntoSlotIn(GenBuf_t *SlotOid, GenBuf_t *SlotValuesString, SlotIn_t **SlotIn)
{
long err = 0;

	PRINT_INFO("LIBBMDAACONFINF Inserting data into slot\n");
	if(SlotOid == NULL)
	{
		PRINT_ERROR("LIBBMDAA2ERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(SlotValuesString == NULL)
	{
		PRINT_ERROR("LIBBMDAA2ERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
	if(SlotIn == NULL || *SlotIn != NULL)
	{
		PRINT_ERROR("LIBBMDAA2ERR Invalid third parameter value. Error=%i\n",BMD_ERR_PARAM3);
		return BMD_ERR_PARAM3;
	}

	*SlotIn = (SlotIn_t *) malloc (sizeof(SlotIn_t));
	if(*SlotIn == NULL)
	{
		PRINT_ERROR("LIBBMDAA2ERR Memory error. Error=%i\n",NO_MEMORY);
		return NO_MEMORY;
	}
	memset(*SlotIn, 0, sizeof(SlotIn_t));

	err = SetOidInSlot(SlotOid, *SlotIn);
	if(err<0)
	{
		PRINT_ERROR("LIBBMDAADBFILEERR Error in setting OID in Slot. Error=%li\n", err);
		return err;
	}
	err = SetValuesInSlot(SlotValuesString, *SlotIn);
	if(err<0)
	{
		PRINT_ERROR("LIBBMDAADBFILEERR Error in setting values in Slot.Error=%li\n", err);
		return err;
	}
	/*for(i=0; i<(*SlotIn)->ValuesList->size; i++){
		printf("WOW[%d] %s [%i]\n", i, (*SlotIn)->ValuesList->gbufs[i]->buf, (*SlotIn)->ValuesList->gbufs[i]->size);
	}*/
	return err;
}

long SetOidInSlot(GenBuf_t *SlotOid, SlotIn_t *SlotIn)
{
long err = 0;

	PRINT_INFO("LIBBMDAACONFINF Setting oid in slot\n");
	if(SlotOid == NULL)
	{
		PRINT_ERROR("LIBBMDAA2ERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(SlotIn == NULL)
	{
		PRINT_ERROR("LIBBMDAA2ERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	SlotIn->OidString = (GenBuf_t *) malloc (sizeof(GenBuf_t));
	if(SlotIn->OidString == NULL)
	{
		PRINT_ERROR("LIBBMDAA2ERR Memory error. Error=%i\n",NO_MEMORY);
		return NO_MEMORY;
	}
	memset(SlotIn->OidString, 0, sizeof(GenBuf_t));

	SlotIn->OidString->buf = (char *) malloc (sizeof(char) * SlotOid->size);
	if(SlotIn->OidString->buf == NULL)
	{
		PRINT_ERROR("LIBBMDAA2ERR Memory error. Error=%i\n",NO_MEMORY);
		return NO_MEMORY;
	}
	SlotIn->OidString->size = SlotOid->size;
	memmove(SlotIn->OidString->buf, SlotOid->buf, SlotOid->size);
	return err;
}

long SetValuesInSlot(GenBuf_t *SlotValuesString, SlotIn_t *SlotIn)
{
long err = 0;

	PRINT_INFO("LIBBMDAACONFINF Setting values in slot\n");
	if(SlotValuesString == NULL)
	{
		PRINT_ERROR("LIBBMDAA2ERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(SlotIn == NULL)
	{
		PRINT_ERROR("LIBBMDAA2ERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	err = CutString2GenBufList_alloc((char *)SlotValuesString->buf, &(SlotIn->ValuesList));
	if(err<0)
	{
		PRINT_ERROR("LIBBMDAADBFILEERR Error in converting String into GenBufList_t. Error=%li\n", err);
		return err;
	}
	return err;
}

long GenBufList2UnizetoAttrSyntaxInList(GenBufList_t *GenBufList, UnizetoAttrSyntaxInList_t **UnizetoAttrSyntaxInList)
{
long err = 0;
long i = 0;

	PRINT_INFO("LIBBMDAACONFINF Coverting genbuf list to unizeto syntax in list\n");
	if(GenBufList == NULL)
	{
		PRINT_ERROR("LIBBMDAA2ERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(UnizetoAttrSyntaxInList == NULL || *UnizetoAttrSyntaxInList != NULL)
	{
		PRINT_ERROR("LIBBMDAA2ERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	*UnizetoAttrSyntaxInList = (UnizetoAttrSyntaxInList_t *) malloc (sizeof(UnizetoAttrSyntaxInList_t));
	if(*UnizetoAttrSyntaxInList == NULL)
	{
		PRINT_ERROR("LIBBMDAA2ERR Memory error. Error=%i\n",NO_MEMORY);
		return NO_MEMORY;
	}
	memset(*UnizetoAttrSyntaxInList, 0, sizeof(UnizetoAttrSyntaxInList_t));

	(*UnizetoAttrSyntaxInList)->size = GenBufList->size;
	(*UnizetoAttrSyntaxInList)->UnizetoAttrSyntaxIn = (UnizetoAttrSyntaxIn_t **)malloc (sizeof(UnizetoAttrSyntaxIn_t *) * GenBufList->size);
	if((*UnizetoAttrSyntaxInList)->UnizetoAttrSyntaxIn == NULL)
	{
		PRINT_ERROR("LIBBMDAA2ERR Memory error. Error=%i\n",NO_MEMORY);
		return NO_MEMORY;
	}
	memset((*UnizetoAttrSyntaxInList)->UnizetoAttrSyntaxIn, 0, sizeof(UnizetoAttrSyntaxIn_t *) * GenBufList->size);

	for(i=0; i<GenBufList->size; i++)
	{
		err = GenBuf2UnizetoAttrSyntaxInput(GenBufList->gbufs[i], &((*UnizetoAttrSyntaxInList)->UnizetoAttrSyntaxIn[i]));	
		if(err<0)
		{
			PRINT_ERROR("LIBBMDAADBFILEERR Error in converting GenBuf to UnizetoAttrSyntaxInput_t. Error=%li\n", err);
			return err;
		}
	}
	return err;
}


