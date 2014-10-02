/*******************************
 *                             *
 * Data : 15-04-2009           *
 * Autor : Tomasz Klimek       *
 *                             *
 *******************************/

#ifndef _GEN_METADATA_
#define _GEN_METADATA_


#include <bmd/libbmdarchpack/config.h>
#include <bmd/common/bmd-sds.h>


/* -------- RODZAJ ODBIORCY --------- */
#define APM_GLOWNY "glowny"
#define APM_DO_WIADOMOSCI "do wiadomosci"

/* -------- TYP DATY --------------- */
#define APM_DOSTEPNY_PO "dostepnyPo"
#define APM_OPUBLIKOWANY "opublikowany"
#define APM_STWORZONY "stworzony"
#define APM_UZYSKANY "uzyskany"
#define APM_OTRZYMANY "otrzymany"
#define APM_WYSLANY "wyslany"
#define APM_ZAAKCEPTOWANY "zaakceptowany"
#define APM_ZATWIERDZONY "zatwierdzony"
#define APM_ZMODYFIKOWANY "zmodyfikowany"
#define APM_DATYSKRAJNE "daty skrajne"

/* --------- RODZAJ DOSTĘPU -------- */
#define APM_WSZYSTKO "wszystko"
#define APM_METADANE "metadane"
#define APM_NIEDOSTEPNE "niedostępne"


#define APM_BAJT "bajt"
#define APM_JEZYK1 "polski"


/* ---------- RODZAJ RELACJI -------- */
#define APM_maFormat "maFormat"
#define APM_jestFormatem "jestFormatem"
#define APM_maWersje "maWersje"
#define APM_jestWersja "jestWersja"
#define APM_jestCzescia "jestCzescia"
#define APM_maOdniesienie "maOdniesienie"
#define APM_odnosiSieDo "odnosiSieDo"
#define APM_maZrodlo "maZrodlo"
#define APM_wymaga "wymaga"
#define APM_jestWymagany "jestWymagany"
#define APM_maDekretacje "maDekretacje"
#define APM_jestDekretacja "jestDekretacja"

/* --------- KATEGORIA TWORCY -------- */
#define APM_stworzyl "stworzyl"
#define APM_modyfikowal "modyfikowal"
#define APM_zatwierdzil "zatwierdzil"


/* ------------ TYP ---------------- */
#define APM_Collection "Collection"
#define APM_Dataset "Dataset"
#define APM_Event "Event"
#define APM_Image "Image"
#define APM_InteractiveResource "InteractiveResource"
#define APM_MovingImage "MovingImage"
#define APM_PhysicalObject "PhysicalObject"
#define APM_Service "Service"
#define APM_Software "Software"
#define APM_Sound "Sound"
#define APM_StillImage "StillImage"
#define APM_Text "Text"


/* RODZAJ KONTAKTU */
#define APM_Email "email"
#define APM_Telefon "telefon"
#define APM_Telkom "telkom"
#define APM_Faks "faks"

/* KOD JEZYKA */
#define APM_POL "pol"


typedef enum { osoba, instytucja } enumPodmiot_t;
typedef enum { email, telefon, telkom, faks } enumKontakt_t;
typedef enum { glowny, do_wiadomosci } enumRodzaj_t;
typedef enum { dostepnyPo, opublikowany, stworzony, uzyskany, \
               otrzymany, wyslany, zaakceptowany, zatwierdzony, \
               zmodyfikowany, datySkrajne 
} enumTypDaty_t;


typedef enum { wszystko, metadane, niedostepne } enumDostepnosc_t;
typedef enum { pol } enumJezyk_t;
typedef enum {
                maFormat, jestFormatem, maWersje, jestWersja, \
                jestCzescia, maOdniesienie, odnosiSieDo, \
                maZrodlo, wymaga, jestWymagany, maDekretacje, \
                jestDekretacja
 }enumTypRelacji_t;

 typedef enum { stworzyl, modyfikowal, zatwierdzil } enumTworca_t;
 typedef enum { collection, dataset, event, image, interactiveResource, \
                movingImage, physicalObject, Service, software, sound, \
                stillImage, text
 }enumType_t; 


typedef struct
 {
    /* ---------------------- */

       char*             typ;    /* wymagany */
       char*           value;    /* opcjonalny */

    /* ---------------------- */

 }kontakt_t;


typedef struct
 {
    /* ---------------------- */

       char*             kod;    /* wymagany */
       char*           value;    /* wymagany */

    /* ---------------------- */

 }jezyk_t;

typedef struct
 {
     /* ----------------------- */

        char*              kod;    /* nie wymagane */
        char*           poczta;    /* nie wymagane */
        char*      miejscowosc;    /* wymagane */
        char*            ulica;    /* nie wymagane */
        char*          budynek;    /* nie wymagane */
        char*            lokal;    /* nie wymagane */
        char*  skrytkapocztowa;    /* nie wymagane */
        char*            uwagi;    /* nie wymagane */
        char*             kraj;    /* wymagane */

     /* ----------------------- */

 }adres_t;


typedef struct
 {
   /* -------------- */

      char*   value;
      char*    type;  /* atrybut wymagany */

   /* -------------- */
 }Id_t;


typedef struct
 {
   /* ---------------------- */

      Id_t*              id;    /* opcjonalne */
      char*        nazwisko;    /* wymagane */
      char*            imie;    /* opcjonalne */
      kontakt_t**   kontakt;    /* opcjonalne */
      char*         funkcja;    /* opcjonalne */

   /* ---------------------- */

 }pracownik_t;


typedef struct jednostka
 {
   /* ----------------------------- */

      char*                  nazwa;   /* wymagane */
      adres_t*               adres;   /* opcjonalne */
      kontakt_t**          kontakt;   /* opcjonalne */
      char*                 poziom;   /* opcjonalne */
      struct jednostka*  jednostka;   /* opcjonalne */
      pracownik_t*       pracownik;   /* opcjonalne */

   /* ----------------------------- */

 }jednostka_t;


typedef struct
 {
    /* ----------------------- */

       Id_t*               id;  /* opcjonalne */
       char*            nazwa;  /* wymagane */
       adres_t*         adres;  /* opcjonalne */
       kontakt_t**    kontakt;  /* opcjonalne */
       jednostka_t* jednostka;  /* opcjonalne */

    /* ----------------------- */

 }instytucja_t;


typedef struct
 {
   /* --------------------- */

      Id_t**            id;
      char*       nazwisko;  /* wymagane */
      char*           imie;  /* opcjonalne */
      adres_t*       adres;  /* opcjonalne */
      kontakt_t**  kontakt;  /* opcjonalne */

   /* --------------------- */

 }osoba_t;


typedef struct
 {
    /* -------------------------- */

       osoba_t*            osoba;   /* Pole wyboru albo osoba albo instytucja */
       instytucja_t*  instytucja;

    /* -------------------------- */

 }podmiot_t;

typedef struct
 {
   /* ---------------------- */

      podmiot_t*    podmiot;  /* wymagane */
      char*          rodzaj;  /* wymagane */

   /* ---------------------- */

 }odbiorca_t;


typedef struct
 {
   /* ------------- */

      char*    _od;   /* wymagane */
      char*    _do;   /* wymagane */

   /* ------------- */

 }przedzialCzasu_t;


 
 typedef struct
  {
     /* ---------------------------------- */

        char*                     typDaty;  /* wymagane */
        char*                        czas;  /* wymagane albo czas albo przedział czasu */
        przedzialCzasu_t*  przedzialCzasu;

     /* ---------------------------------- */


  }data_t;


 typedef struct
  {
    /* ----------------------------- */

       char*             dostepnosc;  /* wymagane */
       char*                  uwagi;  /* opcjonalne */
       data_t*                 data;  /* opcjonalne */

    /* ----------------------------- */

  }dostep_t;


typedef struct
 {
    /* -------------------- */

       char*           typ; /* wymagane */
       char*  specyfikacja; /* opcjonalne */
       char*      wielkosc; /* opcjonalne wyrażone w bajtach */

    /* -------------------- */

 }format_t;

typedef struct
 {
     /* ------------- */

        char*    typ; /* wymagane */
        char*    kod; /* wymagane */
        char*   opis; /* wymagane */

    /* ------------- */
 }grupowanie_t;

typedef struct
 {
   /* --------------------- */

      char*            typ;  /* wymagane */
      char*        wartosc;  /* wymagane */
      podmiot_t*   podmiot;  /* opcjonalne */

   /* --------------------- */

 }Identyfikator_t;


typedef struct
 {
    /* -------------------- */

       char*           typ;  /* wymagane */
       podmiot_t*  podmiot;  /* opcjonalne */
       char*         adres;  /* opcjonalne */

    /* -------------------- */

 }lokalizacja_t;


typedef struct
 {
     /* ---------------------- */

        char*       kategoria;  /* wymagane */
        char*            data;  /* wymagane */
        podmiot_t*    podmiot;  /* wymagane */

     /* ---------------------- */

 }kwalifikacja_t;


typedef struct
 {
    /* -------------------------------- */

       Identyfikator_t*  identyfikator; /* wymagane */
       char*                       typ; /* wymagane */

    /* -------------------------------- */

 }relacja_t;

typedef struct
 {
    /* ----------------- */

       char*     rodzaj;  /* wymagane */
       char*     wersja;  /* wymagane */
       char*       opis;  /* opcjonalne */

    /* ----------------- */


 }status_t;


typedef struct
 {

     /* ---------------- */

        char*     klucz;  /* wymagane */
        char*   wartosc;  /* wymagane */

     /* ---------------- */

 }inne_t;


 
typedef struct
 {
    /* ----------------------------- */

       char**             przedmiot; /* opcjonalne */
       char**                 osoby; /* opcjonalne */
       char**               miejsce; /* opcjonalne */
       przedzialCzasu_t*       czas; /* opcjonalne */
       char**              odbiorcy; /* opcjonalne */
       inne_t**                inne; /* opcjonalne */

    /* ----------------------------- */

 }tematyka_t;


typedef struct
 {

    /* ---------------------- */

       char*         funkcja; /* wymagane */
       podmiot_t*    podmiot; /* wymagane */

    /* ---------------------- */

 }tworca_t;


typedef struct
 {
     /* ------------------- */

        char*    kategoria;  /* wymagane */
        char*       rodzaj;  /* opcjonalne */

     /* ------------------- */

 }typ_t;


typedef struct
 {
     /* ---------------- */

        char*     value; /* wymagany */
        char*     jezyk; /* opcjonalny */

     /* ---------------- */

 }tytulElement_t;

 typedef struct
  {
      /* ------------------------------- */

         tytulElement_t*     oryginalny;  /* wymagane */
         tytulElement_t*   alternatywny;  /* opcjonalne */

      /* ------------------------------- */

  }tytul_t;




typedef struct
 {
    /* ---------------------------------- */

       odbiorca_t**             odbiorca;  /* opcjonalne */
       data_t**                     data;  /* wymagane   */
       dostep_t*                  dostep;  /* wymagane   */
       format_t*                  format;  /* wymagane   */
       grupowanie_t**         grupowanie;  /* opcjonalne */
       Identyfikator_t**   identyfikator;  /* wymagane   */
       jezyk_t**                   jezyk;  /* opcjonalne */
       lokalizacja_t**       lokalizacja;  /* opcjonalne */
       char*                        opis;  /* opcjonalne */
       char*                 uprawnienia;  /* opcjonalne */
       kwalifikacja_t*      kwalifikacja;  /* opcjonalne */
       relacja_t**               relacja;  /* opcjonalne */
       status_t*                  status;  /* opcjonalne */
       tematyka_t*              tematyka;  /* opcjonalne */
       tworca_t*                  tworca;  /* wymagane */
       typ_t*                        typ;  /* wymagane */
       tytul_t*                    tytul;  /* wymagane */

    /* ---------------------------------- */

 }metadata_t;


 /* ------------------------- Zainicjowanie struktury metadanych --------------------------- */

    long initMetadata( metadata_t** const );

 /* ----------------------------------------------------------------------------------------- */


 /* ------------------------------- Generowanie pól metadanych ------------------------------ */

    long genId_t( Id_t** const, char* const, char* const );
    long genInstytucja( instytucja_t** const, Id_t* const, char* const, \
                        adres_t* const, kontakt_t** const, jednostka_t* const );

    long genAdres( adres_t** const, char* const, char* const, char* const, \
                   char* const, char* const, char* const, char* const, char* const, char* const );
    long genKontakt( kontakt_t** const, enumKontakt_t, char* const );

    /* nie zaimplementowane, do pozniejszej realizacji */
    long addKontakty( kontakt_t*** const, kontakt_t* const );

    /* nie zaimplementowane, do pozniejszej realizacji */
    long genJednostka( jednostka_t** const, char* const, adres_t* const, kontakt_t** const, \
                       char* const, struct jednostka* const, pracownik_t* const );

    long genPodmiot( podmiot_t** const, osoba_t* const, instytucja_t* const );
    long genOdbiorca( odbiorca_t** const, podmiot_t* const, char* const );
    long genPrzedzialCzasu( przedzialCzasu_t** const, char* const, char* const );
    long genData( data_t** const, enumTypDaty_t, char* const, przedzialCzasu_t* const );
    long genDostep( dostep_t** const, enumDostepnosc_t, char* const, data_t* const );
    long genFormat( format_t** const, char* const, char* const, char* const );

    /* nie zaimplementowane, do pozniejszej realizacji */
    long genGrupowanie( grupowanie_t** const, char* const, char* const, char* const );
    long genIdentyfikator( Identyfikator_t** const, char* const, char* const, podmiot_t* const );
    long genJezyk( jezyk_t** const, enumJezyk_t, char* const );
    long genLokalizacja( lokalizacja_t** const, char* const, char* const, podmiot_t* const );

    /* nie zaimplementowane, do pozniejszej realizacji */
    long genKwalifikacja( kwalifikacja_t** const, char* const, char* const, podmiot_t* const );
    long genRelacja( relacja_t** const, Identyfikator_t* const, enumTypRelacji_t );

    /* nie zaimplementowane, do pozniejszej realizacji */
    long genStatus( status_t** const, char* const, char* const, char* const );

    /* nie zaimplementowane, do pozniejszej realizacji */
    long genTematyka( tematyka_t** const /* , other params */ );
    long genTworca( tworca_t** const, enumTworca_t, podmiot_t* const );
    long genTyp( typ_t** const, enumType_t, char* const );
    long genTytulElement( tytulElement_t** const, char* const, char* const );
    long genTytulOryginalny( tytul_t** const, tytulElement_t* const );

 /* ---------------------------------------------------------------------------------------------- */

 /* --------------------------- Ustawianie pól metadanych ---------------------------------------- */

    long setOdbiorca( metadata_t* const, odbiorca_t* const );
    long setData( metadata_t* const, data_t* const );
    long setDostep( metadata_t* const, dostep_t* const );
    long setFormat( metadata_t* const, format_t* const );
    long setIdentyfikator( metadata_t* const, Identyfikator_t* const );
    long setJezyk( metadata_t* const, jezyk_t* const );
    long setOpis( metadata_t* const, char* const );
    long setUprawnienia( metadata_t* const, char* const );
    long setRelacja( metadata_t* const, relacja_t* const );
    long setTworca( metadata_t* const, tworca_t* const );
    long setTyp( metadata_t* const, typ_t* const );
    long setTytul( metadata_t* const, tytul_t* const );

 /* ----------------------------------------------------------------------------------------------- */

 /* --------------------------- Generowanie metadanych z archiwum --------------------------------- */

    long composeMetadataStruct( metadata_t** const,Config_t* const, bmdDatagram_t* const );
    long composeXMLDocument( metadata_t* const, char* const );

 /* ----------------------------------------------------------------------------------------------- */

    long clearKontakt( kontakt_t** );
    long clearJezyk( jezyk_t** );
    long clearAdres( adres_t** );
    long clearId( Id_t** );
    long clearInstytucja( instytucja_t** );
    long clearPodmiot( podmiot_t** );
    long clearOdbiorca( odbiorca_t** );
    long clearPrzadzialCzasu( przedzialCzasu_t** );
    long clearData( data_t** );
    long clearDostep( dostep_t** );
    long clearFormat( format_t** );
    long clearIdentyfikator( Identyfikator_t** );
    long clearRelacja( relacja_t** );
    long clearTworca( tworca_t** );
    long clearTyp( typ_t** );
    long clearTytulElement( tytulElement_t** );
    long clearTytul( tytul_t** );
    long clearMetadata( metadata_t** );

#endif

