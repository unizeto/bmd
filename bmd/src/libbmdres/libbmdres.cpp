#define _WINSOCK2API_
#include<bmd/libbmdres/libbmdres.h>
#include<bmd/libbmdpki/libbmdpki.h>

/** PRIVATE
Funkcja _check_libbmdres_struct() sprawdza poprawnosc zainicjowanej/wykorzystywanej struktury libbmdres_t.

@param res (parametr wejsciowy) to wskaznik na sprawdzana strukture libbmdres_t

@retval 0 w przypadku sukcesu operacji
@retval w przypadku bledu odpowiednia wartosc ujemna:
@retval	-1 dla blednego argumentu res
@retval	-2 oraz -3 dla nieprawidlowego strumienia archiwum (pole archstream struktury libbmdres_t)
@retval	-4 oraz -5 dla nieprawidlowego strumienia dekompresji zip (pole zipstream struktury libbmdres_t)
@retval	-6 dla nieprawidlowej sciezki do folder tymczasowego (pole tempdir struktury libbmdres_t)
@retval	-7 dla nieprawidlowej sciezki do zdeszyfrowanego pliku archiwum zip (pole decifered_name struktury libbmdres_t)
*/
long _check_libbmdres_struct(libbmdres_t *res)
{
	if(res == NULL)
		{ return -1; }
	
	if( res->archstream == NULL)
		{ return -2; }
	if( (res->archstream->IsOk()) == false )
		{ return -3; }
	
	if( res->zipstream == NULL )
		{ return -4; }
	if( (res->zipstream->IsOk()) == false )
		{ return -5; }

	if( res->tempdir == NULL)
		{ return -6; }
	if( res->decifered_name == NULL )
		{ return -7; }

	return 0;
}

/** PRIVATE
Funkcja _decrypt_resources() deszyfruje archiwum zip z zasobami graficznymi. Zdeszyfrowane dane 
zaspisane sa w pliku w folderze plikow tymczasowych. Sciezka do pliku zapisana zostaje w polu decifered_name
przekazywanej struktury libbmdres_t.

@param res (parametr wejsciowy) to wskaznik na strukture libbmdres_t
@param file_name (parametr wejsciowy) to nazwa/sciezka do pliku z zaszyfrowanym archiwum zip przechowujacym
		zasoby graficzne

@retval 0 w przypadku sukcesu operacji
@retval w przypadku bledu odpowiednia wartosc ujemna:
@retval	-1 dla blednego argumentu res
@retval	-2 oraz -3, gdy nie mozna otworzyc pliku file_name
@retval -4 jesli nie mozna odczytac dlugosci pliku file_name
@retval -5 przy bledzie alokacji pamieci
@retval -6 przy bledzie otwierania strumienia pliku file_name
@retval -7 przy bledzie czytania strumienia pliku file_name
@retval -8 przy bledzie ustawiania symetrycznego kontekstu deszyfrujacego
@retval -9 przy niepowodzeniu deszyfrowania danych
@retval -10 przy bledzie otwierania strumienia do zapisu strumienia pliku tymczaswego ze zdeszyfrowanym archiwum
@retval -11 przy bledzie zapisu do strumienia
@retval -12 przy niepowodzeniu ustawiania nazwy pliku tymczasowego ze zdeszyfrowanym archiwum zip
*/
long _decrypt_resources(libbmdres_t *res, wxString& file_name )
{
	char res_pass[12];
	bmd_crypt_ctx_t *ctx_dec=NULL;
	GenBuf_t enc_buf, *dec_buf=NULL;
	long result=0;
		
	res_pass[0]=0x5E; res_pass[1]=0x78; res_pass[2]=0x50;
	res_pass[3]=0x2F; res_pass[4]=0x39; res_pass[5]=0x74;
	res_pass[6]=0x5D; res_pass[7]=0x46; res_pass[8]=0x61;
	res_pass[9]=0x52; res_pass[10]=0x67; res_pass[11]=0x36;

	if(res == NULL)
		{ return -1; }

	wxFFile *src_file=new wxFFile( file_name );
	if(src_file == NULL)
		{ return -2; }
	if( (src_file->IsOpened()) == false)
	{
		delete src_file;
		return -3;
	}

	enc_buf.size=src_file->Length();
	delete src_file; //zamykanie pliku do odczytu
	if(enc_buf.size < 0)
		{ return -4; }
	
	enc_buf.buf=(char*)calloc(enc_buf.size, 1);
	if(enc_buf.buf == NULL)
		{ return -5; }
	
	wxFFileInputStream ffinput(file_name);
	if( (ffinput.IsOk()) == false)
	{
		free(enc_buf.buf);
		return -6;
	}
	ffinput.Read(enc_buf.buf,enc_buf.size);
	if( (ffinput.LastRead()) < (unsigned long)enc_buf.size )
	{
		free(enc_buf.buf);
		return -7;
	}
			
	if( (bmd_gen_sym_ctx_with_pass(BMD_CRYPT_ALGO_DES3,BMD_HASH_ALGO_SHA1,res_pass,12,&ctx_dec)) != 0 )
	{
		free(enc_buf.buf);
		return -8;
	}

	result=bmdpki_symmetric_decrypt(ctx_dec, &enc_buf, BMDPKI_SYM_LAST_CHUNK, &dec_buf);
	free(enc_buf.buf);
	bmd_ctx_destroy(&ctx_dec);
	if( result != 0 )
		{ return -9; }

	wxString dec_name(wxFileName::CreateTempFileName( *(res->tempdir)));
	wxFFileOutputStream dec_ffstream( dec_name );
	if( (dec_ffstream.IsOk()) == false)
	{
		free_gen_buf(&dec_buf);
		return -10;
	}
	
	dec_ffstream.Write(dec_buf->buf,dec_buf->size);
	if( (dec_ffstream.LastWrite()) < (unsigned long)dec_buf->size)
	{
		free_gen_buf(&dec_buf);
		dec_ffstream.Close();
		return -11;
	}
	free_gen_buf(&dec_buf);
	dec_ffstream.Close();

	res->decifered_name=new wxString( dec_name ); 
	if(res->decifered_name == NULL)
		{ return -12; }
	return 0;
}



/** PUBLIC
patrz dokumentacja docs/libbmres.txt
*/
long libbmdres_init(wxString& archive_name, libbmdres_t **res)
{
	libbmdres_t *resData=NULL;
	
	if( res == NULL )
		{ return -1; }
	if( (*res) != NULL )
		{ return -1; }
		
	resData = (libbmdres_t*)calloc(1, sizeof(libbmdres_t));
	if(resData == NULL)
		{ return -2; }

	resData->tempdir=new wxString(wxFileName::GetTempDir() + wxString(_("\\")) );
	if( (resData->tempdir) == NULL)
	{
		free(resData);
		return -3;
	}

	//dzeszyfrowanie i ustawienie decifered_name
	if( (_decrypt_resources(resData, archive_name)) != 0 )
	{
		delete resData->tempdir;
		free(resData);
		return -4;
	}

	resData->archstream=new wxFFileInputStream( *(resData->decifered_name) );
	if( resData->archstream == NULL)
	{
		delete resData->tempdir;
		delete resData->decifered_name;
		free(resData);
		return -5;
	}
	if( (resData->archstream->IsOk()) == false )
	{
		delete resData->tempdir;
		delete resData->decifered_name;
		delete (resData->archstream);
		free(resData);
		return -6;
	}

	resData->zipstream=new wxZipInputStream(*(resData->archstream));
	if(resData->zipstream == NULL)
	{
		delete resData->tempdir;
		delete resData->decifered_name;
		delete (resData->archstream);
		free(resData);
		return -7;
	}
	if( (resData->zipstream->IsOk()) == false)
	{
		delete resData->tempdir;
		delete resData->decifered_name;
		delete (resData->archstream);
		delete (resData->zipstream);
		free(resData);
		return -8;
	}

	*res=resData;
	return 0;
}


/** PUBLIC
patrz dokumentacja docs/libbmres.txt
*/
long libbmdres_get_wxIcon(libbmdres_t *res, wxString& icon_filename, wxIcon** icon)
{
	wxZipEntry *entry=NULL;
	long ile_wpisow=0;
	long offset=0;
	long rozmiar=0;
	unsigned char* bufor=NULL;
	wxIcon *ikona=NULL;
	
	long check_status=_check_libbmdres_struct(res);
	//jesli cos nie tak ze struktura, to nic nie bedzie na niej wykonywane
	if( check_status != 0)
		{ return -1; }

	if(icon == NULL)
		{ return -2; }
	if( (*icon) != NULL)
		{ return -2; }
	
	ile_wpisow=res->zipstream->GetTotalEntries();
	//utworzenie unikatowej nazwy pliku tymczasowego
	wxString temp_filename( ( wxFileName::CreateTempFileName( *(res->tempdir) )) );
	
	while( (entry=res->zipstream->GetNextEntry()) != NULL)
	{
		offset=entry->GetOffset();
		rozmiar=entry->GetSize();
		res->zipstream->CloseEntry();
		wxString internal_name(entry->GetInternalName());
		delete entry;

		if( (icon_filename.Cmp(internal_name)) == 0) //jesli to wpis, ktorego szukamy
		{
			bufor=(unsigned char*)calloc(rozmiar, 1);
			if(bufor == NULL)
				{ return -3; }
			//odczytuje plik z archiwum
			res->zipstream->SeekI(offset);
			res->zipstream->Read(bufor, rozmiar);
			
			//zapisuje do pliku tymczasowego
			wxFFileOutputStream outstream(temp_filename);
			if( (outstream.IsOk()) == false)
			{
				free(bufor);
				return -4;
			}
			outstream.Write(bufor, rozmiar);
			outstream.Close();

			free(bufor);
			//stworzenie wxIcon
			ikona=new wxIcon(temp_filename,wxBITMAP_TYPE_ICO);
			//po odczycie ikony z pliku tymczasewego- usuwam go
			wxRemoveFile(temp_filename);
			if(ikona == NULL)
				{ return -5; }
			if( (ikona->IsOk()) == false)
			{
				delete ikona;
				return -6; 
			}
						
			//ustawienie na paczatek streama archiwum zip
			res->archstream->SeekI(0);
			//ponowne otwarcie i ustawienie na paczatek streamu dekompresji zip
			delete (res->zipstream);
			res->zipstream= new wxZipInputStream( *(res->archstream));
			if( res->zipstream == NULL)
			{ 
				//aby uniknac niespojnosci, nie zwroci ikony nawet, gdy pozniejsza operacja zawiodla
				delete ikona; 
				return -7; 
			}
			if( (res->zipstream->IsOk()) == false )
			{ 
				//aby uniknac niespojnosci, nie zwroci ikony nawet, gdy pozniejsza operacja zawiodla
				delete ikona; 
				return -8; 
			}
			//jesli tu doszlo, jest ok
			*icon=ikona;
			return 0;
		}
	}
	return -9; //nie znalazl
}


/** PUBLIC
patrz dokumentacja docs/libbmres.txt
*/
long libbmdres_get_wxBitmap(libbmdres_t *res, wxString& bitmap_filename, long wxBITMAP_TYPE, wxBitmap** bitmap)
{
	wxZipEntry *entry=NULL;
	long ile_wpisow=0;
	long offset=0;
	long rozmiar=0;
	unsigned char* bufor=NULL;
	wxBitmap *bitmapa=NULL;
	
	long check_status=_check_libbmdres_struct(res);
	//jesli cos nie tak ze struktura, to nic nie bedzie na niej wykonywane
	if( check_status != 0)
		{ return -1; }

	if(bitmap == NULL)
		{ return -2; }
	if( (*bitmap) != NULL)
		{ return -2; }
	
	ile_wpisow=res->zipstream->GetTotalEntries();
	//utworzenie unikatowej nazwy pliku tymczasowego
	wxString temp_filename( ( wxFileName::CreateTempFileName( *(res->tempdir) )) );
	
	while( (entry=res->zipstream->GetNextEntry()) != NULL)
	{
		offset=entry->GetOffset();
		rozmiar=entry->GetSize();
		res->zipstream->CloseEntry();
		wxString internal_name(entry->GetInternalName());
		delete entry;

		if( (bitmap_filename.Cmp(internal_name)) == 0) //jesli to wpis, ktorego szukamy
		{
			bufor=(unsigned char*)calloc(rozmiar, 1);
			if(bufor == NULL)
				{ return -3; }
			//odczytuje plik z archiwum
			res->zipstream->SeekI(offset);
			res->zipstream->Read(bufor, rozmiar);
			
			//zapisuje do pliku tymczasowego
			wxFFileOutputStream outstream(temp_filename);
			if( (outstream.IsOk()) == false)
			{
				free(bufor);
				return -4;
			}
			outstream.Write(bufor, rozmiar);
			outstream.Close();

			free(bufor);
			//stworzenie wxBitmap
			bitmapa=new wxBitmap(temp_filename,wxBITMAP_TYPE);
			//po odczycie ikony z pliku tymczasewego- usuwam go
			wxRemoveFile(temp_filename);
			if( bitmapa == NULL )
				{ return -5; }
			if( (bitmapa->IsOk()) == false)
			{
				delete bitmapa;
				return -6;
			}
						
			//ustawienie na paczatek streama archiwum zip
			res->archstream->SeekI(0);
			//ponowne otwarcie i ustawienie na paczatek streamu dekompresji zip
			delete (res->zipstream);
			res->zipstream= new wxZipInputStream( *(res->archstream));
			if( res->zipstream == NULL)
			{ 
				//aby uniknac niespojnosci, nie zwroci bitmapy nawet, gdy pozniejsza operacja zawiodla
				delete bitmapa;
				return -7;
			}
			if( (res->zipstream->IsOk()) == false )
			{
				//aby uniknac niespojnosci, nie zwroci bitmapy nawet, gdy pozniejsza operacja zawiodla
				delete bitmapa;
				return -8; 
			}
			//jesli tu doszlo, jest ok
			*bitmap=bitmapa;
			return 0;
		}
	}
	return -9; //nie znalazl
}


/** PUBLIC
patrz dokumentacja docs/libbmres.txt
*/
long libbmdres_destroy(libbmdres_t **res)
{
	if(res != NULL)
	{
		if(*res != NULL)
		{
			if( (*res)->archstream )
				{ delete ( (*res)->archstream ); }
			if( (*res)->zipstream )
				{ delete ((*res)->zipstream ); }
			if( (*res)->tempdir )
				{ delete ( (*res)->tempdir ); }
			if( (*res)->decifered_name )
			{
				wxRemoveFile( *((*res)->decifered_name) );
				delete ( (*res)->decifered_name );
			}
			free(*res); *res=NULL;
		}
	}
	return 0;
}
