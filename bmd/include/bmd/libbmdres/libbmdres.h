#ifndef _HAVE_LIBBMDRES_LIBRARY
#define _HAVE_LIBBMDRES_LIBRARY
#include <wx/wx.h>
#include <wx/icon.h>
#include <wx/bitmap.h>
#include <wx/zipstrm.h>
#include <wx/wfstream.h>



struct libbmdres
{
	wxFFileInputStream *archstream; //strumien archiwum zip
	wxZipInputStream *zipstream; //strumien dekompresji zip oparty na archstream
	wxString *tempdir; //sciezka do katalogu plikow tymczasowych
	wxString *decifered_name; //sciezka bezwzgledna pliku tymczasowego ze zdekodowana zawartoscia (archiwum z zasobami graficznymi)
};
	
typedef struct libbmdres libbmdres_t;

/*PRIVATE*/
long _check_libbmdres_struct(libbmdres_t *res);
long _decrypt_resources(libbmdres_t *res, wxString& file_name);

/*PUBLIC API*/
long libbmdres_init(wxString& archive_name, libbmdres_t **res);
//icon_filename to sama nazwa pliku (ewentulanie "folder_w_archiwum\nazwa_pliku" itp.) - nazwy wewnetrzne w archiwum
long libbmdres_get_wxIcon(libbmdres_t *res, wxString& icon_filename, wxIcon** icon);
//bitmap_filename to sama nazwa pliku (ewentulanie "folder_w_archiwum\nazwa_pliku" itp.) - nazwy wewnetrzne w archiwum
long libbmdres_get_wxBitmap(libbmdres_t *res, wxString& bitmap_filename, long wxBITMAP_TYPE, wxBitmap** bitmap);
long libbmdres_destroy(libbmdres_t **res);

#endif //_HAVE_LIBBMDRES_LIBRARY
