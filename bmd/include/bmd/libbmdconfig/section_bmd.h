/** @file
Ten plik zawiera konfigurację sekcji [bmd]. Jest to sekcja definiująca parametry połączenia 
z serwerem BMD. 

Pliki .h dla innych sekcji powinny się znajdować w src/, zaraz przy plikach z kodem 
korzystająch z nich aplikacji - chyba, że zakładamy możliwość ich użycia w innych 
miejscah.

*/
#ifndef __INCLUDE_CONF_SECTION_BMD 
#define __INCLUDE_CONF_SECTION_BMD 

static const char * BMDCONF_DICT_BMD[]=
{
        "host 0",
        "port 0",
        "%cert_file 0",
        "cert_label 0",
        "cert_pin 0",
        ""
};

#endif
