/* przyjmujemy zasade uzywania tego samego numeru wersji wszystkich elementow systemu (serwer i klient wraz z pluginami) jezeli komplet wspolpracuje ze soba nawet jezeli zmienila sie tylko czesc systemu (klient, serwer, plugin, ...)*/
/* zmiany w numeracji wersji stosujemy zgodnie z dokumentem /bmd/common/dokumentacja/bmd/projektowa/BAE_architektura/Wersjonowanie_protokolu_BMD.odt */
/* zmiany opisujemy w pliku /bmd/bmd/trunk/BMDprotocolLog/BMDprotocol_version.txt */
/* uwagi i problemy trudne do przeskoczenia opisujemy w pliku /bmd/bmd/trunk/BMDprotocolLog/uwagi_dokumentacyjne.txt */

#define BMD_PROTOCOL_VERSION	2030007							/* wersja protokou komunikacyjnego BMD (wspolna dla klienta i serwera) 1.03.00.00*/
#define FILEVER					2,03,00,07						/* definicja wersji uzywana przy tworzeniu pliku wykonywalnego klienta i soe_gui.dll*/
#define ABTVRNR					"2.3.00.07"						/* numer wyswietlany w okienku informacji o pluginie - oraz zapisany w klient.conf*/ 
#define STRFILEVER				"2.3.00.07 (2013.02.21)\0"		/* wazne zeby numeracja byla oddzielona znakiem "." (kropki) - istotne dla weryfikacji wersji pluginu */
																/* weryfikacja pluginu w kliencie nie sprawdza ostatnich dwoch znakow numeru wersji. Podobnie jak w numerowaniu protokou s one uznane za nieistotne dla kompatybilnoci */
#define PRODUCTVER				2,3								/* definicja wersji uzywana przy tworzeniu pliku wykonywalnego klienta i soe_gui.dll*/
#define STRPRODUCTVER			"2.3\0"							/* wartosc stringowa pola wersja produktu w zakladce wersja pliku klient.exe */
//#define RELDATE			"2009.01.12"
