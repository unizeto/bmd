<?
require_once('DAO/com_func.php');
draw_htmlhead('Informacje z przebiegu prac nad SOE SOAP WWW','');
?>
<div class="information w_lewo">
<code><br />
CHANGE LOG<br />

*************************************************<br /><br />
-------------------------------------------------<br />
2009-07-15<br /><br />
COMMENT=> Dołożono funckje dodawania dodatkowych podpisów do plików w bazie<br />
-------------------------------------------------<br />
2009-07-06<br /><br />
COMMENT=> Dodano obsługę dodawania, edycji grup. Na chwilę obecną portalik zawiera pełną obsługę użytkowników.<br />
COMMENT=> <b>Zrezygnowano z prowadzenia linii 1.7. Od dnia dzisiejszego portalik jest stricte do testów wersji trunk. Niektóre funkcje nadal działają z wersją 1.7 (do czasu ich modyfikacji przez developera bmd soap).</b> <br />
COMMENT=> Dodano automatyczne ładowanie serial i issuer użytkownika z podanego pliku der/cer.<br />
-------------------------------------------------<br />
2009-03-07<br /><br />
COMMENT=> Dodano panel administracyjny z obsługą dodawania edycji ról i użytkowników.<br />
-------------------------------------------------<br />
2009-02-24<br /><br />
TODO=> Dodać więcej linków "powrotnych" pod wynikami działania funkcji dla szybkiego wywołania ponownie funkcji.<br />
COMMENT=> Rozróżnienie funkcji pomiędzy wersjami (1.7.x vs trunk).<br />
-------------------------------------------------<br />
2009-01-14<br /><br />
TODO=> !!Należy usunąć przekazywanie wielkich tablic przez sesje. Zamiast tego co najwyżej parametry wywołania funkcji soap, i ją uruchamiać na danej podstronie!! W skrócie więcej małych SELECT'ów.<br />
-------------------------------------------------<br />
2009-01-13<br /><br />
COMMENT=> Dodano wyświetlanie metadanych po kliknięciu na pliku przy wyświetlaniu wyników w postaci drzewa. Narazie działa tylko dla plików. Dodanie do katalogów nie jest problemem.<br />
COMMENT=> Dodano możliwość wyboru wersji serwera SOAP przy logowaniu. Sprowadza się do wpisania nazwy produkcyjnej. Efektem jest włączanie/wyłączanie dostępu do najnowszych funkcji.<br />
COMMENT=> Drobne poprawki wizualne<br />
-------------------------------------------------<br />
2009-01-12<br /><br />
COMMENT=> Poprawiono blokowanie dostępu. Nadal jest to wersja "by lame". Do poprawienia w przyszłości na coś bardziej prof.<br />
COMMENT=> Dodano nazwy kolumn  w wynikach wyszukiwania.<br />
-------------------------------------------------<br />
2009-01-08<br /><br />
TODO=> Zastanowić się nad implementacją get/delete itp.  na wynikach wyszukiwania, 
np. jako linki lub rozwijane menu kontekstowe a najlepiej ograniczyć listę wyświetlanych kolumn,
 a po kliknięciu przejście do pełnych informacji + operacje na danym wyniku.<br />
COMMENT=> Nowy layout logowania.<br />
COMMENT=> Modyfikacja sposobu dodawania nagłówka dla stron HTML ( od teraz funkcja z parametrami ).<br />
-------------------------------------------------<br />
2009-01-06<br /><br />
COMMENT=> Zaimplementowano blokowanie dostępu do zawartości portalu w chwili braku logowania do usługi.<br />
COMMENT=> Idziemy w stronę funkcji.<br /> 
-------------------------------------------------<br />
2009-01-05<br /><br />
<del>TODO=> Poprawić zależność dostępu do funkcji od wyniku odpowiedzi przy logowaniu a nie od wpisania adresu w logowaniu!</del><br /> 
-------------------------------------------------<br />
2008-12-23<br /><br />
COMMENT=> Ujednolicono wykorzystanie formularzy.<br />
-------------------------------------------------<br />
2008-12-22<br /><br />
TODO=> UJEDNOLICIĆ ŚCIEŻKĘ SVN DO JEDNEJ INSTANCJI WWW SOAP. KONIEC Z ROZŁAMEM NA 1.7 I TRUNK => JEDEN KOD, ZALEŻNY JEDYNIE OD WSDL!<br />
<del>TODO=> Ujednolicić formularze dla InsertFile,CreateDir i UpdateMetadata.</del><br />
TODO=> Poprawić funkcje search tree - pozbyć się w miarę możliwości globali oraz dodać przekazywanie parametrów poprzez wywołanie funkcji.<br />
TODO=> Zrobić obsługę błędów.<br />
COMMENT=> Ujednolicono formularze dla "PLAY specific" oraz GET.<br />
-------------------------------------------------<br />
2008-12-19<br /><br />
TODO=> Ładowanie pliku wsdl z dysku (testowo przy logowaniu, domyślnie z poziomu konfiguracji administracyjnej)<br />
-------------------------------------------------<br />
2008-12-16<br /><br />
TODO=> Logowanie na wybrany z dysku certyfikat. Zostawione na później ze zwględu na brak takiej potrzeby w funkcjonalności.<br />
TODO=> Ze względu na rozrastanie się systemu, należy się zastanowić nad stworzeniem panelu administracyjnego z opcjami konfiguracyjnymi dla witryny.<br /> 
COMMENT=> Dodano "wybór" logowania z/bez certyfikatu. Na chwilę obecną wybór w postaci modyfikacji pliku certyfikat.php .<br />
COMMENT=> Formularze oparte o oidsDesc. Czyli mamy częściową konfigurowalność !<br />
-------------------------------------------------<br />
2008-12-15<br /><br />
<del>TODO=> Funkcje mogą być ujednolicone (pogrupowane) do jednego polecenia z radioboxem -> "get","search","PLAY specific".</del><br />
TODO=> Dla automatyzacji i uproszczenia integracji z innymi systemami przerobić wywołania na funkcje php. Przykład search w postaci drzewa.<br />
COMMENT=> Dodano opisy do wywołań (html title tags).<br />
COMMENT=> Dodano opcje logowania na serwer o podanym adresie.<br />
-------------------------------------------------<br />
2008-12-01<br /><br />
<del>TODO=> Tworzenie formularzy oprzeć o oidsDesc! </del><br />
<del>COMMENT=> Formularze generowane są na podstawie informacji zwrotnej z wywołania funkcji bmdlogin.<br />
Ze względu na chęć dodania kalendarzy i zakresów przy wyszukiwaniu (zależnie od typu danych) informacje o oidach i ich opisach są w pliku zewn (oidsDesc.php), minus takiego rozwiązania - zależnie od serwera trzeba będzie taki plik edytować ręcznie.</del><br />
</code>
</div>
<?
print("<br /><hr />");
require_once "tail.php";
?>