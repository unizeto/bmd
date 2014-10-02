#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmderr/libbmderr.h>

/*#include <PR_OCTET_STRING.h>
#include <PR_ANY.h>*/

/************************************************************ PROBLEM OIDO W KONTROLCE ********************************************/
/*
 * PCRE Funkcja alokuje pamiec na dopasowane substringi !!!
 */

/**
 * Funkcja wyszukuje podnapisy w (GenBuf_t) zgodne z łańcuchem formatującym (pattern) i 
   umieszcza je w strukturze SubstringsGenBuf_t.
 
 * @param[in] parsing_buff - adres struktury GenBuf_t.
 * @param[in] pattern - łańcuch formatujący.
 * @param[in] flag - czy przetwarzać wiele lini tekstu.
 * - MULTILINE
 * - SINGLELINE
 * @param[in] find_all - czy wyszukać wszystkie dopasowania.
 * - YES
 * - NO
 * @param[out] substrings_genbuf - adres struktury SubstringsGenBuf_t.
 * @return status zakończenia operacji.
 * @retval 0 - zakończenie pomyślne.
 * @retval PCRE_COMPILATION_ERR - błąd kompilacji wyrażenia regularnego.
 * @retval NO_MEMORY - błąd alokacji pamięci.
 * @retval PCRE_ERROR_NOMATCH - brak dopasowania.
 * @retval 1 - brak wielokrotnego dopasowania.

 * @note funkcja nie sprawdza poprawności przekazywanych parametrów.
 */

long find_substrings(const GenBuf_t *parsing_buff, const char *pattern, SubstringsGenBuf_t *substrings_genbuf, pcre_multiline_t flag, boolean_t find_all)
{
pcre *re = NULL;
const char *error = NULL;
int erroffset;
int ovector[OVECCOUNT]; /* wektor par ovector[i]ovector[i+1] == [offset poczatku substringa][dlugosc substringa] */
long result_counter, i;
long overall_result_counter = 0;
long tmp_result_counter = 0;
long substring_length = 0;
char *substring_start = NULL;
long options = 0;		/* standardowo brak opcji */
long start_offset = 0;   	/* przy wielokrotnym dopasowaniu nalezy przesuwac offset na koniec */
				/* poprzedniego dopasowania */
	/*************************************************************************
	* Kompilacja wyrazenia i obsluga bledow					 *
	*************************************************************************/
	if (flag == MULTILINE)
	{
		re = pcre_compile(
			pattern,              /* wyrazenie */
			PCRE_MULTILINE|PCRE_UTF8,       /* domyslne opcje */
			&error,               /* wskaznik na komunikat bledu */
			&erroffset,           /* offset komunikatu bledu */
			NULL);                /* uzycie domyslnej tablicy znakow */
	}
	else
	{
		re = pcre_compile(
			pattern,              /* wyrazenie */
			PCRE_UTF8,		      /* domyslne opcje */
			&error,               /* wskaznik na komunikat bledu */
			&erroffset,           /* offset komunikatu bledu */
			NULL);                /* uzycie domyslnej tablicy znakow */
	}
	
	if (re == NULL)
	{
		PRINT_DEBUG("LIBBMDERR PCRE compilation error\n");
		return PCRE_COMPILATION_ERR;
	}

	/*************************************************************************
	* Dokonanie jednokrotnego dopasowania					 *
	*************************************************************************/
	result_counter = pcre_exec(
		re,                   /* skompilowany wczesniej wzorzec */
		NULL,                 /* brak dodatkowych danych - nie analizuje wzorca */
		(const char *)parsing_buff->buf,    /* bufor, w ktorym szukamy dopasowania */
		parsing_buff->size,   /* rozmiar powyzszego bufora */
		0,                    /* rozpocznij od offsetu 0 bufora */
		0,                    /* domyslne opcje */
		ovector,              /* wektor wyjsciowy zawierajacy wskazniki poczatku i konca dopasowanych substringow */
		OVECCOUNT);           /* liczba elementow, ktore opisuje ovector */

	if (result_counter < 0) /* nie ma dopasowania */
	{
		/* wewnetrzna obsluga bledow */
		switch(result_counter)
		{
			case PCRE_ERROR_NOMATCH: /*PRINT_DEBUG("PCRE error no match\n");*/	/* No match */ break;
			default: 			/* Matching error */ break;
		}
		pcre_free(re);     /* Release memory used for the compiled pattern */
		re = NULL;
		return result_counter;
	}

	/********************************************************************************/
	/* bingo - wzorzec dopasowany							*/
	/* Znaleziono pierwsze dopsowanie do wzorca. jesli wektor wyjsciowy nie jest	*/
	/* wystarczajaco dlugi, ustaw jego rozmiar do maksimum. Nastepnie obsluz	*/ 
	/* znalezione substringi.							*/
	/********************************************************************************/
	
	/* Wektor wyjsciowy nie jest wystarczajaco pojemny */
	if (result_counter == 0)
	{
		result_counter = OVECCOUNT/3;
		/* ovector only has room for result_counter-1 captured substrings */
	}

	/* Obsługa znalezionych substringow  					*/
	/* Alokacja pamieci na znalezione substringi - tablice substringow 	*/
	substrings_genbuf->substrings_table = (char **) malloc (sizeof(char *) * result_counter);
	if (substrings_genbuf->substrings_table == NULL)
	{
		PRINT_DEBUG("LIBBMDERR Memory error\n");
		return NO_MEMORY;
	}
	substrings_genbuf->no_of_substrings = result_counter;
		
	for (i = 0; i < result_counter; i++)
	{
		substring_start = (char *)parsing_buff->buf + ovector[2*i];
		substring_length = ovector[2*i+1] - ovector[2*i];
		
		/* alokacja pamieci na substringi + '\0' */
		substrings_genbuf->substrings_table[i] = (char *) malloc (sizeof(char) * substring_length+1);
		if (substrings_genbuf->substrings_table[i] == NULL)
		{
			PRINT_DEBUG("LIBBMDERR Memory error \n");
			return NO_MEMORY;
		}
		memset(substrings_genbuf->substrings_table[i], 0, substring_length+1);
		memmove(substrings_genbuf->substrings_table[i], substring_start, substring_length);
		/*printf("Substring table [%i] = %s\n", i, substrings_genbuf->substrings_table[i]);*/
	}
	overall_result_counter = result_counter;
	
	if (find_all == NO)
	{
		pcre_free(re);   /* Zwolnij pamiec uzywana na kompilowany wzorzec */
		return 0;   /* zakoncz funkcje */
	}

	/* Petla do sprawdzenia drugiego i kolejnych dopasowan */
	for (;;)
	{
		options = 0;                 /* Normally no options */
		start_offset = ovector[1];   /* Start at end of previous match */
		
		/* Jesli poprzednie dopasowanie bylo pustym stringiem, to konczymy jesli jestesmy na 
		koncu bufora. W przeciwnym wypadku, dokonaj kolejnego dopasowania rozpoczynajac
		od biezacego polozenia wskaznika w buforze by zobaczyc czy znaleziono nie-puste
		dopasowanie */
		if (ovector[0] == ovector[1])
		{
			if (ovector[0] == substring_length) break; /* koniec bufora */
			options = PCRE_NOTEMPTY | PCRE_ANCHORED;
		}

		/* szukaj kolejnego dopasowania */
		result_counter = pcre_exec(
			re,                   /* the compiled pattern */
			NULL,                 /* no extra data - we didn't study the pattern */
			(const char *)parsing_buff->buf,    /* the subject string */
			parsing_buff->size,   /* the length of the subject */
			start_offset,         /* starting offset in the subject */
			options,              /* options */
			ovector,              /* output vector for substring information */
			OVECCOUNT);           /* number of elements in the output vector */

		/* W tym wypadku kod NOMATCH nie jest bledem. Jesli wartosc w opcjach jest = 0 	*/
		/* oznacza to ze wlasnie znalezlismy wszystkie mozliwe dopasowania, tak wiec	*/
		/* petla sie konczy								*/
		/* W przeciwnym przypadku oznacza to ze nie udalo sie znalezc niepustego 	*/
		/* stringa w punkcie, gdzie byl poprzedni dopasowany pusty string.		*/
		/* W tym przypadku przesuwamy sie o jeden bialy znak do przodu i kontynuujemy	*/
		/* szukanie dopasowania.							*/
		
		if (result_counter == PCRE_ERROR_NOMATCH)
		{
			if (options == 0) break;
			ovector[1] = start_offset + 1;
			continue;    /* kolejna iteracja petli */
		}

		/* Obsluga bledow dopasowania */
		if (result_counter < 0)
		{
			/* Matching error */
			pcre_free(re);    /* zwolnij pamiec przyznana na skompilowanie wzorca */
			re = NULL;
			return 1;
		}

		/* Kolejne udane dopasowanie */
		/* Dopasowanie powiodlo sie ale wektor wyjsciowy nie byl dostatecznie pojemny */
		if (result_counter == 0)
		{
			result_counter = OVECCOUNT/3;
			/* ovector only has room for result_counter-1 captured substrings */
		}

		/* na znalezione kolejne dopasowania nalezy realokowac tablice substringow */
		tmp_result_counter = overall_result_counter;
		overall_result_counter += result_counter;
		
		substrings_genbuf->substrings_table = realloc (substrings_genbuf->substrings_table, sizeof(char *) * overall_result_counter);
		if (substrings_genbuf->substrings_table == NULL)
		{
			PRINT_DEBUG("LIBBMDERR Memory error \n");
			return NO_MEMORY;
		}
		substrings_genbuf->no_of_substrings = overall_result_counter;
		
		
		/* Obsluga znalezionych substringow */
		for (i = 0; i < result_counter; i++)
		{
			substring_start = (char *)parsing_buff->buf + ovector[2*i];
			substring_length = ovector[2*i+1] - ovector[2*i];
			/* podpinam do odpowiednich indeksow tablicy substringow i gotowe */	
			substrings_genbuf->substrings_table[tmp_result_counter+i] = (char *) malloc (sizeof(char) * substring_length+1);
			if (substrings_genbuf->substrings_table[tmp_result_counter+i] == NULL)
			{
				PRINT_DEBUG("LIBBMDERR Memory error \n");
				return NO_MEMORY;
			}
			memset(substrings_genbuf->substrings_table[tmp_result_counter+i], 0, substring_length+1);
			memmove(substrings_genbuf->substrings_table[tmp_result_counter+i], substring_start, substring_length);
		}
	}      /* Koniec petli do znalezienia drugiego i kolejnych dopasowan */
	
	pcre_free(re);       /* zwolnij pamiec zaalokowana na wzorzec */
	re = NULL;
	return 0;
}
