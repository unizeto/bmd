#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/common/bmd-common.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/common/bmd-const.h>
#include <bmd/common/global_types.h>
#ifdef WIN32
#pragma warning(disable:4702)
#endif
/**
 * Funkcja kopiuje zawartość listy predefiniowanych wartości dla kontrolki
 * @param[in] source - adres struktury BMD_attr_t
 * @param[out] dest - adres utworzonej listy predefiniowanych wartości dla kontrolki
 * @return status zakonczenia operacji
 * @retval BMD_OK - zakonczenie pomyślne
 * @note funkcja zawsze zwraca BMD_OK mimo iz po drodze moga wystapić bledy
 * @note funkcja nie dziala prawidlowo dokladne informacje skomentowane w kodzie
 * @notr brak sprawdzenie poprawności przekazywanych argumentow
*/

long duplicate_values(BMD_attr_t *source,BMD_attr_value ***dest)  // funkcja nie wykorzystywana
{
	long i;
	long count=0;

	if(source->valuesList==NULL)
		*dest=NULL;

	i=0;
	while(source->valuesList[i]) /*jezeli source->valueList == NULL to bedzie tutaj naruszenie ochrony pamieci*/
	{
		count++;
		i++;
	}


	(*dest)=(BMD_attr_value **)malloc((count+2)*sizeof(BMD_attr_value *));// brak sprawdzenia malloca
	memset(*dest,0,sizeof(BMD_attr_value *)*(count+1));

	for(i=0;i<count;i++)
	{
		set_gen_buf2(source->valuesList[i]->buf,source->valuesList[i]->size,&( (*dest)[i] ));
		// brak sprawdzenia kodu powrotu
	}

	return BMD_OK;
}


/**
 * Funkcja kopiuje zawartość struktury OID_attr_t wskazywanej przez (source)
 * do obszaru struktury wskazywanej przez (dest).
 * @param[in] source - adres struktury OID_attr_t
 * @param[out] dest - adres struktury OID_attr_t
 * @return status zakonczenia operacji
 * @retval -1 - blad parametru wejściowego lub wyjściowego.
 * @retval -2 - blad parametru wejściowego.
 * @retval -4 - brak pamięci.
 * @retval 0 - zakonczenie pomyślne
 */

// long duplicate_oid(	OID_attr_t *source,
// 				OID_attr_t *dest)
// {
//
// 	if (source == NULL)		{	return BMD_ERR_PARAM1;	}
// 	if (dest == NULL)			{	return BMD_ERR_PARAM1;	}
// 	if (source->oid_table==NULL)	{	return BMD_ERR_PARAM2;	}
//
// 	/** @test malloc mial inna ilosc pamieci niz memcpy() kilka linii nizej!
// 		usunalem sizeof(long) -- LK
// 	*/
// 	dest->oid_table=(char *)malloc(sizeof(char)*(strlen(source->oid_table)+1));
// 	if (dest->oid_table == NULL)	{	return NO_MEMORY;	}
// 	memset(dest->oid_table, 0, sizeof(char)*(strlen(source->oid_table)+1));
// 	memcpy(dest->oid_table,source->oid_table,sizeof(char)*strlen(source->oid_table));
//
// 	return BMD_OK;
//
// }

/**
 * Clear OID structure and prepare to use with dupilcate_oid etc.
 * If allocated == 1 && oid->oid_table_size > 0, then this function free()s memory
 * allocated for that OID. Otherwise it only puts NULL into oid->oid_table.
 * @param[in,out] oid - oid to clear
 * @param[in] allocated - if OID is already allocated and should be freed, place 1 here
 * @note brak sprawdzenia poprawności przekazywanych argumentow
*/
// void clear_oid(OID_attr_t * oid, long allocated)
// {
// 	if (allocated == 1)
// 	{
// 		if (oid->oid_table != NULL)
// 		{
// 			free(oid->oid_table);
// 		}
// 	}
//
// 	oid->oid_table = NULL;
// }


/**
 * Inicjowanie struktury BMD_attr
 * @param[in,out] buf - wskaznik węzla struktury BMD_attr_t
 * @return status zakonczenia operacji
 * @retval BMD_OK - normalne zakonczenie
 * @note brak sprawdzenia poprawności przekazywanych argumentow
*/
long BMD_attr_init(BMD_attr_t **buf)
{
	memset(*buf,0,sizeof(BMD_attr_t)); // wypelnianie bez sprawdzenia poprawności argumentu
	return BMD_OK;
}


/**
 * Funkcja kopiuje zawartość struktury BMD_attr_t wskazywanej przez (input) do
 * struktury BMD_attr_t wskazywanej przez (output).
 * @param[in] input - adres struktury BMD_attr_t (zrodlo)
 * @param[out] output - adres struktury BMD_attr_t (cel)
 * @param[in] seqNumber - numer kolejny kontrolki
 * @param[in] dup_values - czy kopiować predefiniowane wartości z listy
 * @param[in] dup_priority - czy kopiować listę priorytetow dla wartości powyzszej listy.
 * @param[in] dup_depValuesList - czy kopiować listę wartości zaleznych
 * @return status zakonczenia operacji
 * @retval BMD_OK - pomyślne zakonczenie
 * @note funkcja zwraca zawsze BMD_OK mimo iz po drodze moze wystapic jakis
 *  blad, ktory obecnie jest ignorowany.
 * @note brak prototypu tej funkcji w pliku naglowkowym
*/
long BMD_attr_duplicate(	BMD_attr_t *input,
					long seqNumber,
					BMD_attr_t **output,
					long dup_values,
					long dup_priority,
					long dup_depValuesList)
{
long i		= 0;
long count		= 0;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (input==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (output==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}

	(*output)=(BMD_attr_t *)malloc(sizeof(BMD_attr_t));
	if ((*output)==NULL)		{	BMD_FOK(BMD_ERR_MEMORY);	}

	memset((*output),0,sizeof(BMD_attr_t));

	/**********************************************/
	/* alokacja pamiecia na poszczegolne elementy */
	/**********************************************/
//  	(*output)->label=(GenBuf_t *)malloc(sizeof(GenBuf_t));
//  	memset((*output)->label, 0, sizeof(GenBuf_t));
//
// 	(*output)->oid=(GenBuf_t *)malloc(sizeof(GenBuf_t));
// 	memset((*output)->oid, 0, sizeof(GenBuf_t));
//
// 	(*output)->regexp=(GenBuf_t *)malloc(sizeof(GenBuf_t));
// 	memset((*output)->regexp, 0, sizeof(GenBuf_t));
//

	BMD_FOK(set_gen_buf2(input->label->buf,input->label->size,&((*output)->label)));
	(*output)->wxType=input->wxType;
	(*output)->visibility=input->visibility;
	(*output)->targetForm=input->targetForm;
	(*output)->seqNumber=seqNumber;

	if(dup_values==1)
	{
		i=0;
		count=0;
		while(input->valuesList[count])// bledne sprawdzenie jesli input->valuesList == NULL
		{
			count++;
		}
		(*output)->valuesList=(BMD_attr_value **)malloc(sizeof(BMD_attr_value *)*(count+2));//brak sprawdzenia malloca
		memset((*output)->valuesList,0,sizeof(BMD_attr_value *)*(count+1));
		for(i=0;i<count;i++)
		{
			set_gen_buf2(input->valuesList[i]->buf,input->valuesList[i]->size,&((*output)->valuesList[i]));//brak sprawdzenia kodu powrotu
		}
	}

	if(dup_priority==1)
	{
		i=0;
		count=0;
		while(input->priorityList[count])// bledne sprawdzenie jesli input->priorityList == NULL
		{
			count++;
		}
		(*output)->priorityList=(long *)malloc(sizeof(long)*(count+2));// brak sprawdzenia malloca
		memset((*output)->priorityList,0,sizeof(long)*(count+1));

		for(i=0;i<count;i++)
		{
			(*output)->priorityList[i]=input->priorityList[i];
		}
	}

	(*output)->sizeX=input->sizeX;
	(*output)->sizeY=input->sizeY;
	BMD_FOK(set_gen_buf2(input->oid->buf,input->oid->size,&((*output)->oid)));
	(*output)->contentType=input->contentType;
	BMD_FOK(set_gen_buf2(input->regexp->buf,input->regexp->size,&((*output)->regexp)));
	(*output)->superiorControl=input->superiorControl;
	(*output)->inferiorControl=input->inferiorControl;
	(*output)->connectType=input->connectType;
	(*output)->valueSource=input->valueSource;

	if(input->tooltip)
	{
		BMD_FOK(set_gen_buf2(input->tooltip->buf,input->tooltip->size,&((*output)->tooltip)));
	}
	if(dup_depValuesList==1)
	{
		if(input->depValueList)
		{
			BMD_FOK(set_gen_buf2(input->depValueList->buf,input->depValueList->size,&((*output)->depValueList)));
		}
	}

	(*output)->inheritancePresent=input->inheritancePresent;
	BMD_FOK(set_gen_buf2(input->controlName->buf,input->controlName->size,&((*output)->controlName)));
	(*output)->rangeEnabled=input->rangeEnabled;
	(*output)->splitToWords=input->splitToWords;
	(*output)->mandatory=input->mandatory;
	(*output)->isUpdateEnabled=input->isUpdateEnabled;

    	return BMD_OK;
}

/**
 * Funkcja przygotowuje formularz do wysylania danych
 * @param[in] kontrolki - adres dostępnych kontrolek
 * @param[out] send_kontrolki - odfiltrowane kontrolki powiazane z formularzem do wysylania danych
 * @return status zakonczenia operacji
 * @retval 0 - zakonczenie pomyślne.
 * @note funkcja zawsze zwraca 0 mimo iz po drodze moze wystapić jakiś blad
 * @note brak sprawdzenia poprawności przekazywanych argumentow
*/

long create_specific_kontrolki(	BMD_attr_t **kontrolki,
				KontrolkiType_t KontrolkiType,
				BMD_attr_t ***spec_kontrolki)
{
long i		= 0;
long spec_count	= 0;
long spec_index	= 0;
long count		= 0;

	while(kontrolki[i])
	{
		switch (KontrolkiType)
		{
			case SEARCH_KONTROLKI	:
				if(is_search_control(kontrolki[i]->targetForm,kontrolki[i]->visibility))
				{
					spec_count++;
				}
				break;
			case SEND_KONTROLKI	:
				if(is_send_control(kontrolki[i]->targetForm,kontrolki[i]->visibility))// brak sprawdzenia kodu powrotu
				{
					spec_count++;
				}
				break;
			case UPDATE_KONTROLKI	:
				if ((kontrolki[i]->isUpdateEnabled==1) && (kontrolki[i]->visibility==1))
				{
					spec_count++;
				}
				break;
			case UNVISIBLE_KONTROLKI:
				if(is_unvisible_used_control(kontrolki[i]->visibility)>0)
				{
					spec_count++;
				}
				break;
			case GRID_KONTROLKI	:
				if(is_grid_control(kontrolki[i]->targetForm,kontrolki[i]->visibility))//brak sprawdzenia malloca
				{
					spec_count++;
				}
				break;
			case HISTORY_KONTROLKI	:
				if ((kontrolki[i]->isHistoryEnabled==1) && (kontrolki[i]->visibility==1))
				{
					spec_count++;
				}
				break;
		}
		i++;
		count++;
	}

	(*spec_kontrolki)=(BMD_attr_t **)malloc(sizeof(BMD_attr_t *)*(spec_count+1));
	if ((*spec_kontrolki)==NULL)		{	BMD_FOK(BMD_ERR_MEMORY);	}
	memset((*spec_kontrolki), 0, sizeof(BMD_attr_t *)*(spec_count+1));

	for(i=0;i<count;i++)
	{
		switch (KontrolkiType)
		{
			case SEARCH_KONTROLKI	:
				if(is_search_control(kontrolki[i]->targetForm,kontrolki[i]->visibility))
				{
					BMD_FOK(BMD_attr_duplicate(kontrolki[i], spec_index, &((*spec_kontrolki)[spec_index]),1,0,1));
		 			spec_index++;
				}
				break;
			case SEND_KONTROLKI	:
				if(is_send_control(kontrolki[i]->targetForm,kontrolki[i]->visibility))// brak sprawdzenia kodu powrotu
				{
					BMD_FOK(BMD_attr_duplicate(kontrolki[i], spec_index, &((*spec_kontrolki)[spec_index]),1,0,1));
		 			spec_index++;
				}
				break;
			case UPDATE_KONTROLKI	:
				if ((kontrolki[i]->isUpdateEnabled==1) && (kontrolki[i]->visibility==1))
				{
					BMD_FOK(BMD_attr_duplicate(kontrolki[i], spec_index, &((*spec_kontrolki)[spec_index]),1,0,1));
		 			spec_index++;
				}
				break;
			case UNVISIBLE_KONTROLKI:
				if(is_unvisible_used_control(kontrolki[i]->visibility)>0)
				{
					BMD_FOK(BMD_attr_duplicate(kontrolki[i], spec_index, &((*spec_kontrolki)[spec_index]),1,0,1));
		 			spec_index++;
				}
				break;
			case GRID_KONTROLKI	:
				if(is_grid_control(kontrolki[i]->targetForm,kontrolki[i]->visibility))//brak sprawdzenia malloca
				{
					BMD_FOK(BMD_attr_duplicate(kontrolki[i], spec_index, &((*spec_kontrolki)[spec_index]),1,0,1));
		 			spec_index++;
				}
				break;
			case HISTORY_KONTROLKI	:
				if ((kontrolki[i]->isHistoryEnabled==1) && (kontrolki[i]->visibility==1))
				{
					BMD_FOK(BMD_attr_duplicate(kontrolki[i], spec_index, &((*spec_kontrolki)[spec_index]),1,0,1));
		 			spec_index++;
				}
				break;
		}
	}

	return BMD_OK;
}

/**
 * Funkcja określa docelowy formularz kontrolki
 * @param[in] pos - docelowy formularz kontrolki
 * @return d - docelowy formularz kontrolki
*/
long get_pos(long pos)
{
	return pos;
}

/**
 * Funkcja podaje status widoczności kontrolki
 * @param[in] show - status widoczności kontrolki
 * @retval d - status widoczności kontrolki
*/
long get_visible(long show)
{
	return show;
}


/**
 * Funkcja sprawdza czy dana kontrolka moze być umieszczona na formularzu powiazanym z wysylaniem danych
 *
 * @param[in] place - rodzaj formularza
 * @param[in] show - czy kontrolka jest widoczna
 * @return status zakonczenia operacji
 * @retval 1 - pomyślne zakonczenie, mozna umieścić kontrolkę na odpowiednim formularzu
 * @retval 0 - kontrolka jest widoczna
 * @retval -1 - blad
*/

// testy zbędne, funkcja wykorzystywana w create_send_kontrolki
long is_send_control(long place,long show)
{
	if(show==CONTROL_VISIBLE)
	{
	    if( (place==TARGET_FORM_SEND) || (place==TARGET_FORM_ALL) || (place==TARGET_FORM_SEND_SEARCH) )
		return 1;
	    else
		return 0;
	}
	else
	    return 0;

	return -1;
}

/**
 * Funkcja sprawdza czy dana kontrolka moze być umieszczona na formularzu powiazanym z wyszukiwaniem danych.
 * @param[in] place - rodzaj formularza
 * @param[in] show - czy kontrolka jest widoczna
 * @return status zakonczenia operacji
 * @retval 1 - pomyślne zakonczenie, mozna umieścić kontrolkę na odpowiednim formularzu.
 * @retval 0 - kontrolka jest widoczna
 * @retval -1 - blad
*/

// testy zbędne, funkcja wykorzystywana w create_search_kontrolki
long is_search_control(long place,long show)
{
	if(show==CONTROL_VISIBLE)
	{
	    if( (place==TARGET_FORM_SEARCH) || (place==TARGET_FORM_ALL) || (place==TARGET_FORM_SEND_SEARCH) || (place==TARGET_FORM_SEARCH_GRID) )
		return 1;
	    else
		return 0;
	}
	else
	    return 0;

	return -1;
}

/**
 * Funkcja sprawdza czy dana kontrolka moze być umieszczona na formularzu powiazanym z przegladaniem danych.
 * @param[in] place - rodzaj formularza
 * @param[in] show - czy kontrolka jest widoczna
 * @return status zakonczenia operacji
 * @retval 1 - pomyślne zakonczenie, mozna umieścić kontrolkę na odpowiednim formularzu
 * @retval 0 - kontrolka jest widoczna
 * @retval -1 - blad
*/

// testy zbędne, funkcja wykorzystywana w create_grid_kontrolki
long is_grid_control(long place,long show)
{
	if(show==CONTROL_VISIBLE)
	{
	    if( (place==TARGET_FORM_GRID) || (place==TARGET_FORM_ALL) || (place==TARGET_FORM_SEARCH_GRID) )
		return 1;
	    else
		return 0;
	}
	else
	    return 0;

	return -1;
}

/**
 * Funkcja sprawdza czy kontrolka jest niewidoczna
 * @param[in] show - status widoczności kontrolki
 * @retval 1 - kontrolka niewidoczna
 * @retval 0 - kontrolka widoczna
 * @retval -1 - blad
*/

// testy zbędne funkcja wykorzystywana w create_unvisible_kontrolki
long is_unvisible_used_control(long show)
{
	if(show==CONTROL_UNVISIBLE)
	    return 1;
	else
	    if(show==CONTROL_VISIBLE)
		return 0;

	return -1;
}
