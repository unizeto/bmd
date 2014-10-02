<?
//require_once "DAO/bmdsoap081203.php";
//require_once "DAO/bmdsoap090303.php";
//require_once "DAO/bmdsoap16_090310.php";
//require_once "DAO/bmdsoap090330.php";
//require_once "DAO/bmdsoap090626.php";
//require_once "DAO/bmdsoap090629.php";
//require_once "DAO/bmdsoap090630.php";
require_once "DAO/bmdsoap090703.php";
$wlaczony=true;
$cert=null;

if ( $wlaczony )
{
//	$cert = file_get_contents("cert/log_klient_ade_1.der");
//	$cert = file_get_contents("cert/mizia.cer");
	//$cert=file_get_contents("cert/marua.der");
	$cert=(isset($_SESSION['cert']))?$_SESSION['cert']:null;
	// 298014.cer -authoff/oki , piekarski.der ,
	/*
	 * 298014.cer -authoff/oki , dateval/ oki, crl/
	 * piekarski.der -authoff/oki , dateval/err, crl/
	 * TomaszLitarowicz.der -authoff/oki , dateval/oki, crl/oki
	 * twojciechowski_level4.cer -authoff/oki , dateval/oki, crl/oki
	 * 
	 */ 
#$cert="jakis_tekst"; 
}
?>
