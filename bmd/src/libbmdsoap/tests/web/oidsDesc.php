<?
/*
Adekwatne do kontrolek
typ:
1 = string
2 = number
3 = data
4 = money

updt: czy jest metadaną update'owalną
0 - nie 
1 - tak

szukaj: czy ma być w formularzu wyszukiwania
0 - nie 
1 - tak

opcja: czy jest metadaną opcjonalną
0 - nie 
1 - tak

*/

$oidsDesc=array();
$oidsDesc['1.2.616.1.113527.4.3.2.5.2']=array("OID"=>"1.2.616.1.113527.4.3.2.5.2","opis"=>"Nazwa faktury", 
"typ"=>1, "updt"=>1, "pokaz"=>1, "szukaj"=>1, "opcja"=>1);
$oidsDesc['1.2.616.1.113527.4.3.2.5.1']=array("OID"=>"1.2.616.1.113527.4.3.2.5.1","opis"=>"Data wysłania",
"typ"=>3, "updt"=>0, "pokaz"=>1, "szukaj"=>1 , "opcja"=>1);//

$oidsDesc['1.2.616.1.113527.4.3.2.5.3']=array("OID"=>"1.2.616.1.113527.4.3.2.5.3","opis"=>"Rozmiar kryptogramu", 
"typ"=>2, "updt"=>0, "pokaz"=>0, "szukaj"=>0, "opcja"=>1);
$oidsDesc['1.2.616.1.113527.4.3.2.5.4']=array("OID"=>"1.2.616.1.113527.4.3.2.5.4","opis"=>"Visible", 
"typ"=>2, "updt"=>1, "pokaz"=>0, "szukaj"=>0, "opcja"=>1);
$oidsDesc['1.2.616.1.113527.4.3.2.5.5']=array("OID"=>"1.2.616.1.113527.4.3.2.5.5","opis"=>"Present", 
"typ"=>2, "updt"=>0, "pokaz"=>0, "szukaj"=>0, "opcja"=>1);
$oidsDesc['1.2.616.1.113527.4.3.2.5.6']=array("OID"=>"1.2.616.1.113527.4.3.2.5.6","opis"=>"File type", 
"typ"=>1, "updt"=>0, "pokaz"=>1, "szukaj"=>0, "opcja"=>1);//w innym miejscu forma search
$oidsDesc['1.2.616.1.113527.4.3.2.5.7']=array("OID"=>"1.2.616.1.113527.4.3.2.5.7","opis"=>"Coresponding id", 
"typ"=>1, "updt"=>1, "pokaz"=>1, "szukaj"=>0, "opcja"=>1);//w innym miejscu forma search
$oidsDesc['1.2.616.1.113527.4.3.2.5.8']=array("OID"=>"1.2.616.1.113527.4.3.2.5.8","opis"=>"Transaction id", 
"typ"=>1, "updt"=>0, "pokaz"=>0, "szukaj"=>0, "opcja"=>1);
$oidsDesc['1.2.616.1.113527.4.3.2.5.9']=array("OID"=>"1.2.616.1.113527.4.3.2.5.9","opis"=>"Pointing id (Link)", 
"typ"=>1, "updt"=>1, "pokaz"=>1, "szukaj"=>0, "opcja"=>0);
$oidsDesc['1.2.616.1.113527.4.3.2.5.10']=array("OID"=>"1.2.616.1.113527.4.3.2.5.10","opis"=>"Location id", 
"typ"=>1, "updt"=>1, "pokaz"=>1, "szukaj"=>1, "opcja"=>1);

$oidsDesc['1.2.616.1.113527.4.3.2.12.2']=array("OID"=>"1.2.616.1.113527.4.3.2.12.2","opis"=>"Wystawca certyfikatu użytkownika (Link)", 
"typ"=>1, "updt"=>1, "pokaz"=>1, "szukaj"=>0, "opcja"=>0);
$oidsDesc['1.2.616.1.113527.4.3.2.12.3']=array("OID"=>"1.2.616.1.113527.4.3.2.12.3","opis"=>"Numer seryjny certyfikatu użytkownika (Link)", 
"typ"=>1, "updt"=>1, "pokaz"=>1, "szukaj"=>0, "opcja"=>0);

$oidsDesc['1.3.6.1.5.5.7.10.4']=array("OID"=>"1.3.6.1.5.5.7.10.4","opis"=>"Grupa użytkownika", 
"typ"=>1, "updt"=>0, "pokaz"=>1, "szukaj"=>0, "opcja"=>1);
$oidsDesc['1.2.616.1.113527.4.3.2.1']=array("OID"=>"1.2.616.1.113527.4.3.2.1","opis"=>"Kategoria bezpieczeństwa", 
"typ"=>1, "updt"=>0, "pokaz"=>1, "szukaj"=>0, "opcja"=>1);
$oidsDesc['2.5.1.5.55']=array("OID"=>"2.5.1.5.55","opis"=>"Poziom bezpieczeństwa", 
"typ"=>1, "updt"=>0, "pokaz"=>0, "szukaj"=>0, "opcja"=>1);
$oidsDesc['1.2.616.1.113527.4.3.1.5']=array("OID"=>"1.2.616.1.113527.4.3.1.5","opis"=>"Typ pliku", 
"typ"=>1, "updt"=>0, "pokaz"=>0, "szukaj"=>0, "opcja"=>1);

$oidsDesc['1.2.616.1.113527.4.3.4.0']=array("OID"=>"1.2.616.1.113527.4.3.4.0","opis"=>"undefined", 
"typ"=>1, "updt"=>0, "pokaz"=>0, "szukaj"=>0, "opcja"=>1);

$oidsDesc['1.2.616.1.113527.4.3.3.1']=array("OID"=>"1.2.616.1.113527.4.3.3.1","opis"=>"Numer faktury", 
"typ"=>1, "updt"=>1, "pokaz"=>1, "szukaj"=>1, "opcja"=>0);//
$oidsDesc['1.2.616.1.113527.4.3.3.2']=array("OID"=>"1.2.616.1.113527.4.3.3.2","opis"=>"NIP wystawcy", 
"typ"=>1, "updt"=>1, "pokaz"=>1, "szukaj"=>1, "opcja"=>0);//
$oidsDesc['1.2.616.1.113527.4.3.3.3']=array("OID"=>"1.2.616.1.113527.4.3.3.3","opis"=>"Nazwa wystawcy", 
"typ"=>1, "updt"=>1, "pokaz"=>1, "szukaj"=>1, "opcja"=>0);//
$oidsDesc['1.2.616.1.113527.4.3.3.4']=array("OID"=>"1.2.616.1.113527.4.3.3.4","opis"=>"NIP odbiorcy", 
"typ"=>1, "updt"=>1, "pokaz"=>1, "szukaj"=>1, "opcja"=>0);//
$oidsDesc['1.2.616.1.113527.4.3.3.5']=array("OID"=>"1.2.616.1.113527.4.3.3.5","opis"=>"Nazwa odbiorcy", 
"typ"=>1, "updt"=>1, "pokaz"=>1, "szukaj"=>1, "opcja"=>0);//
$oidsDesc['1.2.616.1.113527.4.3.3.6']=array("OID"=>"1.2.616.1.113527.4.3.3.6","opis"=>"Data wystawienia", 
"typ"=>3, "updt"=>1, "pokaz"=>1, "szukaj"=>1, "opcja"=>0);//
$oidsDesc['1.2.616.1.113527.4.3.3.7']=array("OID"=>"1.2.616.1.113527.4.3.3.7","opis"=>"Data płatności", 
"typ"=>3, "updt"=>1, "pokaz"=>1, "szukaj"=>1, "opcja"=>0);//
$oidsDesc['1.2.616.1.113527.4.3.3.8']=array("OID"=>"1.2.616.1.113527.4.3.3.8","opis"=>"E-mail odbiorcy", 
"typ"=>1, "updt"=>1, "pokaz"=>1, "szukaj"=>1, "opcja"=>0);//
$oidsDesc['1.2.616.1.113527.4.3.3.9']=array("OID"=>"1.2.616.1.113527.4.3.3.9","opis"=>"E-mail nadawcy", 
"typ"=>1, "updt"=>1, "pokaz"=>1, "szukaj"=>1, "opcja"=>0);//

$oidsDesc['1.2.616.1.113527.4.3.3.10']=array("OID"=>"1.2.616.1.113527.4.3.3.10","opis"=>"Zgoda odbiorcy", 
"typ"=>1, "updt"=>1, "pokaz"=>0, "szukaj"=>0, "opcja"=>1);

$oidsDesc['1.2.616.1.113527.4.3.3.11']=array("OID"=>"1.2.616.1.113527.4.3.3.11","opis"=>"Kwota netto", 
"typ"=>4, "updt"=>1, "pokaz"=>1, "szukaj"=>1, "opcja"=>0);//
$oidsDesc['1.2.616.1.113527.4.3.3.12']=array("OID"=>"1.2.616.1.113527.4.3.3.12","opis"=>"Kwota brutto", 
"typ"=>4, "updt"=>1, "pokaz"=>1, "szukaj"=>1, "opcja"=>0);//
$oidsDesc['1.2.616.1.113527.4.3.3.13']=array("OID"=>"1.2.616.1.113527.4.3.3.13","opis"=>"Typ faktury", 
"typ"=>1, "updt"=>1, "pokaz"=>1, "szukaj"=>1, "opcja"=>1);//
$oidsDesc['1.2.616.1.113527.4.3.3.14']=array("OID"=>"1.2.616.1.113527.4.3.3.14","opis"=>"Do data", 
"typ"=>1, "updt"=>1, "pokaz"=>1, "szukaj"=>1, "opcja"=>1);//
$oidsDesc['1.2.616.1.113527.4.3.3.15']=array("OID"=>"1.2.616.1.113527.4.3.3.15","opis"=>"Do numer", 
"typ"=>3, "updt"=>1, "pokaz"=>1, "szukaj"=>1, "opcja"=>1);//
$oidsDesc['1.2.616.1.113527.4.3.3.16']=array("OID"=>"1.2.616.1.113527.4.3.3.16","opis"=>"Data sprzedazy", 
"typ"=>3, "updt"=>1, "pokaz"=>1, "szukaj"=>1, "opcja"=>0);//
$oidsDesc['1.2.616.1.113527.4.3.3.17']=array("OID"=>"1.2.616.1.113527.4.3.3.17","opis"=>"Id klienta", 
"typ"=>1, "updt"=>0, "pokaz"=>1, "szukaj"=>1, "opcja"=>1);
$oidsDesc['1.2.616.1.113527.4.3.3.21']=array("OID"=>"1.2.616.1.113527.4.3.3.21","opis"=>"Wysłano awizo", 
"typ"=>1, "updt"=>0, "pokaz"=>1, "szukaj"=>0, "opcja"=>1);
$oidsDesc['1.2.616.1.113527.4.3.3.22']=array("OID"=>"1.2.616.1.113527.4.3.3.22","opis"=>"Odebrano fakturę", 
"typ"=>1, "updt"=>0, "pokaz"=>1, "szukaj"=>0, "opcja"=>1);
$oidsDesc['1.2.616.1.113527.4.3.3.23']=array("OID"=>"1.2.616.1.113527.4.3.3.23","opis"=>"Skrot pliku", 
"typ"=>1, "updt"=>0, "pokaz"=>1, "szukaj"=>1, "opcja"=>1);//
/////////////////////////1.7///////////////////////
$oidsDesc['1.2.616.1.113527.4.3.3.24']=array("OID"=>"1.2.616.1.113527.4.3.3.24","opis"=>"Visible", 
"typ"=>2, "updt"=>1, "pokaz"=>0, "szukaj"=>0, "opcja"=>1);//wersja 1.7
$oidsDesc['1.2.616.1.113527.4.3.3.25']=array("OID"=>"1.2.616.1.113527.4.3.3.25","opis"=>"File type", 
"typ"=>1, "updt"=>0, "pokaz"=>1, "szukaj"=>0, "opcja"=>1);//w innym miejscu forma search
$oidsDesc['1.2.616.1.113527.4.3.3.26']=array("OID"=>"1.2.616.1.113527.4.3.3.26","opis"=>"Coresponding id", 
"typ"=>1, "updt"=>1, "pokaz"=>1, "szukaj"=>0, "opcja"=>1);//w innym miejscu forma search, wersja 1.7
/////////////////////////////////////////////////////////////////
$oidsDesc['1.2.616.1.113527.4.3.3.27']=array("OID"=>"1.2.616.1.113527.4.3.3.27","opis"=>"Awizo mail template", 
"typ"=>1, "updt"=>1, "pokaz"=>1, "szukaj"=>0, "opcja"=>1);
$oidsDesc['1.2.616.1.113527.4.3.3.28']=array("OID"=>"1.2.616.1.113527.4.3.3.28","opis"=>"Transaction id", 
"typ"=>1, "updt"=>0, "pokaz"=>0, "szukaj"=>0, "opcja"=>1);

$oidsDesc['1.2.616.1.113527.4.3.3.55']=array("OID"=>"1.2.616.1.113527.4.3.3.55","opis"=>"Pierwsze wysłanie awiza", 
"typ"=>1, "updt"=>0, "pokaz"=>1, "szukaj"=>0, "opcja"=>1);
$oidsDesc['1.2.616.1.113527.4.3.3.56']=array("OID"=>"1.2.616.1.113527.4.3.3.56","opis"=>"Pierwsze pobranie faktury", 
"typ"=>1, "updt"=>0, "pokaz"=>1, "szukaj"=>0, "opcja"=>1);
$oidsDesc['1.2.616.1.113527.4.3.3.57']=array("OID"=>"1.2.616.1.113527.4.3.3.57","opis"=>"Present", 
"typ"=>2, "updt"=>0, "pokaz"=>0, "szukaj"=>0, "opcja"=>1);
$oidsDesc['1.2.616.1.113527.4.3.3.58']=array("OID"=>"1.2.616.1.113527.4.3.3.58","opis"=>"Visualisation", 
"typ"=>1, "updt"=>1, "pokaz"=>1, "szukaj"=>0, "opcja"=>1);
//print_r($oidsDesc[0]['OID'], "pokaz"=>1, "szukaj"=>1, "opcja"=>0);

$actionsDesc=array();
 
$actionsDesc['1']="BMD_DATAGRAM_TYPE_GET_FORM";
$actionsDesc['2']="BMD_DATAGRAM_TYPE_INSERT";
$actionsDesc['3']="BMD_DATAGRAM_TYPE_INSERT_LO";
$actionsDesc['4']="BMD_DATAGRAM_TYPE_GET";
$actionsDesc['5']="BMD_DATAGRAM_TYPE_GET_LO";
$actionsDesc['6']="BMD_DATAGRAM_TYPE_GET_PKI";
$actionsDesc['7']="BMD_DATAGRAM_TYPE_GET_PKI_LO";
$actionsDesc['8']="BMD_DATAGRAM_TYPE_DELETE";
$actionsDesc['9']="BMD_DATAGRAM_TYPE_PERSISTENT";
$actionsDesc['10']="BMD_DATAGRAM_GET_CGI";
$actionsDesc['11']="BMD_DATAGRAM_GET_CGI_PKI";
$actionsDesc['12']="BMD_DATAGRAM_GET_CGI_LO";
$actionsDesc['13']="BMD_DATAGRAM_GET_CGI_PKI_LO";
$actionsDesc['14']="BMD_DATAGRAM_GET_METADATA";
$actionsDesc['15']="BMD_DATAGRAM_GET_METADATA_WITH_TIME";
$actionsDesc['16']="BMD_DATAGRAM_GET_ONLY_FILE";
$actionsDesc['17']="BMD_DATAGRAM_TYPE_NOOP";
$actionsDesc['18']="BMD_DATAGRAM_TYPE_SEND_AVIZO";
$actionsDesc['19']="BMD_DATAGRAM_TYPE_VALIDATE_CERTIFICATE";
$actionsDesc['20']="BMD_DATAGRAM_TYPE_VALIDATE_SIGNATURE";
$actionsDesc['21']="BMD_DATAGRAM_TYPE_UPDATE";
$actionsDesc['22']="BMD_DATAGRAM_TYPE_CLOSE_SESSION";
$actionsDesc['23']="BMD_DATAGRAM_TYPE_LOGIN_AS_OTHER";
$actionsDesc['24']="BMD_DATAGRAM_TYPE_LOGIN_FOREVER";
$actionsDesc['25']="BMD_DATAGRAM_TYPE_CERTIFY_DVCS";
$actionsDesc['26']="BMD_DATAGRAM_TYPE_CERTIFY_TIMESTAMP";
$actionsDesc['27']="BMD_DATAGRAM_TYPE_CERTIFY_DOCUMENT";
$actionsDesc['28']="BMD_DATAGRAM_TYPE_VERIFY_CRYPTO_SIGNATURE";
$actionsDesc['29']="BMD_DATAGRAM_TYPE_END_TRANSACTION";
$actionsDesc['30']="BMD_DATAGRAM_TYPE_COUNT_DOCS";
$actionsDesc['31']="BMD_DATAGRAM_GET_METADATA_ADD";
$actionsDesc['32']="BMD_DATAGRAM_DIR_CREATE";
$actionsDesc['33']="BMD_DATAGRAM_DIR_LIST";
$actionsDesc['34']="BMD_DATAGRAM_DIR_LIST_WITH_OR";
$actionsDesc['35']="BMD_DATAGRAM_DIR_DELETE";
$actionsDesc['36']="BMD_DATAGRAM_DIR_GET";
$actionsDesc['37']="BMD_DATAGRAM_DIR_GET_DETAIL";
$actionsDesc['38']="BMD_DATAGRAM_TYPE_SEARCH";
$actionsDesc['39']="BMD_DATAGRAM_TYPE_REGISTER_USER";
$actionsDesc['40']="BMD_DATAGRAM_TYPE_UPDATE_USER";
$actionsDesc['41']="BMD_DATAGRAM_TYPE_GET_USER_LIST";
$actionsDesc['42']="BMD_DATAGRAM_TYPE_DISABLE_USER";
$actionsDesc['43']="BMD_DATAGRAM_TYPE_DELETE_USER";
$actionsDesc['44']="BMD_DATAGRAM_TYPE_REGISTER_ROLE";
$actionsDesc['45']="BMD_DATAGRAM_TYPE_UPDATE_ROLE";
$actionsDesc['46']="BMD_DATAGRAM_TYPE_GET_ROLE_LIST";
$actionsDesc['47']="BMD_DATAGRAM_TYPE_DELETE_ROLE";
$actionsDesc['48']="BMD_DATAGRAM_TYPE_CHOOSE_ROLE";
$actionsDesc['49']="BMD_DATAGRAM_LINK_CREATE";
$actionsDesc['50']="BMD_DATAGRAM_ADD_SIGNATURE";
$actionsDesc['51']="BMD_DATAGRAM_TYPE_REGISTER_GROUP";
$actionsDesc['52']="BMD_DATAGRAM_TYPE_UPDATE_GROUP";
$actionsDesc['53']="BMD_DATAGRAM_TYPE_GET_GROUP_LIST";
$actionsDesc['54']="BMD_DATAGRAM_TYPE_DELETE_GROUP";
$actionsDesc['55']="BMD_DATAGRAM_TYPE_REGISTER_SECURITY";
$actionsDesc['56']="BMD_DATAGRAM_TYPE_UPDATE_SECURITY";
$actionsDesc['57']="BMD_DATAGRAM_TYPE_GET_SECURITY_LIST";
$actionsDesc['58']="BMD_DATAGRAM_TYPE_DELETE_SECURITY";

function type_test($baza,$testoid)
{
$wynik=0;
	if (isset($baza['OID'])) { $wynik=$baza['OID']['typ'];}
	
return $wynik;
}
?>