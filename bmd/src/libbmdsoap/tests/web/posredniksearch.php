<?
session_start();
ini_set("soap.wsdl_cache_enabled", "0"); // disabling WSDL cache 
ini_set("display_errors",1);
error_reporting(E_STRICT|E_ALL);

if(isset($_POST['prev'])) {
	if ($_SESSION['offset']!=0) {
		$_SESSION['offset']-=$_SESSION['limit'];
	}
}
if(isset($_POST['next'])) {
	if ($_POST['iloscwynikow']>=$_SESSION['limit']){
		$_SESSION['offset']+=$_SESSION['limit'];
	}
}

header("Location:searchresults.php");

?>