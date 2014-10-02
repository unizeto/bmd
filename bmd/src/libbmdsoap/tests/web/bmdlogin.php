<?
session_start();
/**
 * Pobranie listy metadanych charakteryzujących dane przechowywane w archiwum po zalogowaniu się użytkownika do serwera przez podanie certyfikatu
 */
ini_set("soap.wsdl_cache_enabled", "0"); // disabling WSDL cache 
ini_set("display_errors",1);
error_reporting(E_STRICT|E_ALL);
require_once('DAO/com_func.php');

if ( !isset($_POST['SOAPaddress'] ) || empty($_POST['SOAPaddress']) ) { 
	draw_htmlhead('Komunikat SOE SOAP WWW','');
	print('<div class="glowny wysrodkuj">Nie podano adresu serwera usług SOAP.<br /> 
Proszę wrócić na stronę główną logowania lub kliknąć <a href="index.php">tutaj</a>
</div>');	
	require_once("tailtree.php");	
	die(); 
}else{
	$_SESSION['SOAPaddress']=$_POST['SOAPaddress'];
}
//////////////////////////////////////////////////////////////

draw_htmlhead('Wynik procesu logowania do SOE SOAP WWW','');
print('<div class="glowny">');
$ret=-987654321;

if (isset($_FILES['certyfikat']) && !empty($_FILES['certyfikat']['tmp_name'])){
	$_SESSION['cert']=file_get_contents($_FILES['certyfikat']['tmp_name']);
}

require_once "certyfikat.php";
/*obiekt klasy pomocniczej*/
$BMDSOAP = new BMDSOAP();


///////////////////////////////////////////////////////////////
if ( isset($_POST['testcon']) && $_POST['testcon']=='test' ){
$ret = $BMDSOAP->testConnection();
//print($ret);
if ( strpos($ret,'onnected to server. 1')!=1 ){	
	$_SESSION['ERROR']='1';
	print('Serwer nie nasłuchuje :'.$ret);
	print('<p style="text-align: center;"><a href="index.php">logowanie</a></p></div>');
	require_once "tailtree.php";	 
} else { 
//	$_SESSION['ERROR']='6'; 
	print('Serwer nasłuchuje :'.$ret.'<br />');
	print('Udane połączenie testowe nie jest równoznaczne z pełną możliwością pracy z usługą.<br />
	Proszę teraz przeprowadzić test logowania. ');
	print('<p style="text-align: center;"><a href="index.php">logowanie</a></p></div>');
	require_once "tailtree.php";
}

}else{

$_SESSION['SOAPver']=strtolower(trim($_POST['SOAPver']));
	
$ret = $BMDSOAP->bmdLogin($cert);
/*pobranie odpowiedzi z serwera - tablica obiektów(rodzajów metadanych)
    $ret->mtdOid - identyfikator OID metadanej
    $ret->mtdDesc - opis metadanej
*/



if(is_array($ret)) {
$_SESSION['ERROR']='8';    
//$plik=fopen("bmdlogin.txt","w");
//fwrite($plik,$ret);

//header("Content-type: text/html; charset=utf-8");

	print ('Zapytanie zwróciło "' . count($ret) . '" wyników: <br />');
	  print('<br /><table class="w_lewo"><tbody>');
    foreach ($ret as $key => $value) {
	 print("<tr><td>". $value->mtdOid . "</td><td>" . $value->mtdDesc . "</td></tr>");
//	$temp=$value->mtdDesc;
//	fwrite($plik,$temp);
}
print("</tbody></table>");
print('<br />
<form action="main.php" method="post">
<input type="submit" value="Na stronę główną" />
</form></div>');
require_once "tail.php";
} else {
    //header("Content-type: text/plain; charset=utf-8");
    //$_SESSION['SOAPaddress']=NULL;
    $_SESSION['ERROR']='1';
    print('Mamy błąd :'.$ret);
    print('<p style="text-align: center;"><a href="index.php">logowanie</a></p></div>');
    require_once "tailtree.php";
	 }
}
?>
