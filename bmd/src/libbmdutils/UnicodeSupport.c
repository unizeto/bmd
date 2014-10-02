#include<bmd/libbmdutils/libbmdutils.h>

#ifdef WIN32

/**
funkcja WindowsStringToUTF8String() konwertuje WideCharacter String (czyli UTF-16 w Windows) do lancucha
znakowego zakodowanego w UTF-8.

Parametry wywolania:
@param [in] windowsString to wskaznik na konwertowany lancuch w UTF-16
@param [out] utf8String to adres wskaznika, pod ktory zostanie przypisany wynik konwersji (lancuch w UTF-8)
@param [out, optional] utf8StringLength to adres zmiennej long,
	w ktorej zostanie zapisana dlugosc utf8String (bez konczacego NULLa)
	(parametr jest opcjonalny - mozna podac NULL)

Wartosc zwracana:
@retval 0 w przypadku sukcesu operacji
@retval odpowiednia wartosc ujemna w przypadku bledu
*/
long WindowsStringToUTF8String(wchar_t *windowsString, char **utf8String, long *utf8StringLength)
{
	long retVal=0;
	long windowsStringLength=0;

	char *safeOutputString=NULL;
	long safeOutputStringLength=0;

	char *outputString=NULL;
	long outputStringLength=0;


	if(windowsString == NULL)
	{
		return -1;
	}
	if(utf8String == NULL)
	{
		return -2;
	}
	if(*utf8String != NULL)
	{
		return -3;
	}

	windowsStringLength=(long)wcslen(windowsString);
	if(windowsStringLength <= 0)
	{
		outputString=(char*)calloc(1, sizeof(char));
		if(outputString == NULL)
		{
			return -4;
		}
		else
		{
			*utf8String=outputString;
			return 0;
		}
	}

	//alokacja bufora, w ktorym na pewno zmiesci sie wynik (na 100 procent bufor bedzie za duzy)
	safeOutputStringLength=(windowsStringLength+1)*6;
	safeOutputString=(char*)calloc(safeOutputStringLength, sizeof(char));
	if(safeOutputString == NULL)
	{
		return -5;
	}

	retVal=WideCharToMultiByte(CP_UTF8, 0, windowsString, windowsStringLength+1, safeOutputString, safeOutputStringLength, NULL, NULL); 
	if(retVal == 0) //blad dla 0
	{
		free(safeOutputString);
		safeOutputString=NULL;
		return -6;
	}

	//zaalokowany bufor jest za duzy, wiec alokuje najkrotszy mozliwy bufor mieszczacy wynik
	outputStringLength=(long)strlen(safeOutputString);
	outputString=(char*)calloc(outputStringLength+1, sizeof(char));
	if(outputString == NULL)
	{
		free(safeOutputString);
		safeOutputString=NULL;
		return -7;
	}

	memcpy(outputString, safeOutputString, outputStringLength);
	free(safeOutputString);
	safeOutputString=NULL;

	*utf8String=outputString;
	outputString=NULL;

	if(utf8StringLength != NULL)
	{
		*utf8StringLength=outputStringLength;
	}

	return 0;
}

/**
funkcja UTF8StringToWindowsString() konwertuje lancuch znakowy zakodowany w UTF-8
do WideCharacter String (czyli UTF-16 w Windows)

Parametry wywolania:
@param [in] utf8String to adres wskaznika na konwertowany lancuch w UTF-8
@param [out] windowsString to wskaznik, pod ktory zostanie przypisany wynik konwersji (lancuch w UTF-16)
@param [out, optional] windowsStringLength to adres zmiennej long,
	w ktorej zostanie zapisana dlugosc windowsString (bez konczacego NULLa)
	(parametr jest opcjonalny - mozna podac NULL)

Wartosc zwracana:
@retval 0 w przypadku sukcesu operacji
@retval odpowiednia wartosc ujemna w przypadku bledu
*/
long UTF8StringToWindowsString(char *utf8String, wchar_t **windowsString, long *windowsStringLength)
{
	long retVal=0;
	wchar_t *safeWindowsString=NULL;
	long utf8StringLength=0;

	wchar_t *outputString=NULL;
	long outputStringLength=0;

	if(utf8String == NULL)
	{
		return -1;
	}
	if(windowsString == NULL)
	{
		return -2;
	}
	if(*windowsString != NULL)
	{
		return -3;
	}

	utf8StringLength=(long)strlen(utf8String);
	safeWindowsString=(wchar_t*)calloc(utf8StringLength+1, sizeof(wchar_t)); //w utf8 znak nawet na 6 bajtach wiec na pewno wystarczy
	if(safeWindowsString == NULL)
	{
		return -4;
	}

	retVal=MultiByteToWideChar(CP_UTF8, 0, utf8String, (int)utf8StringLength, safeWindowsString, (int)(utf8StringLength+1));
	if(retVal == 0) //jesli 0 to blad
	{
		free(safeWindowsString);
		safeWindowsString=NULL;
		return -5;
	}

	//zaalokowany bufor jest za duzy, wiec alokuje najkrotszy mozliwy bufor mieszczacy wynik
	outputStringLength=(long)wcslen(safeWindowsString);
	outputString=(wchar_t*)calloc(outputStringLength+1, sizeof(wchar_t));
	if(outputString == NULL)
	{
		free(safeWindowsString);
		safeWindowsString=NULL;
		return -6;
	}

	wcscpy(outputString, safeWindowsString);
	free(safeWindowsString);
	safeWindowsString=NULL;
	
	*windowsString=outputString;
	outputString=NULL;

	if(windowsStringLength != NULL)
	{
		*windowsStringLength=outputStringLength;
	}

	return 0;
}

/**
funkcja UTF8StringToCurrentCodePage() konwertuje lancuch znakowy zakodowany w UTF-8
do lancucha znakowa w aktualnej stronie kodowej systemu (zazwyczaj cp1250)

Parametry wywolania:
@param [in] utf8String to adres wskaznika na konwertowany lancuch w UTF-8
@param [out] outputString to wskaznik, pod ktory zostanie przypisany zaalokowany dynamicznie wynik konwersji
@param [out, optional] outputStringLength to adres zmiennej long,
	w ktorej zostanie zapisana dlugosc outputString (wlacznie z konczacym NULL)
	(parametr jest opcjonalny - mozna podac NULL)

Wartosc zwracana:
@retval 0 w przypadku sukcesu operacji
@retval odpowiednia wartosc ujemna w przypadku bledu
*/
long UTF8StringToCurrentCodePage(const char *utf8String, char **outputString, long *outputStringLength)
{
	long retVal						= 0;
	wchar_t *wideString				= NULL;
	long wideStringLength			= 0;
	char* safeOutputString			= NULL;
	long safeOutputStringLength		= 0;

	if(utf8String == NULL)
		{ return -1; }
	if(outputString == NULL)
		{ return -2; }
	if(*outputString != NULL)
		{ return -3; }
	

	retVal = UTF8StringToWindowsString((char*)utf8String, &wideString, &wideStringLength);
	if(retVal < 0)
		{ return -11; }

	// obliczenie rozmiaru potrzebnego bufora
	retVal=WideCharToMultiByte(CP_ACP, 0, wideString, wideStringLength+1, NULL, 0, NULL, NULL); // CP_ACP (aktualna strona kodowa systemu - zapewne cp1250)
	if(retVal == 0) // blad dla 0
	{
		free(wideString);
		return -12;
	}

	safeOutputStringLength = retVal;
	safeOutputString = (char*)calloc(safeOutputStringLength + 1, sizeof(char));
	if(safeOutputString == NULL)
	{
		free(wideString);
		return -13;
	}

	retVal=WideCharToMultiByte(CP_ACP, 0, wideString, wideStringLength+1, safeOutputString, safeOutputStringLength+1, NULL, NULL); // CP_ACP (aktualna strona kodowa systemu - zapewne cp1250)
	free(wideString); wideString = NULL;
	if(retVal == 0) // blad dla 0
	{
		free(safeOutputString);
		return -14;
	}

	*outputString=safeOutputString;
	safeOutputString=NULL;

	if(outputStringLength != NULL)
	{
		*outputStringLength=safeOutputStringLength;
	}

	return 0;
}

/**
funkcja WindowsStringToCurrentCodePage() konwertuje WideCharacter String (czyli UTF-16 w Windows) do lancucha
znakowego zakodowanego w aktualnej stronie kodowej systemu (zazwyczaj cp1250)

Parametry wywolania:
@param [in] windowsString to wskaznik na konwertowany lancuch w UTF-16
@param [out] outputString to adres wskaznika, pod ktory zostanie przypisany zaalokowany dynamicznie wynik konwersji
@param [out, optional] outputStringLength to adres zmiennej long,
	w ktorej zostanie zapisana dlugosc outputString (wlacznie z konczacym NULL)
	(parametr jest opcjonalny - mozna podac NULL)

Wartosc zwracana:
@retval 0 w przypadku sukcesu operacji
@retval odpowiednia wartosc ujemna w przypadku bledu
*/
long WindowsStringToCurrentCodePage(const wchar_t *windowsString, char **outputString, long *outputStringLength)
{
	long retVal						= 0;
	char* safeOutputString			= NULL;
	long safeOutputStringLength		= 0;
	long windowsStringLength		= 0;

	if(windowsString == NULL)
		{ return -1; }
	if(outputString == NULL)
		{ return -2; }
	if(*outputString != NULL)
		{ return -3; }
	

	windowsStringLength=(long)wcslen(windowsString);

	// obliczenie rozmiaru potrzebnego bufora
	retVal=WideCharToMultiByte(CP_ACP, 0, windowsString, windowsStringLength+1, NULL, 0, NULL, NULL); // CP_ACP (aktualna strona kodowa systemu - zapewne cp1250)
	if(retVal == 0) // blad dla 0
		{ return -12; }

	safeOutputStringLength = retVal;
	safeOutputString = (char*)calloc(safeOutputStringLength + 1, sizeof(char));
	if(safeOutputString == NULL)
		{ return -13; }

	retVal=WideCharToMultiByte(CP_ACP, 0, windowsString, windowsStringLength+1, safeOutputString, safeOutputStringLength+1, NULL, NULL); // CP_ACP (aktualna strona kodowa systemu - zapewne cp1250)
	if(retVal == 0) // blad dla 0
	{
		free(safeOutputString);
		return -14;
	}

	*outputString=safeOutputString;
	safeOutputString=NULL;

	if(outputStringLength != NULL)
	{
		*outputStringLength=safeOutputStringLength;
	}

	return 0;
}

#endif //ifdef WIN32


/**
funkcja ValidateFirstUtf8Character() wykonuje sprawdzenie poprawnosci pierwszego znaku
lancucha znakowego zakodowanego w UTF-8
(jeden znak zakodowany w UTF-8 moze byc zapisany nawet na 6 bajtach)

Parametry wywolania:
@param [in] utf8Char to lanuch znakowy, z ktorego sprawdzany jest pierwszy znak UTF-8
Za pomoca tej funkcji mozna zwalidowac wszystkie znaki lancucha UTF-8.
Wystarczy za kazdym wywolaniem funkcji, przekazac odpowiednio przesuniety wskaznik na lanuch znakowy.

@param [out, optional] bytesCount to adres zmiennej long, w ktorej zostanie zapisana ilosc bajtow,
na ktorych zakodowany jest pierwszy znak lancucha (parametr opcjonalny - mozna przekazac NULL)

Wartosc zwracana:
@retval 0 gdy znak jest poprawny (sukces operacji)
Odpowiednia wartosc ujemna w przypadku bledu:
@retval -1 jesli za utf8Char przekazano NULL
@retval -3 jesli pierwszy bajt wskazuje,
	ze poczatek lanucha nie przypada na poczatek poprawnego znaku zakodowanego w UTF-8
@retval -4 jesli pierwszy znak lancucha nie jest poprawna sekwencja UTF-8
*/
long ValidateFirstUtf8Character(char *utf8Char, long *bytesCount)
{
	long iter=0;
	long tempBytesCount=0;


	if(utf8Char == NULL)
		{ return -1; }
		

	//jesli najstarszy bit ustawiony na 1, to znak zapisany na wielu bajtach
	if((long)utf8Char[0] & 0x80) //binarnie 10000000
	{
		if( ((long)utf8Char[0] & 0xE0) == 0xC0 ) //maska 11100000, wynik 110bbbbb (pierwszy bajt z sekwencji 2 bajtow)
		{
			tempBytesCount=2;
		}
		else if(((long)utf8Char[0] & 0xF0) == 0xE0) //maska 11110000, wynik 1110bbbb (pierwszy bajt z sekewncji 3 bajtow)
		{
			tempBytesCount=3;
		}
		else if(((long)utf8Char[0] & 0xF8) == 0xF0) //maska 11111000, wynik 11110bbb (pierwszy bajt z sekwencji 4 bajtow)
		{
			tempBytesCount=4;
		}
		else if(((long)utf8Char[0] & 0xFC) == 0xF8) //maska 11111100, wynik 111110bb (pierwszy bajt z sekwencji 5 bajtow)
		{
			tempBytesCount=5;
		}
		else if(((long)utf8Char[0] & 0xFE) == 0xFC) //maska 11111110, wynik 1111110b (pierwszy bajt z sekwencji 6 bajtow)
		{
			tempBytesCount=6;
		}
		else //napotkal 10bbbbbb, czyli nie pierwszy bajt wielobajtowaego zapisu znaku
		{
			return -3;
		}
	}
	//jesli najstarszy bit ustawiony na 0, to znak zapisany na jednym bajcie
	else
	{
		tempBytesCount=1;
	}

	//sprawdzenie poprawnosci sekwencji UTF-8 (kazdego nie pierwszego znaku sekwencji UTF-8)
	for(iter=1; iter<tempBytesCount; iter++)
	{
		if(((long)utf8Char[iter] & 0xC0) != 0x80) // maska 11000000, wynik 10bbbbbb
		{
			return -4;
		}
	}

	*bytesCount=tempBytesCount;
	return 0;
}
