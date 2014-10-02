<? 
require_once('DAO/com_func.php');
draw_htmlhead('Logowanie do portalu SOE SOAP WWW','');
?>
<div class="logowanie_kontener">
<div class="szerokosc ramka logowanie w_prawo ">

<form action="bmdlogin.php" method="post" enctype="multipart/form-data" name="insertform">

<table class="login_tab">
<tbody ><tr><td ><a title="Plik certyfikatu logującego klienta">Cert</a></td><td><input type="file" name="certyfikat" id="certyfikat" /></td></tr>
<tr ><td><a title="Podaj adres serwera soap - Wpis w postaci: http[s]://address:port/[path] -> wpisy [] opcjonalne">Adres</a></td><td>
<input type="text" name="SOAPaddress" class="login_adressform" /></td></tr>
<tr>
<td><a title="Można podać opis/numer wersji produkcyjnej">Wersja</a></td><td><input type="text" name="SOAPver" value="trunk" readonly="readonly" />
<a title="Wywołanie zaloguje na zadany serwer i pobierze zestaw metadanych.">Login</a><input type="radio" name="testcon" value="login" checked="checked" />
<a title="Test connection - Sprawdza czy pod danym adresem nasłuchuje usługa.">Test</a><input type="radio" name="testcon" value="test" />
<input type="submit" value="Połącz" class="fontbold"/></td></tr><tr><td>&#160;</td></tr></tbody></table>
</form>
<script language="javascript" type="text/javascript">
	alert("\t\t\t\t\tUWAGA!\n\n\
\t\tPortal przestał wspierać ścieżkę 1.7 BMD. \n\n\
O ile połączenie z takim serwerem się powiedzie to funkcjonalność może być bardzo ograniczona. \
Wynika to z faktu zmian w pliku WSDL i dostosowania portalu pod nowe funkcje.");
</script>
</div>
</div>
<? 
require_once('tailtree.php'); 
?>
