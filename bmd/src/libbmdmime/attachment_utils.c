#include <bmd/libbmdmime/libbmdmime.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmderr/libbmderr.h>

/** narzedzia potrzebne przez SMIME P7S API - załączniki, obróbka base64, nazwy zalacznikow z ogonkami
Autor: Włodzimierz Szczygieł (Dział Programowania, Unizeto Technologies SA)
*/


long init_attach_list(bmd_attach_list **list)
{
	if(list == NULL)
	{ 
		PRINT_DEBUG("LIBBMDMIMEERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1; 
	}
	if(*list != NULL)
	{ 
		PRINT_DEBUG("LIBBMDMIMEERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1; 
	}

	*list=NULL;
	return BMD_OK;
}


long add_to_attch_list(bmd_attach_list **list, bmd_mime_attachment *attach)
{
	bmd_attach_list *new_item=NULL;
	bmd_attach_list *iter=NULL;
	
	if(list == NULL)
	{ 
		PRINT_DEBUG("LIBBMDMIMEERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1; 
	}
	if(attach == NULL)
	{ 
		PRINT_DEBUG("LIBBMDMIMEERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	iter=*list;
	
	if(iter == NULL)
	{
		new_item=(bmd_attach_list*)calloc(1, sizeof(bmd_attach_list));
		if(new_item == NULL)
		{ 
			PRINT_DEBUG("LIBBMDMIMEERR Memory error. Error=%i\n",BMD_ERR_MEMORY);
			return BMD_ERR_MEMORY; 
		}
		new_item->item=attach;
		new_item->next=NULL;
		*list=new_item;
	}
	else
	{
		while(iter->next != NULL)
		{ 
			/*jesli zalacznik o tej samej nazwie zostal wczesniej dodany, to nie mozna dodac 2 raz*/
			if( (strcmp(iter->item->file_name, attach->file_name)) == 0)
			{
				PRINT_DEBUG("LIBBMDMIMEERR Attachment already in list. Error=%i\n",BMD_ERR_OP_FAILED);
				return BMD_ERR_OP_FAILED;
			}
			/**/
			iter=iter->next;
		}
		/*jesli zalacznik o tej samej nazwie zostal wczesniej dodany, to nie mozna dodac 2 raz- ostatni element listy*/
		if( (strcmp(iter->item->file_name, attach->file_name)) == 0)
		{
			PRINT_DEBUG("LIBBMDMIMEERR Attachment already in list. Error=%i\n",BMD_ERR_OP_FAILED);
			return BMD_ERR_OP_FAILED;
		}
		/**/
		new_item=(bmd_attach_list*)calloc(1, sizeof(bmd_attach_list));
		if(new_item == NULL)
		{ 
			PRINT_DEBUG("LIBBMDMIMEERR Memory error. Error=%i\n",BMD_ERR_MEMORY);
			return BMD_ERR_MEMORY; 
		}
		new_item->item=attach;
		new_item->next=NULL;
		iter->next=new_item;
	}
	
	return BMD_OK;
}

/**
funkcja remove_from_attach_list() usuwana z listy zaacznikow list, zalacznik o nazwie file_name

@arg list to adres wskaznika do listy zalacznikow
@arg file_name to nazwa zalacznika do usuniecia (moze byc rowniez sciezka bezwzgledna/wzgledna do pliku zalacznika -
			funkcja wyodrebnia nazwe pliku ze sciezki)

@retval 0 jesli usunieto zalacznik z listy
@retval -1 jesli za list przekazano NULL
@retval -2 jesli za file_name przekazano NULL
@retval -3 jesli podano pusty lancuch file_name
@retval -10 jesli nie odnaleziono zalacznika w liscie
*/
long remove_from_attach_list(bmd_attach_list** list, char *file_name)
{
	bmd_attach_list *iter=NULL;
	bmd_attach_list *prev_item=NULL;
	char **substrings=NULL;
	long subs_num=0;
	char *cut_fileName=NULL;
	
	if(list == NULL)
		{return -1;}
	if(*list == NULL) //jesli lista pusta, to nie odnaleziono zalacznika do usuniecia
		{ return -10; }
	if(file_name == NULL)
		{ return -2; }
	if( (strlen(file_name)) <= 0 )
		{ return -3; }


	iter=*list;

	//jesli podano sciezke bezwgledna/wzgledna do pliku, to wyodrebnienie nazwy samego pliku
	bmd_strsep(file_name, BMD_SMIME_PATH_SEPARATOR, &substrings, &subs_num);
	if(subs_num > 1)
	{ 
		cut_fileName=strdup( substrings[subs_num-1] );
	}
	else
	{ 
		cut_fileName=strdup(file_name);
	}
	bmd_strsep_destroy(&substrings, subs_num);

	while(iter != NULL)
	{
		//jesli obecnie przetwarzany element listy jest zalacznikiem do usuniecia
		if( (strcmp(iter->item->file_name, cut_fileName)) == 0 )
		{
			//jesli ma byc usuniety pierwszy element, to musi zmienic sie "korzen" calej listy
			if(prev_item == NULL)
			{
				*list=iter->next;
				bmd_destroy_attachment(&(iter->item));
				free(iter);
				free(cut_fileName);
				return 0;
			}
			//jesli usuwany element nie jest pierwszy w liscie
			else
			{
				prev_item->next=iter->next;
				bmd_destroy_attachment(&(iter->item));
				free(iter);
				free(cut_fileName);
				return 0;
			}
		}
		else
		{
			prev_item=iter;
			iter=iter->next;
		}
	}
	free(cut_fileName);

	//zalacznika do usuniecia nie znaleziono w liscie
	return -10;
}

/*sprawdzenie, czy plik jest w liscie zalacznikow; mozna podac sciezke - funkcja sama wyodrebni nazwe pliku*/
long is_attachment_in_attach_list(bmd_attach_list* list, char *file_name)
{
bmd_attach_list *iterator=list;
bmd_mime_attachment *next_item=NULL;

char **substrings=NULL;
long subs_num=0;
char *cut_fileName=NULL;
	
	if(file_name == NULL)
		{ return -1; }
	if(strlen(file_name) <= 0)
		{ return -2; }

	if(list == NULL)
	{
		return 0; //nie znalazl 
	}
	
	//jesli podano sciezke bezwgledna/wzgledna do pliku, to wyodrebnienie nazwy samego pliku
	bmd_strsep(file_name, BMD_SMIME_PATH_SEPARATOR, &substrings, &subs_num);
	if(subs_num > 1)
	{ 
		cut_fileName=strdup( substrings[subs_num-1] );
	}
	else
	{ 
		cut_fileName=strdup(file_name);
	}
	bmd_strsep_destroy(&substrings, subs_num);


	init_iter_attach_list(&iterator, list);
	get_next_item_attach_list(&iterator, &next_item);
	
	while(next_item != NULL)
	{
		if(strcmp(next_item->file_name, cut_fileName) == 0)
		{
			return 1; //znalazl
		}
		next_item=NULL;
		get_next_item_attach_list(&iterator, &next_item);
	}
	
	return 0; //nie znalazl
}

/*inicjalizacja iteratora po liscie zalacznikow*/
long init_iter_attach_list(bmd_attach_list **iterator, bmd_attach_list *head)
{
	if(iterator == NULL)
		{ return BMD_ERR_PARAM1; }
	if(*iterator != NULL)
		{ return BMD_ERR_PARAM1; }
	if(head == NULL)
		{ return BMD_ERR_PARAM2;}
	
	*iterator=head;
	return BMD_OK;
}


/*pobieranie kolejnego zalacznika z listy zalacznikow*/
long get_next_item_attach_list(bmd_attach_list **iterator, bmd_mime_attachment **next_item)
{
	if(iterator == NULL)
	{ 
		PRINT_DEBUG("LIBBMDMIMEERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1; 
	}
	if(*iterator == NULL)
	{
		return BMD_END_OF_LIST; //to juz koniec listy
	}
	if(next_item == NULL)
	{ 
		PRINT_DEBUG("LIBBMDMIMEERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2; 
	}
	if(*next_item != NULL)
	{ 
		PRINT_DEBUG("LIBBMDMIMEERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2; 
	}
	
	*next_item= (*iterator)->item;
	*iterator=(*iterator)->next;
	
	return BMD_OK;
}


long destroy_attach_list(bmd_attach_list **list)
{
	bmd_attach_list *iter=NULL;
	bmd_attach_list *temp=NULL;
	
	temp=*list;
	if(temp != NULL)
	{
		while(temp->next != NULL)
		{
			iter=temp->next;
			bmd_destroy_attachment( &(temp->item) );
			free(temp);
			temp=iter;
		}
		bmd_destroy_attachment( &(temp->item) );
		free(temp);
		*list=NULL;
	}
	
	return BMD_OK;
}

/**
file_name w przypadku sciezki bezwzglednej/wzglednej do pliku, funkcja wyodrebnia nazwe pliku i pod taka nazwa dodaje 
zalacznik do listy
*/
long bmd_create_attachment(char *file_name, long hidden_flag, bmd_mime_attachment **attach)
{
	bmd_mime_attachment *new_attachment=NULL;
	char **substrings=NULL;
	long subs_num=0;

	if(file_name == NULL)
	{ 
		PRINT_DEBUG("LIBBMDMIMEERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1; 
	}
	if(attach == NULL)
	{ 
		PRINT_DEBUG("LIBBMDMIMEERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2; 
	}
	if(*attach != NULL)
	{ 
		PRINT_DEBUG("LIBBMDMIMEERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2; 
	}

	new_attachment=(bmd_mime_attachment*)calloc(1, sizeof(bmd_mime_attachment));
	if(new_attachment == NULL)
 	{ 
		PRINT_DEBUG("LIBBMDMIMEERR Memory error. Error=%i\n",BMD_ERR_MEMORY);
		return BMD_ERR_MEMORY; 
	}
	
	if(bmd_load_binary_content(file_name, &(new_attachment->content) ) != BMD_OK )
	{
		free(new_attachment);
		PRINT_DEBUG("LIBBMDMIMEERR Disk space error. Error=%i\n",BMD_ERR_MEMORY);
		return BMD_ERR_DISK;
	}
	// TUTAJ trzeba dobrac sie do nazwy pliku w ścieżce bezwzględnej
	bmd_strsep(file_name, BMD_SMIME_PATH_SEPARATOR, &substrings, &subs_num);
	if(subs_num > 1)
	{ 
		new_attachment->file_name=strdup( substrings[subs_num-1] ); 
	}
	else
	{ 
		new_attachment->file_name=strdup(file_name); 
	}
	bmd_strsep_destroy(&substrings, subs_num);

	new_attachment->hidden=hidden_flag;
	
	*attach=new_attachment;
	return BMD_OK;
}


long bmd_create_attachment_from_buf(char *file_buffer, long file_len, char *file_name, long hidden_flag, bmd_mime_attachment **attach)
{
	bmd_mime_attachment *new_attachment=NULL;
	long status = 0;
	char **substrings=NULL;
	long subs_num=0;

	if(file_buffer == NULL)
	{ 
		PRINT_DEBUG("LIBBMDMIMEERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1; 
	}
	//plik zalacznika moze byc pusty
	if(file_len < 0)
	{ 
		PRINT_DEBUG("LIBBMDMIMEERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2; 
	}
	if(file_name == NULL)
	{ 
		PRINT_DEBUG("LIBBMDMIMEERR Invalid third parameter value. Error=%i\n",BMD_ERR_PARAM3);
		return BMD_ERR_PARAM3; 
	}
	if(attach == NULL)
	{ 
		PRINT_DEBUG("LIBBMDMIMEERR Invalid third parameter value. Error=%i\n",BMD_ERR_PARAM3);
		return BMD_ERR_PARAM4; 
	}
	if(*attach != NULL)
	{ 
		PRINT_DEBUG("LIBBMDMIMEERR Invalid third parameter value. Error=%i\n",BMD_ERR_PARAM3);
		return BMD_ERR_PARAM4; 
	}

	new_attachment=(bmd_mime_attachment*)calloc(1, sizeof(bmd_mime_attachment));
	if(new_attachment == NULL)
	{ 
		PRINT_DEBUG("LIBBMDMIMEERR Memory error. Error=%i\n",BMD_ERR_MEMORY);
		return BMD_ERR_MEMORY; 
	}

	status = set_gen_buf2((char*)file_buffer, file_len, &(new_attachment->content));
	if (status != BMD_OK)
	{
		free(new_attachment);
		PRINT_DEBUG("LIBBMDMIMEERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
	}

	// TUTAJ trzeba dobrac sie do nazwy pliku, jesli sciezka bezwzgledna/wzgledna
	bmd_strsep(file_name, BMD_SMIME_PATH_SEPARATOR, &substrings, &subs_num);
	if(subs_num > 1)
	{ 
		new_attachment->file_name=strdup( substrings[subs_num-1] ); 
	}
	else
	{ 
		new_attachment->file_name=strdup(file_name); 
	}
	bmd_strsep_destroy(&substrings, subs_num);
	
	new_attachment->hidden=hidden_flag;

	*attach=new_attachment;
	return BMD_OK;
}


long bmd_destroy_attachment(bmd_mime_attachment **attach)
{
	if(attach != NULL)
	{
		if(*attach != NULL)
		{
			free((*attach)->file_name);
			free_gen_buf( &((*attach)->content) );
			free(*attach);
			*attach=NULL;
		}
	}

	return BMD_OK;
}


/*
//funkcja odcina ostatni znak tekstu
long cut_last_sign( char *text, char **cut_text )
{
	long len=0;
	long iter=0;
	char *new_text=NULL;

	if(text == NULL)
		{ return BMD_ERR_PARAM1; }
	if(cut_text == NULL)
		{ return BMD_ERR_PARAM2; }
	if(*cut_text != NULL)
		{ return BMD_ERR_PARAM2; }

	len=strlen(text);
	new_text=(char*)calloc(len, sizeof(char*));
	if(new_text == NULL)
		{ return BMD_ERR_MEMORY; }
	
	for(iter=0; iter<len-1; iter++)
	{
		new_text[iter]=text[iter];
	}
	new_text[iter]='\0';

	*cut_text=new_text;
	return BMD_OK;
}
*/


/*funkcja zmienia w tekscie '\n' na sekwencję '\r\n'
potrzebna do przetwarzania kodu base64 - wyniku funkcji spc_base64_encode()
same koncowki \n w base64 powoduja zgloszenie niepoprawnego podpisu smime- zmiana zawartosci maila*/
long bmd_change_linebreak( char *text, char **cut_text )
{
	long len=0;
	long iter=0, i=0;
	char *new_text=NULL;
	long count_break=0;

	if(text == NULL)
	{ 
		PRINT_DEBUG("LIBBMDMIMEERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1; 
	}
	if(cut_text == NULL)
	{ 
		PRINT_DEBUG("LIBBMDMIMEERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2; 
	}
	if(*cut_text != NULL)
	{ 
		PRINT_DEBUG("LIBBMDMIMEERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2; 
	}

	len=(long)strlen(text);
	
	for(i=0; i < len; i++)
	{
		if(text[i] == '\n')
		{	
			count_break++;
		}
	}

	new_text=(char*)calloc(len+count_break+1, sizeof(char*));
	if(new_text == NULL)
	{ 
		PRINT_DEBUG("LIBBMDMIMEERR Memory error. Error=%i\n",BMD_ERR_MEMORY);
		return BMD_ERR_MEMORY; 
	}
	

	count_break=0; //tutaj jako informacja ile \r zostalo dozuconych
	for(iter=0; iter<len; iter++)
	{
		if(text[iter] == '\n')
		{
			new_text[iter + count_break]= '\r';
			count_break++;
			new_text[iter + count_break] = '\n';
		}
		else
		{
			new_text[iter+count_break]=text[iter];
		}
	}
	new_text[iter+count_break]='\0';

	*cut_text=new_text;
	return BMD_OK;
}

/*
funkcja character_utf8_to_quotedprintable() tworzy dla polskiego znaku (ogonka) w utf8 jego zapis quoted-printable

funkcja przyjmuje na wejscie dwa kolejne bajty lancucha znakowego
jesli pierwszy przekazywany bajt wskazuje na polski znak (ogonek) w utf8,
to bierze sie jeszcze pod uwage drugi przekazywany bajt i tworzy lancuch znakowy z zapisem quoted-printable

@arg first_byte_in_seq to pojedynczy bajt z lancucha znakowego
@arg second_byte_in_seq to bajt nastepujacy bezposrednio po first_char_in_seq
@arg quoted_printable to adres wskaznika na tworzony lanuch znakowy z zapisem quoed-printable podawanej sekwencji bajtow

@retval 0 w przypadku, gdy utworzono zapis quoted_printable
@retval -1 jesli za quoted_printable przekazano NULL
@retval -2 jesli wartosc wyluskana z quoted_printable nie jest wyNULLowana
@retval -3 jesli zawiodla operacja tworzenia lancucha znakowego
@retval -4 jesli funkcja nie rozpoznaje przekazanej sekwencji bajtow

!!!! w tej chwili funkcja nie jest wykorzystywana !!!!
*/
/*
long character_utf8_to_quotedprintable(char first_byte_in_seq, char second_byte_in_seq, char** quoted_printable)
{
	if(quoted_printable == NULL)
	{
		return -1;
	}
	if(*quoted_printable != NULL)
	{
		return -2;
	}

	switch(first_byte_in_seq)
	{
	
		case 0xC4:
		{
			switch(second_byte_in_seq)
			{
				case 0x85: //ą
				{
					//w przypadku bledu return -3, jesli sukces return 0
					bmd_strmerge("=C4=85", "",quoted_printable) ? -3 : 0;
				}
				case 0x84: //Ą
				{
					bmd_strmerge("=C4=84", "",quoted_printable) ? -3 : 0;
				}
				case 0x87: //ć
				{
					bmd_strmerge("=C4=87", "",quoted_printable) ? -3 : 0;
				}
				case 0x86: //Ć
				{
					bmd_strmerge("=C4=86", "",quoted_printable) ? -3 : 0;
				}
				case 0x99: //ę
				{
					bmd_strmerge("=C4=99", "",quoted_printable) ? -3 : 0;
				}
				case 0x98: //Ę
				{
					bmd_strmerge("=C4=98", "",quoted_printable) ? -3 : 0;
				}
				default:
				{
					return -4;
				}
			}
			break;
			
		}
		case 0xC5:
		{
			switch(second_byte_in_seq)
			{
				case 0x82: //ł
				{
					bmd_strmerge("=C5=82", "",quoted_printable) ? -3 : 0;
				}
				case 0x81: //Ł
				{
					bmd_strmerge("=C5=81", "",quoted_printable) ? -3 : 0;
				}
				case 0x9B: //ś
				{
					bmd_strmerge("=C5=9B", "",quoted_printable) ? -3 : 0;
				}
				case 0x9A: //Ś
				{
					bmd_strmerge("=C5=9A", "",quoted_printable) ? -3 : 0;
				}
				case 0xBC: //ż
				{
					bmd_strmerge("=C5=BC", "",quoted_printable) ? -3 : 0;
				}
				case 0xBB: //Ż
				{
					bmd_strmerge("=C5=BB", "",quoted_printable) ? -3 : 0;
				}
				case 0xBA: //ź
				{
					bmd_strmerge("=C5=BA", "",quoted_printable) ? -3 : 0;
				}
				case 0xB9: //Ź
				{
					bmd_strmerge("=C5=B9", "",quoted_printable) ? -3 : 0;
				}
				case 0x84: //ń
				{
					bmd_strmerge("=C5=84", "",quoted_printable) ? -3 : 0;
				}
				case 0x83: //Ń
				{
					bmd_strmerge("=C5=83", "",quoted_printable) ? -3 : 0;
				}
				default:
				{
					return -4;
				}
			}
			break;
		}
		case 0xC3:
		{
			switch(second_byte_in_seq)
			{
				case 0xB3: //ó
				{
					bmd_strmerge("=C3=B3", "",quoted_printable) ? -3 : 0;
				}
				case 0x93: //Ó
				{
					bmd_strmerge("=C3=93", "",quoted_printable) ? -3 : 0;
				}
				default:
				{
					return -4;
				}
			}
			break;
		}
		default:
		{
			return -4;
		}
		
	}
}
*/

/*
funkcja utf8string_to_quotedprintable transformuje lanuch znakowy w utf-8 na postac quoted-printable

@arg utf8string to lanuch znakowy w utf8
@arg to adres wskaznika na tworzony lanuch znakowy z zapisem quoted-printable

@retval 0 w przypadku sukcesu operacji
@retval -1 jesli za utf8string podano NULL
@retval -2 jesli za quotedstring podano NULL
@retval -3 jesli wartosc wyluskana z quotedstring nie jest wyNULLowana
@retval -4 jesli utf8string to lancuch pusty
@retval -5,-6,-7,-8,-9,-10 w przypadku bledow z towrzeniem lancuhow/alokacji pamieci

!!!! w tej chwili funkcja nie jest wykorzystywana !!!!
*/
/*
long utf8string_to_quotedprintable(char *utf8string, char **quotedstring)
{
	char *new_str=NULL;
	char *temp_str=NULL;
	char *quoted=NULL;
	char character[2];
	long len=0;
	long iter=0;
	

	if(utf8string == NULL)
		{ return -1; }
	if(quotedstring == NULL)
		{ return -2; }
	if(*quotedstring != NULL)
		{ return -3; }
	
	character[0]='\0'; character[1]='\0';

	len=strlen(utf8string);
	
	while(iter < len)
	{
		//mozna jeszcze dwa znaki wziac, jesli len-iter > 1
		if(len-iter>1)
		{
			//wykorzystanie dwoch bajtow
			if( (character_utf8_to_quotedprintable(utf8string[iter], utf8string[iter+1], &quoted)) == 0)
			{
				//jesli pierwsze bajty calego lanucha
				if(new_str==NULL)
				{
					new_str=strdup(quoted);
					free(quoted); quoted=NULL;
					if(new_str == NULL)
						{ return -5; }
					iter+=2;
				}
				//niepierwsze bajty lanucha
				else
				{
					bmd_strmerge(new_str, quoted, &temp_str);
					free(quoted); quoted=NULL;
					free(new_str);
					if(temp_str == NULL)
						{ return -6; }
					new_str=temp_str;
					temp_str=NULL;
					iter+=2;
				}
			}
			else //wykorzystany jeden bajt (bo nie ogonek - zwykly znak ascii)
			{
				//jesli pierwszy znak
				if(new_str==NULL)
				{
					character[0]=utf8string[iter];
					new_str=strdup(character);
					if(new_str == NULL)
						{ return -7; }
					iter++;
				}
				else
				{
					character[0]=utf8string[iter];
					bmd_strmerge(new_str, character, &temp_str);
					free(new_str);
					if(temp_str == NULL)
						{ return -8; }
					new_str=temp_str;
					temp_str=NULL;
					iter++;
				}
			}
		}
		//zostal jeden znak badz zaden
		else
		{	
			if( iter < len) //zostal ostatni
			{
				character[0]=utf8string[iter];
				//jesli ostatni jest rownaczesnie pierwszym (jendnoznakowy string)
				if(new_str==NULL)
				{
					new_str=strdup(character);
					if(new_str == NULL)
						{ return -9; }
				}
				else
				{
					bmd_strmerge(new_str, character, &temp_str);
					free(new_str);
					if(temp_str == NULL)
						{ return -10; }
					new_str=temp_str;
					temp_str=NULL;
				}
				
				*quotedstring=new_str;
				return 0;
			}
			else
			{
				*quotedstring=new_str;
				return 0;
			}
		}
		
	}
	
	return -4;
}
*/


/**
funkcja mail_attachment_name() na podstawie lancucha znakowego (nazwy zalaczanego pliku), tworzy lancuch, ktory jest 
odopwiednio interpretowany przez klienckie programy pocztowe i nie powoduje zgloszenia niezgodnosci podpisu z powodu polskich znakow
Funckja operuje na lanuchu znakowym w utf8.

@arg file_name to lancuch znakowy, ktory jest transformowany
@arg filename_in_message to adres wskaznika na tworzony lancuch znakowy, odporny na bledy interpretacji w mailu

@retval 0 jesli operacja zakonczyla sie sukcesem
@retval -1  jesli za file_name przekazano NULL
@retval -2 jesli za filename_in_message przekazano NULL
@retval -3 jesli wartosc wyluskana z filename_in_message nie jest wyNULLowana
@retval -4 jesli wystapil problem z kodowaniem base64
@retval -5,-6 jesli wystapil problem z laczeniem lancuchow znakowych

*/
long mail_attachment_name(char *file_name, char** filename_in_message)
{
	char *encoded64=NULL;
	char *new_str=NULL;
	char *temp_str=NULL;

	if(file_name == NULL)
	{
		return -1;
	}
	if(filename_in_message == NULL)
	{
		return -2;
	}
	if(*filename_in_message!=NULL)
	{
		return -3;
	}

	encoded64=(char *)spc_base64_encode( (unsigned char *)file_name, strlen(file_name), 0);
	if(encoded64 == NULL)
	{
		return -4;
	}
	if( (bmd_strmerge("=?utf-8?B?",(char*)encoded64, &temp_str)) != 0 )
	{
		free(encoded64);
		return -5;
	}
	free(encoded64);
	if( (bmd_strmerge(temp_str, "?=", &new_str)) != 0 )
	{
		free(temp_str);
		return -6;
	}
	free(temp_str);

	*filename_in_message=new_str;
	return 0;
}


/**
funkcja check_ma_ogonki() sprawdza, czy lancuch znakowy zawiera polskie znaki (ogonki)

@arg str to lancuch znakowy przeznaczony do sprawdzenia

@retval 1, jesli lancuch znakowy zawiera ogonki
@retval 0, jesli lancuch znakowy nie zawiera ogonkow
@retval -1, jesli do funkcji przekazano NULL

*/
long check_ma_ogonki(char* str)
{
	long iter=0;
	long len= (long)strlen(str);

	if(str== NULL)
	{
		return -1;
	}

	for(iter=0; iter<len; iter++)
	{
		if( (unsigned char)str[iter] == 0xC3 || (unsigned char)str[iter]== 0xC4 || (unsigned char)str[iter]==0xC5)
		{
			return 1;
		}
	}
	return 0;
}
