<?
session_start();
require_once('DAO/com_func.php');
//check_siteblock();//testowo wylaczone
draw_htmlhead('Portal SOE WWW - funkcje soap','');
?>


<p style="text-align: center;color: red;">
	<b>UWAGA!<br /> To jest portal stworzony tylko i wyłącznie jako Proof Of Concept na potrzeby testów.<br />
	 Nie zawiera żadnych zabezpieczeń przeciw atakom.</b><br />Work in progress.
</p>
<div class="glowny">
<fieldset>
<legend>SOE soap functions</legend>


<p>
<a href="serverSOAP.php" title="Logowanie do serwera z pobraniem metadanych.">bmdlogin</a>
 - Przed wykonaniem jakichkolwiek operacji sprawdź połączenie i jak serwer działa zaloguj się</p>
<p><a href="apanel.php" title="Panel administracyjny - zarządzanie użytkownikami">Panel administracyjny</a></p>
<table class="main_tab w_lewo">
<tbody>
<tr>

<td valign="top"><br />
<dl>
<dt>INSERT</dt>
<?
if (isset($_SESSION['SOAPver']) && $_SESSION['SOAPver']=='trunk') { 
print('<dd><a href="send2DB.php?formtype=createD" title="Tworzenie katalogu w bazie danych">bmdcreatedirectory</a></dd>');
print('<dd><a href="send2DB.php?formtype=createL" title="Tworzenie skrótu w bazie danych">bmdcreatelink</a></dd>');
}
?>
<dd><a href="send2DB.php?formtype=insertF" title="Wysłanie pliku na serwer">bmdinsertfile</a></dd>
<dd><!-- <a href="bmdinsertfilelist" > --><a> bmdinsertfilelist</a></dd>
<dd><a href="send2DB.php?formtype=insertF" title="Wysłanie pliku z podpisem na serwer">bmdinsertsignedfile</a></dd>
<!-- <dd><a href="bmdinsertfilechunk">bmdinsertfilechunk</a></dd>-->
<dd><a href="send2DB.php?formtype=insertSig" title="Dodanie pliku z podpisem do istniejącego pliku">bmdAddSignature</a></dd>
</dl>
<br />
</td>
<td valign="top"><br />
<dl>
<dt>SEARCH</dt>
<dd><a href="searchForm.php" title="Wyszukiwanie plików w bazie">bmdsearchfiles</a></dd>
<!-- <dd><a href="bmdsearchfileswithor">bmdsearchfileswithor</a></dd>-->
<dd><a href="searchForm.php" title="Wyszukiwanie plików w bazie z zapisem wyników do pliku CSV">bmdsearchfiles_csv</a></dd>
<dd>---------------------</dd>
<dd><a href="searchForm_dynamic.php" title="Wyszukiwanie plików w bazie z dynamicznym tworzeniem kryteriów wyszukiwania">bmdsearchfiles_dynamic</a></dd>
</dl>
<br />
</td>
<td valign="top"><br />
<dl>
<dt>GET</dt>
<dd><a href="fileByID.php?formtype=hash" title="Pobranie pliku  z serwera po jego skrócie">bmdgetfilebyhash</a></dd>
<dd><a href="fileByID.php?formtype=file" title="Pobranie pliku z serwera po jego ID w bazie">bmdgetfilebyid</a></dd>
<dd><a href="fileByID.php?formtype=pki" title="Wyświetlenie dostępnych wartości dowodowych dla pliku o zadanym ID">bmdgetfilepkibyid</a></dd>
<dd><a href="fileByID.php?formtype=zippack" title="Pobranie wszystkich dowodów w paczce ZIP dla pliku o zadanym ID">bmdgetfilebyidinzippack</a></dd>
<dd><a href="fileByID.php?formtype=prop" title="Pobranie dowodów (tsr/sig) pliku o zadanym ID">bmdgetfilepropbyid</a></dd>
<dd><a href="fileByID.php?formtype=history" title="Wyświetlenie historii pliku o zadanym ID">bmdhistory</a></dd>
<dd><a href="inputgettsr.php" title="Pobranie znacznika czasu dla wrzucanego pliku - niezależne od pracy serwera BMD">bmdgettimestamp</a></dd>
<!-- <dd><a href="bmdgetfilechunk">bmdgetfilechunk</a></dd> -->
</dl>
<br />
</td>
</tr>
<tr>

<td valign="top"><br />
<dl>
<dt>UPDATE METADATA</dt>
<dd><a href="send2DB.php?formtype=updateF" title="Uaktualnienie metadanych dla pliku o zadanym ID">bmdupdatemetadata</a></dd>
</dl>
<br />
</td>
<td valign="top"><br />
<dl>
<dt title="Na tą chwilę opcje 'PLAY specific'">INVOICE</dt>
<dd><a href="invoice.php?formtype=htmlimage" title="Pobranie obrazu faktury w postaci html - PLAY">gethtmlimage</a></dd>
<dd><a href="invoice.php?formtype=getpack" title=" Pobranie wszystkich dowodów w paczce ZIP dla pliku o zadanych kryteriach - PLAY">getinvoicepack</a></dd>
<dd><a href="invoice.php?formtype=count" title="Liczba faktur spełniających kryteria - PLAY">countInvoice</a></dd>
<dd><a href="invoice.php?formtype=verify" title="Weryfikacja poprawności podpisu poprzez usługę DVCS - PLAY">verifyInvoice</a></dd>
<?
if (isset($_SESSION['SOAPver']) && $_SESSION['SOAPver']=='trunk') { 
print('<dd>
<a href="invoice.php?formtype=verifyD" title="Weryfikacja poprawności podpisu poprzez usługę DVCS z zapisem pliku na dysk - PLAY">
verifyInvoiceWithDetails
</a>
</dd>');
}
?>
</dl>
<br />
</td>
<td valign="top"><br />
<dl>
<dt>DELETE</dt>
<dd><a href="fileByID.php?formtype=del" title="Usunięcie z bazy pliku o zadanym ID">bmddeletefilebyid</a></dd>
</dl>
<br />
</td>
</tr>
</tbody>
</table>
<br />
</fieldset>
</div>

<?php 
//wrzucone w tagi php aby ecplise nie zglaszal bledow nieotwartych tagow ;P
print('<p style="text-align: center;font-size: 11px;"><a href="information.php" title="Pod linkiem dodatkowe informacje do witryny">Informacja</a></p>
<hr />
<p style="text-align: center;font-size: 11px;"> &copy; 2008-2009 by Unizeto Technologies SA. Wszelkie prawa zastrzeżone.<br />
Uwagi, błędy, sugestie - Łukasz Kośmidek - Dział wdrożeń, pkj 326.<br /> lkosmidek[at]unet[dot]pl , lukasz[dot]kosmidek[at]unizeto[dot]pl</p>

</body>
</html>');
?>