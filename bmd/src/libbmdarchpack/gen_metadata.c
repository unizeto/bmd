#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdarchpack/gen_metadata.h>
#include <bmd/common/global_types.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmdutils/libbmdutils.h>

//#if defined(LIBXML_TREE_ENABLED) && defined(LIBXML_OUTPUT_ENABLED)



/**
*/

long initMetadata( metadata_t** const metadata_tParam )
{

    if ( metadata_tParam == NULL || *metadata_tParam != NULL )
     {
        PRINT_ERROR("Nieprawidłowy pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
        return BMD_ERR_PARAM1;
     }

    *metadata_tParam = calloc(1, sizeof(metadata_t));

    if ( *metadata_tParam == NULL )
     {
        PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
        return BMD_ERR_MEMORY;
     }

    return BMD_OK;

}

/**
 */


long genId_t( Id_t** const Id_tParam, char* const value, char* const type )
 {

    if ( Id_tParam == NULL || *Id_tParam != NULL )
     {
        PRINT_ERROR("Nieprawidłowy pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
        return BMD_ERR_PARAM1;
     }

    if ( value == NULL )
     {
        PRINT_ERROR("Nieprawidłowy drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
        return BMD_ERR_PARAM2;
     }

    if ( type == NULL )
     {
        PRINT_ERROR("Nieprawidłowy trzeci parametr. Error = %d\n", BMD_ERR_PARAM3);
        return BMD_ERR_PARAM3;
     }

    *Id_tParam = (Id_t*)calloc(1, sizeof(Id_t));

    if (*Id_tParam == NULL)
     {
        PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
        return BMD_ERR_MEMORY;
     }

    asprintf( &((*Id_tParam)->value), "%s", value);
    asprintf( &((*Id_tParam)->type), "%s", type);

    return BMD_OK;
 }

/**
 */


long genAdres( adres_t** const adres_tParam, char* const kod, char* const poczta, \
                char* const miejscowosc, char* const ulica, char* const budynek, \
                char* const lokal, char* const skrytkapocztowa, char* const uwagi, char* const kraj )
 {

   if ( adres_tParam == NULL || *adres_tParam != NULL )
    {
       PRINT_ERROR("Nieprawidłowy pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
       return BMD_ERR_PARAM1;
    }

   if ( miejscowosc == NULL )
    {
       PRINT_ERROR("Nieprawidłowy czwarty parametr. Error = %d\n", BMD_ERR_PARAM4);
       return BMD_ERR_PARAM4;
    }

   if ( kraj == NULL )
    {
       PRINT_ERROR("Nieprawidłowy dziesiąty parametr. Error = %d\n", BMD_ERR_PARAM10);
       return BMD_ERR_PARAM10;
    }

    *adres_tParam = (adres_t*)calloc(1, sizeof(adres_t));

    if ( *adres_tParam == NULL)
     {
        PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
        return BMD_ERR_MEMORY;
     }

    asprintf( &((*adres_tParam)->miejscowosc), "%s", miejscowosc);
    asprintf( &((*adres_tParam)->kraj), "%s", kraj);

    if ( kod )
     {
        asprintf( &((*adres_tParam)->kod), "%s", kod);
     }

    if ( poczta )
     {
         asprintf( &((*adres_tParam)->poczta), "%s", poczta);
     }

    if ( ulica )
     {
         asprintf( &((*adres_tParam)->ulica), "%s", ulica);
     }

    if ( budynek )
     {
         asprintf( &((*adres_tParam)->budynek), "%s", budynek);
     }

    if ( lokal )
     {
         asprintf( &((*adres_tParam)->lokal), "%s", lokal);
     }

    if ( skrytkapocztowa )
     {
         asprintf( &((*adres_tParam)->skrytkapocztowa), "%s", skrytkapocztowa);
     }

    if ( uwagi )
     {
         asprintf( &((*adres_tParam)->uwagi), "%s", uwagi);
     }

    return BMD_OK;
 }

/**
 */

long genKontakt( kontakt_t** const kontakt_tParam, enumKontakt_t typ, char* const value )
 {

   if ( kontakt_tParam == NULL || *kontakt_tParam != NULL )
    {
       PRINT_ERROR("Nieprawidłowy pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
       return BMD_ERR_PARAM1;
    }

   *kontakt_tParam = (kontakt_t*)calloc(1, sizeof(kontakt_t));

   if (*kontakt_tParam == NULL)
    {
       PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
       return BMD_ERR_MEMORY;
    }

    switch( typ )
     {
         case email   : asprintf( &((*kontakt_tParam)->typ), "%s", APM_Email ); break;
         case telefon : asprintf( &((*kontakt_tParam)->typ), "%s", APM_Telefon ); break;
         case telkom  : asprintf( &((*kontakt_tParam)->typ), "%s", APM_Telkom ); break;
         case faks    : asprintf( &((*kontakt_tParam)->typ), "%s", APM_Faks );
     }

    asprintf( &((*kontakt_tParam)->value), "%s", value );

   return BMD_OK;

 }

/**
 */


 long genInstytucja( instytucja_t** const instytucja_tParam, Id_t* const id, \
                     char* const nazwa, adres_t* const adres, kontakt_t** const kontakt, \
                     jednostka_t* const jednostka  )
  {

     if ( instytucja_tParam == NULL || *instytucja_tParam != NULL  )
      {
          PRINT_ERROR("Nieprawidłowy pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
          return BMD_ERR_PARAM1;
      }

     if ( nazwa == NULL )
      {
          PRINT_ERROR("Nieprawidłowy trzeci parametr. Error = %d\n", BMD_ERR_PARAM3);
          return BMD_ERR_PARAM3;
      }

     *instytucja_tParam = (instytucja_t*)calloc(1, sizeof(instytucja_t));

     if ( *instytucja_tParam == NULL)
      {
         PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
         return BMD_ERR_MEMORY;
      }

     asprintf( &((*instytucja_tParam)->nazwa), "%s", nazwa );

     (*instytucja_tParam)->id = id;
     (*instytucja_tParam)->adres = adres;
     (*instytucja_tParam)->kontakt = kontakt;
     (*instytucja_tParam)->jednostka = jednostka;

    return BMD_OK;

  }

/**
 */

long genPodmiot( podmiot_t** const podmiot_tParam, osoba_t* const osoba, instytucja_t* const instytucja )
{

    if ( podmiot_tParam == NULL || *podmiot_tParam != NULL  )
     {
        PRINT_ERROR("Nieprawidłowy pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
        return BMD_ERR_PARAM1;
     }

    *podmiot_tParam = (podmiot_t*)calloc(1, sizeof(podmiot_t));

    if ( *podmiot_tParam == NULL)
     {
        PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
        return BMD_ERR_MEMORY;
     }

    if ( instytucja != NULL )
     {
        (*podmiot_tParam)->instytucja = instytucja;
     }
    else if ( osoba != NULL )
     {
        (*podmiot_tParam)->osoba = osoba;
     }
    else
     {
        PRINT_ERROR("Błąd parametrów drugiego i trzeciego. Error = %d\n", BMD_ERR_PARAM2);
        return BMD_ERR_PARAM2;
     }

    return BMD_OK;

}

/**
*/

long genOdbiorca( odbiorca_t** const odbiorca_tParam, podmiot_t* const podmiot, char* const rodzaj )
{

   if ( odbiorca_tParam == NULL || *odbiorca_tParam != NULL )
    {
       PRINT_ERROR("Nieprawidłowy pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
       return BMD_ERR_PARAM1;
    }

   if ( podmiot == NULL )
    {
       PRINT_ERROR("Nieprawidłowy drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
       return BMD_ERR_PARAM2;
    }

   if ( rodzaj == NULL )
    {
       PRINT_ERROR("Nieprawidłowy trzeci parametr. Error = %d\n", BMD_ERR_PARAM3);
       return BMD_ERR_PARAM3;
    }

    *odbiorca_tParam = (odbiorca_t*)calloc(1, sizeof(odbiorca_t));

   if ( *odbiorca_tParam == NULL)
    {
        PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
        return BMD_ERR_MEMORY;
    }

   asprintf( &((*odbiorca_tParam)->rodzaj), "%s", rodzaj );

   (*odbiorca_tParam)->podmiot = podmiot;

   return BMD_OK;

}

 /**
 */

long genPrzedzialCzasu( przedzialCzasu_t** const przedzialCzasu_tParam, char* const _od, char* const _do )
 {

    if ( przedzialCzasu_tParam == NULL || *przedzialCzasu_tParam != NULL )
     {
        PRINT_ERROR("Nieprawidłowy pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
        return BMD_ERR_PARAM1;
     }

    if ( _od == NULL )
     {
        PRINT_ERROR("Nieprawidłowy drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
        return BMD_ERR_PARAM2;
     }

    if ( _do == NULL )
     {
        PRINT_ERROR("Nieprawidłowy trzeci parametr. Error = %d\n", BMD_ERR_PARAM3);
        return BMD_ERR_PARAM3;
     }

    *przedzialCzasu_tParam = (przedzialCzasu_t*)calloc(1, sizeof(przedzialCzasu_t));

    if ( *przedzialCzasu_tParam == NULL )
     {
         PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
         return BMD_ERR_MEMORY;
     }

    asprintf(&((*przedzialCzasu_tParam)->_od), "%s", _od);
    asprintf(&((*przedzialCzasu_tParam)->_do), "%s", _do);


   return BMD_OK;


 }

 /**
  */

 long genData( data_t** const data, enumTypDaty_t typ, char* const czas, \
               przedzialCzasu_t* const przedzialCzasu )
 {

   if ( data == NULL || *data != NULL )
    {
       PRINT_ERROR("Nieprawidłowy pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
       return BMD_ERR_PARAM1;
    }

   if ( czas == NULL && przedzialCzasu == NULL )
    {
       PRINT_ERROR("Nieprawidłowy drugi i trzeci parametr. Error = %d\n", BMD_ERR_PARAM2);
       return BMD_ERR_PARAM2;
    }

   *data = (data_t*)calloc(1, sizeof(data_t));

   if ( *data == NULL )
    {
       PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
       return BMD_ERR_MEMORY;
    }

    if ( czas )
     {
        asprintf(&((*data)->czas), "%s", czas);
     }
    else
     {
        (*data)->przedzialCzasu = przedzialCzasu;
     }

    switch( typ )
     {
         case dostepnyPo    : asprintf(&((*data)->typDaty), "%s", APM_DOSTEPNY_PO ); break;
         case opublikowany  : asprintf(&((*data)->typDaty), "%s", APM_OPUBLIKOWANY ); break;
         case stworzony     : asprintf(&((*data)->typDaty), "%s", APM_STWORZONY ); break;
         case uzyskany      : asprintf(&((*data)->typDaty), "%s", APM_UZYSKANY ); break;
         case otrzymany     : asprintf(&((*data)->typDaty), "%s", APM_OTRZYMANY ); break;
         case wyslany       : asprintf(&((*data)->typDaty), "%s", APM_WYSLANY ); break;
         case zaakceptowany : asprintf(&((*data)->typDaty), "%s", APM_ZAAKCEPTOWANY ); break;
         case zatwierdzony  : asprintf(&((*data)->typDaty), "%s", APM_ZATWIERDZONY ); break;
         case zmodyfikowany : asprintf(&((*data)->typDaty), "%s", APM_ZMODYFIKOWANY ); break;
         case datySkrajne   : asprintf(&((*data)->typDaty), "%s", APM_DATYSKRAJNE ); 
     }

     return BMD_OK;

 }

/**
 */

long genDostep( dostep_t** const dostep_tParam, enumDostepnosc_t dostepnosc, \
                 char* const uwagi, data_t* const data )
 {

    if ( dostep_tParam == NULL || *dostep_tParam != NULL )
     {
        PRINT_ERROR("Nieprawidłowy pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
        return BMD_ERR_PARAM1;
     }

    *dostep_tParam = (dostep_t*)calloc(1, sizeof(dostep_t));

    if ( *dostep_tParam == NULL )
     {
         PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
         return BMD_ERR_MEMORY;
     }

    switch(dostepnosc)
     {
         case wszystko    : asprintf(&((*dostep_tParam)->dostepnosc), "%s", APM_WSZYSTKO ); break;
         case metadane    : asprintf(&((*dostep_tParam)->dostepnosc), "%s", APM_METADANE ); break;
         case niedostepne : asprintf(&((*dostep_tParam)->dostepnosc), "%s", APM_NIEDOSTEPNE );
     }

     if ( uwagi )
      {
         asprintf(&((*dostep_tParam)->uwagi), "%s", uwagi );
      }

     (*dostep_tParam)->data = data;


    return BMD_OK;


 }

/**
 */

long genFormat( format_t** const format_tParam, char* const typ, \
                char* const specyfikacja, char* const wielkosc )

{

   if ( format_tParam == NULL || *format_tParam != NULL )
    {
       PRINT_ERROR("Nieprawidłowy pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
       return BMD_ERR_PARAM1;
    }

   if ( typ == NULL )
    {
       PRINT_ERROR("Nieprawidłowy drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
       return BMD_ERR_PARAM2;
    }

   *format_tParam = (format_t*)calloc(1, sizeof(format_t));

   if ( *format_tParam == NULL )
    {
       PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
       return BMD_ERR_MEMORY;
    }

   asprintf(&((*format_tParam)->typ), "%s", typ );

   if ( specyfikacja )
    {
       asprintf(&((*format_tParam)->specyfikacja), "%s", specyfikacja );
    }

   if ( wielkosc )
    {
       asprintf(&((*format_tParam)->wielkosc), "%s", wielkosc );
    }

   return BMD_OK;

}

/**
 */

long genIdentyfikator( Identyfikator_t** const Identyfikator_tParam, char* const typ, \
                       char* const wartosc, podmiot_t* const podmiot )

 {

    if ( Identyfikator_tParam  == NULL || *Identyfikator_tParam != NULL )
     {
        PRINT_ERROR("Nieprawidłowy pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
        return BMD_ERR_PARAM1;
     }

    if ( typ == NULL )
     {
        PRINT_ERROR("Nieprawidłowy drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
        return BMD_ERR_PARAM2;
     }

    if ( wartosc == NULL )
     {
        PRINT_ERROR("Nieprawidłowy trzeci parametr. Error = %d\n", BMD_ERR_PARAM3);
        return BMD_ERR_PARAM3;
     }

    *Identyfikator_tParam = (Identyfikator_t*)calloc(1, sizeof(Identyfikator_t));

    if ( *Identyfikator_tParam == NULL )
     {
         PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
         return BMD_ERR_MEMORY;
     }

    asprintf(&((*Identyfikator_tParam)->typ), "%s", typ );
    asprintf(&((*Identyfikator_tParam)->wartosc), "%s", wartosc );

    (*Identyfikator_tParam)->podmiot = podmiot;

    return BMD_OK;

 }

/**
 */

 long genJezyk( jezyk_t** const jezyk_tParam, enumJezyk_t kod, char* const value )
  {

    if ( jezyk_tParam == NULL || *jezyk_tParam != NULL )
     {
        PRINT_ERROR("Nieprawidłowy pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
        return BMD_ERR_PARAM1;
     }

    if ( value == NULL )
     {
        PRINT_ERROR("Nieprawidłowy drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
        return BMD_ERR_PARAM2;
     }

    *jezyk_tParam = (jezyk_t*)calloc(1, sizeof(jezyk_t));

    if ( *jezyk_tParam == NULL )
     {
        PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
        return BMD_ERR_MEMORY;
     }

    asprintf(&((*jezyk_tParam)->value), "%s", value );

    switch( kod )
     {
        case pol : asprintf(&((*jezyk_tParam)->kod), "pol");
     }

    return BMD_OK;

  }

/**
 */

long genRelacja( relacja_t** const relacja_tParam, Identyfikator_t* const Identyfikator_tParam, \
                 enumTypRelacji_t typ )

 {

    if ( relacja_tParam == NULL || *relacja_tParam != NULL )
     {
         PRINT_ERROR("Nieprawidłowy pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
         return BMD_ERR_PARAM1;
     }

    if ( Identyfikator_tParam == NULL )
     {
         PRINT_ERROR("Nieprawidłowy drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
         return BMD_ERR_PARAM2;
     }

    *relacja_tParam = (relacja_t*)calloc(1, sizeof(relacja_t));

    if ( *relacja_tParam == NULL )
     {
         PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
         return BMD_ERR_MEMORY;
     }

    (*relacja_tParam)->identyfikator = Identyfikator_tParam;

    switch(typ)
     {
         case maFormat       : asprintf(&((*relacja_tParam)->typ), "%s", APM_maFormat ); break;
         case jestFormatem   : asprintf(&((*relacja_tParam)->typ), "%s", APM_jestFormatem ); break;
         case maWersje       : asprintf(&((*relacja_tParam)->typ), "%s", APM_maWersje ); break;
         case jestWersja     : asprintf(&((*relacja_tParam)->typ), "%s", APM_jestWersja ); break;
         case jestCzescia    : asprintf(&((*relacja_tParam)->typ), "%s", APM_jestCzescia ); break;
         case maOdniesienie  : asprintf(&((*relacja_tParam)->typ), "%s", APM_maOdniesienie ); break;
         case odnosiSieDo    : asprintf(&((*relacja_tParam)->typ), "%s", APM_odnosiSieDo ); break;
         case maZrodlo       : asprintf(&((*relacja_tParam)->typ), "%s", APM_maZrodlo ); break;
         case wymaga         : asprintf(&((*relacja_tParam)->typ), "%s", APM_wymaga ); break;
         case jestWymagany   : asprintf(&((*relacja_tParam)->typ), "%s", APM_jestWymagany ); break;
         case maDekretacje   : asprintf(&((*relacja_tParam)->typ), "%s", APM_maDekretacje ); break;
         case jestDekretacja : asprintf(&((*relacja_tParam)->typ), "%s", APM_jestDekretacja );
     }

    return BMD_OK;

 }

/**
*/

long genTworca( tworca_t** const tworca_tParam, enumTworca_t funkcja, podmiot_t* const podmiot )
 {

    if ( tworca_tParam == NULL || *tworca_tParam != NULL )
     {
        PRINT_ERROR("Nieprawidłowy pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
        return BMD_ERR_PARAM1;
     }

    if ( podmiot == NULL )
     {
         PRINT_ERROR("Nieprawidłowy drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
         return BMD_ERR_PARAM2;
     }

    *tworca_tParam = (tworca_t*)calloc(1, sizeof(tworca_t));

    if ( *tworca_tParam == NULL )
     {
         PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
         return BMD_ERR_MEMORY;
     }

    (*tworca_tParam)->podmiot = podmiot;

    switch(funkcja)
     {
         case stworzyl    : asprintf(&((*tworca_tParam)->funkcja), "%s", APM_stworzyl ); break;
         case modyfikowal : asprintf(&((*tworca_tParam)->funkcja), "%s", APM_modyfikowal ); break;
         case zatwierdzil : asprintf(&((*tworca_tParam)->funkcja), "%s", APM_zatwierdzil );
     }

   return BMD_OK;

 }

/**
 */

long genTyp( typ_t** const typ_tParam, enumType_t kategoria, char* const rodzaj )
 {

    if ( typ_tParam == NULL || *typ_tParam != NULL )
     {
         PRINT_ERROR("Nieprawidłowy pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
         return BMD_ERR_PARAM1;
     }

    if ( rodzaj == NULL )
     {
         PRINT_ERROR("Nieprawidłowy trzeci parametr. Error = %d\n", BMD_ERR_PARAM3);
         return BMD_ERR_PARAM3;
     }

    *typ_tParam = (typ_t*)calloc(1, sizeof(typ_t));

    if ( *typ_tParam == NULL )
     {
         PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
         return BMD_ERR_MEMORY;
     }

    switch( kategoria )
     {
         case collection           : asprintf(&((*typ_tParam)->kategoria), "%s", APM_Collection ); break;
         case dataset              : asprintf(&((*typ_tParam)->kategoria), "%s", APM_Dataset ); break;
         case event                : asprintf(&((*typ_tParam)->kategoria), "%s", APM_Event ); break;
         case image                : asprintf(&((*typ_tParam)->kategoria), "%s", APM_Image ); break;
         case interactiveResource  : asprintf(&((*typ_tParam)->kategoria), "%s", APM_InteractiveResource ); break;
         case movingImage          : asprintf(&((*typ_tParam)->kategoria), "%s", APM_MovingImage ); break;
         case physicalObject       : asprintf(&((*typ_tParam)->kategoria), "%s", APM_PhysicalObject ); break;
         case Service              : asprintf(&((*typ_tParam)->kategoria), "%s", APM_Service ); break;
         case software             : asprintf(&((*typ_tParam)->kategoria), "%s", APM_Software ); break;
         case sound                : asprintf(&((*typ_tParam)->kategoria), "%s", APM_Sound ); break;
         case stillImage           : asprintf(&((*typ_tParam)->kategoria), "%s", APM_StillImage ); break;
         case text                 : asprintf(&((*typ_tParam)->kategoria), "%s", APM_Text );
     }

    asprintf(&((*typ_tParam)->rodzaj), "%s", rodzaj );

    return BMD_OK;

 }

/**
*/

long genTytulElement( tytulElement_t** const tytulElement_tParam, \
                      char* const value, char* const jezyk )
 {

    if ( tytulElement_tParam == NULL || *tytulElement_tParam != NULL )
     {
        PRINT_ERROR("Nieprawidłowy pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
        return BMD_ERR_PARAM1;
     }

    if ( value == NULL )
     {
        PRINT_ERROR("Nieprawidłowy drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
        return BMD_ERR_PARAM2;
     }

    *tytulElement_tParam = (tytulElement_t*)calloc(1, sizeof(tytulElement_t));

    if ( *tytulElement_tParam == NULL )
     {
        PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
        return BMD_ERR_MEMORY;
     }

    asprintf(&((*tytulElement_tParam)->value), "%s", value );

    if ( jezyk )
     {
        asprintf(&((*tytulElement_tParam)->jezyk), "%s", jezyk );
     }

    return BMD_OK;

 }

/**
*/

 long genTytulOryginalny( tytul_t** const tytul_tParam, tytulElement_t* const tytulElement_tParam )
  {

     if ( tytul_tParam == NULL || *tytul_tParam != NULL )
      {
         PRINT_ERROR("Nieprawidłowy pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
         return BMD_ERR_PARAM1;
      }

     if ( tytulElement_tParam == NULL )
      {
         PRINT_ERROR("Nieprawidłowy drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
         return BMD_ERR_PARAM2;
      }

     *tytul_tParam = (tytul_t*)calloc(1, sizeof(tytul_t));

     if ( *tytul_tParam == NULL )
      {
          PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
          return BMD_ERR_MEMORY;
      }

      (*tytul_tParam)->oryginalny = tytulElement_tParam;

    return BMD_OK;

  }

/**
*/

long setOdbiorca( metadata_t* const metadata_tParam, odbiorca_t* const odbiorca_tParam )
 {

     /* ------------------------------- */

        odbiorca_t**   odbiorca = NULL;

     /* ------------------------------- */


     if ( metadata_tParam == NULL )
     {
         PRINT_ERROR("Nieprawidłowy pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
         return BMD_ERR_PARAM1;
     }

     if ( odbiorca_tParam == NULL )
     {
         PRINT_ERROR("Nieprawidłowy drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
         return BMD_ERR_PARAM2;
     }

     odbiorca = (odbiorca_t**)calloc(2, sizeof(odbiorca_t*)); /* ostatni element to strażnik */

     if ( odbiorca == NULL )
     {
         PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
         return BMD_ERR_MEMORY;
     }

     odbiorca[0] = odbiorca_tParam;

     metadata_tParam->odbiorca = odbiorca;

     return BMD_OK;

 }

/**
 */

long setData( metadata_t* const metadata_tParam, data_t* const data_tParam )
 {

     /* ----------------------- */

        data_t**   data = NULL;

     /* ----------------------- */


    if ( metadata_tParam == NULL )
     {
        PRINT_ERROR("Nieprawidłowy pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
        return BMD_ERR_PARAM1;
     }

    if ( data_tParam == NULL )
     {
        PRINT_ERROR("Nieprawidłowy drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
        return BMD_ERR_PARAM2;
     }

    data = (data_t**)calloc(2, sizeof(data_t*)); /* ostatni element to strażnik */

    if ( data == NULL )
     {
        PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
        return BMD_ERR_MEMORY;
     }

    data[0] = data_tParam;

    metadata_tParam->data = data;

  return BMD_OK;

 }

/**
 */

long setDostep( metadata_t* const metadata_tParam, dostep_t* const dostep_tParam )
 {

    if ( metadata_tParam == NULL )
     {
        PRINT_ERROR("Nieprawidłowy pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
        return BMD_ERR_PARAM1;
     }

    if ( dostep_tParam == NULL )
     {
        PRINT_ERROR("Nieprawidłowy drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
        return BMD_ERR_PARAM2;
     }

    metadata_tParam->dostep = dostep_tParam;


   return BMD_OK;

 }

/**
 */

long setFormat( metadata_t* const metadata_tParam, format_t* const format_tParam )
 {

     if ( metadata_tParam == NULL )
      {
         PRINT_ERROR("Nieprawidłowy pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
         return BMD_ERR_PARAM1;
      }

     if ( format_tParam == NULL )
      {
         PRINT_ERROR("Nieprawidłowy drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
         return BMD_ERR_PARAM2;
      }

     metadata_tParam->format = format_tParam;

   return BMD_OK;

 }

/**
 */

long setIdentyfikator( metadata_t* const metadata_tParam, Identyfikator_t* const Identyfikator_tParam )
 {

     /* ------------------------------------------ */

        Identyfikator_t**  identyfikator   = NULL;

     /* ------------------------------------------ */


    if ( metadata_tParam == NULL )
     {
         PRINT_ERROR("Nieprawidłowy pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
         return BMD_ERR_PARAM1;
     }

    if ( Identyfikator_tParam == NULL )
     {
         PRINT_ERROR("Nieprawidłowy drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
         return BMD_ERR_PARAM2;
     }

    identyfikator = (Identyfikator_t**)calloc(2, sizeof(Identyfikator_t*)); /* ostatni element to strażnik */

    if ( identyfikator == NULL )
     {
         PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
         return BMD_ERR_MEMORY;
     }

    identyfikator[0] = Identyfikator_tParam;

    metadata_tParam->identyfikator = identyfikator; 

    return BMD_OK;

 }

/**
 */

long setJezyk( metadata_t* const metadata_tParam, jezyk_t* const jezyk_tParam )
 {

     /* ----------------------- */

        jezyk_t** jezyk = NULL;

     /* ----------------------- */


     if ( metadata_tParam == NULL )
      {
         PRINT_ERROR("Nieprawidłowy pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
         return BMD_ERR_PARAM1;
      }

     if ( jezyk_tParam == NULL )
      {
         PRINT_ERROR("Nieprawidłowy drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
         return BMD_ERR_PARAM2;
      }

     jezyk = (jezyk_t**)calloc(2, sizeof(jezyk_t*)); /* ostatni element to strażnik */

     if ( jezyk == NULL )
      {
         PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
         return BMD_ERR_MEMORY;
      }

     jezyk[0] = jezyk_tParam;

     metadata_tParam->jezyk = jezyk;

    return BMD_OK;

 }

/**
 */

long setOpis( metadata_t* const metadata_tParam, char* const opis )
 {

    if ( metadata_tParam == NULL )
     {
         PRINT_ERROR("Nieprawidłowy pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
         return BMD_ERR_PARAM1;
     }

    if ( opis == NULL )
     {
         PRINT_ERROR("Nieprawidłowy drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
         return BMD_ERR_PARAM2;
     }

    asprintf(&(metadata_tParam->opis) , "%s", opis );

   return BMD_OK;

 }

/**
 */

long setUprawnienia( metadata_t* const metadata_tParam, char* const uprawnienia )
 {

    if ( metadata_tParam == NULL )
     {
        PRINT_ERROR("Nieprawidłowy pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
        return BMD_ERR_PARAM1;
     }

    if ( uprawnienia == NULL )
     {
        PRINT_ERROR("Nieprawidłowy drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
        return BMD_ERR_PARAM2;
     }

    asprintf(&(metadata_tParam->uprawnienia) , "%s", uprawnienia );

   return BMD_OK;

 }

/**
 */

 long setRelacja( metadata_t* const metadata_tParam, relacja_t* const relacja_tParam )
 {

     /* ---------------------------- */

        relacja_t**  relacja = NULL;

     /* ---------------------------- */


    if ( metadata_tParam == NULL )
     {
        PRINT_ERROR("Nieprawidłowy pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
        return BMD_ERR_PARAM1;
     }

    if ( relacja_tParam  == NULL )
     {
        PRINT_ERROR("Nieprawidłowy drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
        return BMD_ERR_PARAM2;
     }

    relacja = (relacja_t**)calloc(2, sizeof(relacja_t*)); /* ostatni element to strażnik */

    if ( relacja == NULL )
     {
        PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
        return BMD_ERR_MEMORY;
     }

    relacja[0] = relacja_tParam;

    metadata_tParam->relacja = relacja;

    return BMD_OK;

 }

/**
 */

 long setTworca( metadata_t* const metadata_tParam, tworca_t* const tworca_tParam )
  {

     if ( metadata_tParam == NULL )
      {
          PRINT_ERROR("Nieprawidłowy pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
          return BMD_ERR_PARAM1;
      }

     if ( tworca_tParam == NULL )
      {
          PRINT_ERROR("Nieprawidłowy drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
          return BMD_ERR_PARAM2;
      }

     metadata_tParam->tworca = tworca_tParam;

    return BMD_OK;

  }

/**
*/

 long setTyp( metadata_t* const metadata_tParam, typ_t* const typ_tParam )
  {

     if ( metadata_tParam == NULL )
      {
          PRINT_ERROR("Nieprawidłowy pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
          return BMD_ERR_PARAM1;
      }

     if ( typ_tParam == NULL )
      {
          PRINT_ERROR("Nieprawidłowy drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
          return BMD_ERR_PARAM2;
      }

     metadata_tParam->typ = typ_tParam;

     return BMD_OK;

  }

/**
 */

long setTytul( metadata_t* const metadata_tParam, tytul_t* const tytul_tParam )
  {

      if ( metadata_tParam == NULL )
       {
           PRINT_ERROR("Nieprawidłowy pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
           return BMD_ERR_PARAM1;
       }

      if ( tytul_tParam == NULL )
       {
           PRINT_ERROR("Nieprawidłowy drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
           return BMD_ERR_PARAM2;
       }

       metadata_tParam->tytul = tytul_tParam;

      return BMD_OK;

  }


/**
 */


long composeMetadataStruct( metadata_t** const metadata_tParam , Config_t* const config_tConf, \
                            bmdDatagram_t* const bmdDatagram_tDatagram )
 {

     /* ---------------------------------------- */

        instytucja_t*      instytucja    = NULL;
        long               i             =    0;
        long               longRet       =    0;
        Identyfikator_t*   identyfikator = NULL;
        podmiot_t*         podmiot       = NULL;
        tworca_t*          tworca        = NULL;
        tytulElement_t*    tytulElement  = NULL;
        tytul_t*           tytul         = NULL;
        data_t*            data          = NULL;
        data_t*            dataDostepu   = NULL;
        typ_t*             typ           = NULL;
        format_t*          format        = NULL;
        dostep_t*          dostep        = NULL;
        long*     	   table         = NULL;
        long		   tableSize     =    0;
        ANY_t                            ANYTmp;
        GenBuf_t                      tmpGenbuf;

     /* ---------------------------------------- */

    if ( metadata_tParam == NULL || *metadata_tParam != NULL )
     {
        PRINT_ERROR("Nieprawidłowy pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
        return BMD_ERR_PARAM1;
     }

    if ( config_tConf == NULL )
     {
        PRINT_ERROR("Nieprawidłowy drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
        return BMD_ERR_PARAM2;
     }

    if ( bmdDatagram_tDatagram == NULL )
     {
        PRINT_ERROR("Nieprawidłowy trzeci parametr. Error = %d\n", BMD_ERR_PARAM3);
        return BMD_ERR_PARAM3;
     }

    if ( bmdDatagram_tDatagram->datagramStatus != 0 )
     {
        PRINT_ERROR("Błąd obsługi datagramu. Error = %d\n", BMD_ERR_OP_FAILED);
        return BMD_ERR_OP_FAILED;
     }

    if ( bmdDatagram_tDatagram->no_of_additionalMetaData == 0)
     {
        PRINT_ERROR("Brak metadanych dodatkowych. Error = %d\n", BMD_ERR_OP_FAILED);
        return BMD_ERR_OP_FAILED;
     }

     initMetadata( metadata_tParam );

     for(i=0; i<bmdDatagram_tDatagram->no_of_additionalMetaData; i++)
      {
        /* Rozpoznano OID Identyfikatora */
        
	longRet =  str_of_OID_to_ulong( bmdDatagram_tDatagram->additionalMetaData[i]->OIDTableBuf, \
					&table, &tableSize);
					

	if ( longRet != 0 )
         {
    	    PRINT_ERROR("Bląd konwersji string OIDa na OID table. Error = %d\n", BMD_ERR_OP_FAILED);
    	    return BMD_ERR_OP_FAILED;
         }

        if ( OID_cmp2( table, \
                       tableSize, \
                       (config_tConf->identyfikator).table, \
                       tableSize)==0 )
         {
            ANYTmp.buf  = (unsigned char*)bmdDatagram_tDatagram->additionalMetaData[i]->AnyBuf;
            ANYTmp.size = bmdDatagram_tDatagram->additionalMetaData[i]->AnySize;
            Any2Octetstring2GenBuf(&ANYTmp, &tmpGenbuf);

            longRet = genIdentyfikator( &identyfikator, "PRIMARY_ID", (char*)tmpGenbuf.buf, NULL );
            longRet = setIdentyfikator( *metadata_tParam, identyfikator );

            free0(tmpGenbuf.buf);
         }

        else if ( OID_cmp2( table, \
                            tableSize, \
                            (config_tConf->tworca).table, \
                            tableSize)==0 )
         {
             ANYTmp.buf  = (unsigned char*)bmdDatagram_tDatagram->additionalMetaData[i]->AnyBuf;
             ANYTmp.size = bmdDatagram_tDatagram->additionalMetaData[i]->AnySize;
             Any2Octetstring2GenBuf(&ANYTmp, &tmpGenbuf);

             longRet = genInstytucja( &instytucja, NULL, (char*)tmpGenbuf.buf, NULL, NULL, NULL );
             longRet = genPodmiot( &podmiot, NULL, instytucja );


             longRet = genTworca( &tworca, stworzyl, podmiot );
             longRet = setTworca( *metadata_tParam, tworca );

             free0(tmpGenbuf.buf);
         }

        else if ( OID_cmp2( table, \
                            tableSize, \
                            (config_tConf->tytul).table, \
                            tableSize)==0 )
         {
             ANYTmp.buf  = (unsigned char*)bmdDatagram_tDatagram->additionalMetaData[i]->AnyBuf;
             ANYTmp.size = bmdDatagram_tDatagram->additionalMetaData[i]->AnySize;
             Any2Octetstring2GenBuf(&ANYTmp, &tmpGenbuf);

             longRet = genTytulElement( &tytulElement, (char*)tmpGenbuf.buf, APM_JEZYK1 );
             longRet = genTytulOryginalny( &tytul, tytulElement );

             longRet = setTytul( *metadata_tParam, tytul );

             free0(tmpGenbuf.buf);
         }


        else if ( OID_cmp2( table, \
                            tableSize, \
                            (config_tConf->typ).table, \
                            tableSize)==0 )
         {
             ANYTmp.buf  = (unsigned char*)bmdDatagram_tDatagram->additionalMetaData[i]->AnyBuf;
             ANYTmp.size = bmdDatagram_tDatagram->additionalMetaData[i]->AnySize;
             Any2Octetstring2GenBuf(&ANYTmp, &tmpGenbuf);

             longRet = genTyp( &typ, text, (char*)tmpGenbuf.buf );
             longRet = setTyp( *metadata_tParam, typ );

             free0(tmpGenbuf.buf);
         }

        else if ( OID_cmp2( table, \
                            tableSize, \
                            (config_tConf->format).table, \
                            tableSize)==0 )
         {
             ANYTmp.buf  = (unsigned char*)bmdDatagram_tDatagram->additionalMetaData[i]->AnyBuf;
             ANYTmp.size = bmdDatagram_tDatagram->additionalMetaData[i]->AnySize;
             Any2Octetstring2GenBuf(&ANYTmp, &tmpGenbuf);

             longRet = genFormat( &format, (char*)tmpGenbuf.buf, NULL, NULL );
             longRet = setFormat( *metadata_tParam, format );

             free0(tmpGenbuf.buf);
         }
         
        free0(table);
        tableSize = 0;
     }

     for(i=0; i<bmdDatagram_tDatagram->no_of_sysMetaData; i++)
      {
  
      	longRet =  str_of_OID_to_ulong( bmdDatagram_tDatagram->sysMetaData[i]->OIDTableBuf, \
					&table, &tableSize);
					
	if ( longRet != 0 )
         {
    	    PRINT_ERROR("Bląd konwersji string OIDa na OID table. Error = %d\n", BMD_ERR_OP_FAILED);
    	    return BMD_ERR_OP_FAILED;
         }
      
      
         if ( OID_cmp2( table, \
                        tableSize, \
                        (config_tConf->data).table, \
                        tableSize)==0 )
          {
              ANYTmp.buf  = (unsigned char*)bmdDatagram_tDatagram->sysMetaData[i]->AnyBuf;
              ANYTmp.size = bmdDatagram_tDatagram->sysMetaData[i]->AnySize;
              Any2Octetstring2GenBuf(&ANYTmp, &tmpGenbuf);

              longRet = genData( &data, stworzony, (char*)tmpGenbuf.buf, NULL );

              longRet = genData( &dataDostepu, dostepnyPo, (char*)tmpGenbuf.buf, NULL );
              longRet = setData( *metadata_tParam, data );

              free0(tmpGenbuf.buf);
          }
    
	free0( table);
	tableSize = 0;
      }

     longRet = genDostep( &dostep, wszystko, NULL, dataDostepu );

     longRet = setDostep( *metadata_tParam, dostep );

   return BMD_OK;

 }

 /**
 */

 long composeXMLDocument( metadata_t* const metadata_tParam, char* const filename )

 {
   /* ------------------------------- */
 
      xmlDocPtr   doc         = NULL;
      xmlNodePtr  root_node   = NULL;
      xmlNodePtr  node        = NULL;
      xmlNodePtr  node1       = NULL;
      xmlNodePtr  node2       = NULL;
      xmlNodePtr  node3       = NULL;
      xmlNsPtr    nsp         = NULL;
      char*       tmpFileName = NULL;

   /* ------------------------------- */


    LIBXML_TEST_VERSION;

    doc = xmlNewDoc(BAD_CAST "1.0");
    root_node = xmlNewNode(NULL, BAD_CAST "root");
    xmlDocSetRootElement(doc, root_node);
    nsp = xmlNewNs(root_node, BAD_CAST "http://www.mswia.gov.pl/standardy/ndap", BAD_CAST "ndap");
    xmlSetNs (root_node, nsp);
    nsp = xmlNewNs(root_node, BAD_CAST "http://www.w3.org/2000/09/xmldsig#", BAD_CAST "ds");
    nsp = xmlNewNs(root_node, BAD_CAST "http://gdf.org.pl/standards/gdf.1.0.0", BAD_CAST "gdf");
    nsp = xmlNewNs(root_node, BAD_CAST "http://www.w3.org/2001/XMLSchema-instance", BAD_CAST "xsi");

    xmlReconciliateNs(doc,root_node);

    /* pomijamy na razie odbiorce */

    /* generujemy pole daty */
    if (metadata_tParam->data && metadata_tParam->data[0])
     {
       node = xmlNewChild(root_node, NULL, BAD_CAST "data", NULL);
       xmlNewChild(node, NULL, BAD_CAST "typ", BAD_CAST (metadata_tParam->data[0])->typDaty );
       xmlNewChild(node, NULL, BAD_CAST "czas", BAD_CAST (metadata_tParam->data[0])->czas );
     }

    /* generujemy pole dostępu */

     if (metadata_tParam->dostep)
      {
         node = xmlNewChild(root_node, NULL, BAD_CAST "dostep", NULL);

         xmlNewChild(node, NULL, BAD_CAST "dostepnosc", BAD_CAST (metadata_tParam->dostep)->dostepnosc );
         node1 = xmlNewChild(node, NULL, BAD_CAST "data", NULL  );

         xmlNewChild(node1, NULL, BAD_CAST "typ", BAD_CAST ((metadata_tParam->dostep)->data)->typDaty );
         xmlNewChild(node1, NULL, BAD_CAST "czas", BAD_CAST ((metadata_tParam->dostep)->data)->czas );
      }

     /* generujemy pole formatu */
     if (metadata_tParam->format)
      {
         node = xmlNewChild(root_node, NULL, BAD_CAST "format", NULL);
         xmlNewChild(node, NULL, BAD_CAST "typ", BAD_CAST (metadata_tParam->format)->typ );
      }

      /* generujemy pole identyfikatora */
      if (metadata_tParam->identyfikator && metadata_tParam->identyfikator[0])
       {
          node = xmlNewChild(root_node, NULL, BAD_CAST "identyfikator", NULL);
          xmlNewChild(node, NULL, BAD_CAST "typ", BAD_CAST (metadata_tParam->identyfikator[0])->typ );
          xmlNewChild(node, NULL, BAD_CAST "wartosc", BAD_CAST (metadata_tParam->identyfikator[0])->wartosc );
       }

      /* generujemy pole jezyka */
      if (metadata_tParam->jezyk && metadata_tParam->jezyk[0])
       {
          node = xmlNewChild(root_node, NULL, BAD_CAST "jezyk", NULL);
          xmlNewProp(node, BAD_CAST "kod", BAD_CAST "polski");
       }

      /* generujemy pole tworcy */
      if (metadata_tParam->tworca)
       {
           node = xmlNewChild(root_node, NULL, BAD_CAST "tworca", NULL);
           xmlNewChild(node, NULL, BAD_CAST "funkcja", BAD_CAST (metadata_tParam->tworca)->funkcja );
           node1 = xmlNewChild(node, NULL, BAD_CAST "podmiot", NULL);
           node2 = xmlNewChild(node1, NULL, BAD_CAST "instytucja", NULL);
           node3 = xmlNewChild( node2, NULL, BAD_CAST "nazwa", \
                                BAD_CAST (metadata_tParam->tworca)->podmiot->instytucja->nazwa );
       }

       /* generujemy pole typu */
      if (metadata_tParam->typ)
       {
           node = xmlNewChild(root_node, NULL, BAD_CAST "typ", NULL);
           xmlNewChild(node, NULL, BAD_CAST "kategoria", BAD_CAST (metadata_tParam->typ)->kategoria );
           xmlNewChild(node, NULL, BAD_CAST "rodzaj", BAD_CAST (metadata_tParam->typ)->rodzaj );
       }

      if (metadata_tParam->tytul)
       {
           node  = xmlNewChild(root_node, NULL, BAD_CAST "tytul", NULL);
           node1 = xmlNewChild(node, NULL, BAD_CAST "oryginalny", BAD_CAST (metadata_tParam->tytul)->oryginalny->value );
           xmlNewProp(node1, BAD_CAST "jezyk", BAD_CAST BAD_CAST (metadata_tParam->tytul)->oryginalny->jezyk );
       }

    asprintf(&tmpFileName, "%s.xml", filename);

    xmlSaveFormatFileEnc(tmpFileName, doc, "UTF-8", 1);

    xmlFreeDoc(doc);

    xmlCleanupParser();

    xmlMemoryDump();

    free0(tmpFileName);

    return(0);
}

/**
*/


long clearKontakt( kontakt_t** kontakt_tParam )
 {

    if ( kontakt_tParam == NULL )
     {
         PRINT_ERROR("Nieprawidłowy pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
         return BMD_ERR_PARAM1;
     }

    if ( *kontakt_tParam )
     {
       free0( (*kontakt_tParam)->typ );
       free0( (*kontakt_tParam)->value );
       free0( (*kontakt_tParam) );
     }

    return BMD_OK;
 }

 /**
 */

 long clearJezyk( jezyk_t** jezyk_tParam )
  {

     if ( jezyk_tParam == NULL )
      {
          PRINT_ERROR("Nieprawidłowy pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
          return BMD_ERR_PARAM1;
      }

      if ( *jezyk_tParam )
       {
        free0( (*jezyk_tParam)->kod );
        free0( (*jezyk_tParam)->value );
        free0( (*jezyk_tParam) );
       }


     return BMD_OK;

  }

 /**
 */

 long clearAdres( adres_t** adres_tParam )
  {

     if ( adres_tParam == NULL )
      {
          PRINT_ERROR("Nieprawidłowy pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
          return BMD_ERR_PARAM1;
      }

     if ( *adres_tParam )
      {
        free0( (*adres_tParam)->poczta );
        free0( (*adres_tParam)->miejscowosc );
        free0( (*adres_tParam)->ulica );
        free0( (*adres_tParam)->budynek );
        free0( (*adres_tParam)->lokal );
        free0( (*adres_tParam)->skrytkapocztowa );
        free0( (*adres_tParam)->uwagi );
        free0( (*adres_tParam)->kraj );
        free0( (*adres_tParam) );
      }

     return BMD_OK;

  }

  /**
  */

  long clearId( Id_t** id_tParam )
   {
      if ( id_tParam == NULL )
       {
           PRINT_ERROR("Nieprawidłowy pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
           return BMD_ERR_PARAM1;
       }

      if ( *id_tParam )
       {
          free0( (*id_tParam)->value );
          free0( (*id_tParam)->type );
          free0( (*id_tParam) );
       }

      return BMD_OK;

   }

   /**
   */

  long clearInstytucja( instytucja_t** instytucja_tParam )
   {

      if ( instytucja_tParam == NULL )
       {
           PRINT_ERROR("Nieprawidłowy pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
           return BMD_ERR_PARAM1;
       }

      if (*instytucja_tParam)
       {
         clearId( &((*instytucja_tParam)->id) );
         free0( (*instytucja_tParam)->nazwa );
         clearAdres( &((*instytucja_tParam)->adres) );
       }

      return BMD_OK;

   }

   /**
   */

  long clearPodmiot( podmiot_t** podmiot_tParam )
   {

      if ( podmiot_tParam == NULL )
       {
           PRINT_ERROR("Nieprawidłowy pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
           return BMD_ERR_PARAM1;
       }

      if (*podmiot_tParam)
       {
         clearInstytucja( &((*podmiot_tParam)->instytucja) );
       }

      return BMD_OK;

   }

   /**
   */

  long clearOdbiorca( odbiorca_t** odbiorca_tParam )
   {

      if ( odbiorca_tParam == NULL )
       {
          PRINT_ERROR("Nieprawidłowy pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
          return BMD_ERR_PARAM1;
       }

      if (*odbiorca_tParam)
       {
         clearPodmiot( &((*odbiorca_tParam)->podmiot) );
         free0( (*odbiorca_tParam)->rodzaj );
       }

      return BMD_OK;
   }

   /**
   */

  long clearPrzadzialCzasu( przedzialCzasu_t** przedzialCzasu_tParam )
   {

      if ( przedzialCzasu_tParam == NULL )
       {
           PRINT_ERROR("Nieprawidłowy pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
           return BMD_ERR_PARAM1;
       }

       if (*przedzialCzasu_tParam)
        {
          free0( (*przedzialCzasu_tParam)->_od );
          free0( (*przedzialCzasu_tParam)->_do );
        }

      return BMD_OK;

   }

  /**
   */

  long clearData( data_t** data_tParam )
   {

      if ( data_tParam == NULL )
       {
          PRINT_ERROR("Nieprawidłowy pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
          return BMD_ERR_PARAM1;
       }

      if (*data_tParam)
       {
         free0( (*data_tParam)->typDaty );
         free0( (*data_tParam)->czas );
         clearPrzadzialCzasu( &((*data_tParam)->przedzialCzasu) );
       }

     return BMD_OK;

   }

  /**
  */

  long clearDostep( dostep_t** dostep_tParam )
   {

      if ( dostep_tParam == NULL )
       {
           PRINT_ERROR("Nieprawidłowy pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
           return BMD_ERR_PARAM1;
       }

       if (*dostep_tParam)
        {
          free0( (*dostep_tParam)->dostepnosc );
          free0( (*dostep_tParam)->uwagi );
          clearData( &((*dostep_tParam)->data) );
        }

      return BMD_OK;

   }

   /**
   */

  long clearFormat( format_t** format_tParam )
   {

      if ( format_tParam == NULL )
       {
           PRINT_ERROR("Nieprawidłowy pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
           return BMD_ERR_PARAM1;
       }

      if ( *format_tParam )
       {
         free0( (*format_tParam)->typ );
         free0( (*format_tParam)->specyfikacja );
         free0( (*format_tParam)->wielkosc );
       }

      return BMD_OK;
   }

   /**
   */

  long clearIdentyfikator( Identyfikator_t** Identyfikator_tParam )
   {

      if ( Identyfikator_tParam == NULL )
       {
           PRINT_ERROR("Nieprawidłowy pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
           return BMD_ERR_PARAM1;
       }

      if ( *Identyfikator_tParam )
       {
          free0( (*Identyfikator_tParam)->typ );
          free0( (*Identyfikator_tParam)->wartosc );
          clearPodmiot( &((*Identyfikator_tParam)->podmiot) );
       }

     return BMD_OK;

   }

  /**
   */

  long clearRelacja( relacja_t** relacja_tParam )
   {

      if ( relacja_tParam == NULL )
       {
          PRINT_ERROR("Nieprawidłowy pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
          return BMD_ERR_PARAM1;
       }

      if ( *relacja_tParam )
       {
          clearIdentyfikator( &((*relacja_tParam)->identyfikator) );
          free0( (*relacja_tParam)->typ );
       }

      return BMD_OK;

   }

   /**
   */

  long clearTworca( tworca_t** tworca_tParam )
   {

      if ( tworca_tParam == NULL )
       {
           PRINT_ERROR("Nieprawidłowy pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
           return BMD_ERR_PARAM1;
       }

      if ( *tworca_tParam )
       {
         free0( (*tworca_tParam)->funkcja );
         clearPodmiot( &((*tworca_tParam)->podmiot) );
       }

       return BMD_OK;

   }

   /**
   */

  long clearTyp( typ_t** typ_tParam )
   {

      if ( typ_tParam == NULL )
       {
           PRINT_ERROR("Nieprawidłowy pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
           return BMD_ERR_PARAM1;
       }

       if (*typ_tParam)
        {
           free0( (*typ_tParam)->kategoria );
           free0( (*typ_tParam)->rodzaj );
        }

       return BMD_OK;

   }

  /**
   */

  long clearTytulElement( tytulElement_t** tytulElement_tParam )
   {

      if ( tytulElement_tParam == NULL )
       {
          PRINT_ERROR("Nieprawidłowy pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
          return BMD_ERR_PARAM1;
       }

      if ( *tytulElement_tParam )
       {
          free0( (*tytulElement_tParam)->value );
          free0( (*tytulElement_tParam)->jezyk );
       }

      return BMD_OK;

   }

  long clearTytul( tytul_t** tytul_tParam )
   {

      if ( tytul_tParam == NULL )
       {
          PRINT_ERROR("Nieprawidłowy pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
          return BMD_ERR_PARAM1;
       }

      if (*tytul_tParam)
       {
         clearTytulElement( &((*tytul_tParam)->oryginalny) );
       }

       return BMD_OK;

   }

/**
*/

 long clearMetadata( metadata_t** metadata_tParam )
  {

      /* ----------- */
         long i = 0;
      /* ----------- */


      if ( metadata_tParam == NULL || *metadata_tParam == NULL )
       {
           PRINT_ERROR("Nieprawidłowy pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
           return BMD_ERR_PARAM1;
       }

      if ( (*metadata_tParam)->odbiorca != NULL )
       {
         for( i=0; ((*metadata_tParam)->odbiorca)[i] != NULL; i++)
          {
              clearOdbiorca( &(((*metadata_tParam)->odbiorca)[i]) );
          }
       }

      if ( (*metadata_tParam)->data != NULL )
       {
           for( i=0; ((*metadata_tParam)->data)[i] != NULL; i++)
           {
               clearData( &(((*metadata_tParam)->data)[i]) );
           }
       }

      clearDostep( &((*metadata_tParam)->dostep) );
      clearFormat( &((*metadata_tParam)->format) );

      if ( (*metadata_tParam)->identyfikator != NULL )
       {
           for( i=0; ((*metadata_tParam)->identyfikator)[i] != NULL; i++)
           {
               clearIdentyfikator( &(((*metadata_tParam)->identyfikator)[i]) );
           }
       }

      if ( (*metadata_tParam)->jezyk != NULL )
       {
          for( i=0; ((*metadata_tParam)->jezyk)[i] != NULL; i++)
           {
               clearJezyk( &(((*metadata_tParam)->jezyk)[i]) );
           }
       }

      free0( (*metadata_tParam)->uprawnienia );

      if ( (*metadata_tParam)->relacja != NULL )
       {
         for( i=0; ((*metadata_tParam)->relacja)[i] != NULL; i++)
          {
              clearRelacja( &(((*metadata_tParam)->relacja)[i]) );
          }
       }

      clearTworca( &((*metadata_tParam)->tworca) );
      clearTyp( &((*metadata_tParam)->typ) );
      clearTytul( &((*metadata_tParam)->tytul) );

      free0( (*metadata_tParam) );

     return BMD_OK;

  }



