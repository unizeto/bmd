<?
session_start();
/**
 * Pobranie wartości metadanych plików spełniających kryteria wyszukiwania.
 */

ini_set("soap.wsdl_cache_enabled", "0"); // disabling WSDL cache 
ini_set("display_errors",1);
error_reporting(E_STRICT|E_ALL);
#require_once "DAO/bmdsoap080513.php";
require_once "certyfikat.php";
//require_once "oidsDesc.php";

/*obiekt klasy pomocniczej*/
$BMDSOAP = new BMDSOAP();
//print_r($_POST);
/*
1.2.616.1.113527.4.3.2.5.2 Nazwa faktury
1.2.616.1.113527.4.3.3.13 Typ faktury
1.2.616.1.113527.4.3.3.1 Numer faktury
1.2.616.1.113527.4.3.3.6 Data wystawienia
1.2.616.1.113527.4.3.3.16 Data sprzedazy
1.2.616.1.113527.4.3.3.4 NIP odbiorcy
1.2.616.1.113527.4.3.3.5 Nazwa odbiorcy
1.2.616.1.113527.4.3.3.2 NIP wystawcy
1.2.616.1.113527.4.3.3.3 Nazwa wystawcy
1.2.616.1.113527.4.3.3.7 Data płatności
1.2.616.1.113527.4.3.3.8 E-mail odbiorcy
1.2.616.1.113527.4.3.3.9 E-mail nadawcy
1.2.616.1.113527.4.3.3.11 Kwota netto
1.2.616.1.113527.4.3.3.12 Kwota brutto
1.2.616.1.113527.4.3.3.15 Do numer
1.2.616.1.113527.4.3.3.14 Do data
1.2.616.1.113527.4.3.2.5.1 Data wysłania
*/

/*znak ">" to dodanie na końcu oidu interesującej nas metadanej przyrostka ".2"*/
//data wystawienia od
/*znak "<" to dodanie na końcu oidu interesującej nas metadanej przyrostka ".1"*/
//data wystawienia do
//$params[] = array("mtdOid" => "1.2.616.1.113527.4.3.3.13","mtdValue" => "zażółcić gęślą jaźń\"!@#$%^&*()_+-=[]\{}|;':,./<>?`~01234567890");
//$params = array();
//$sortInfo[] = array("sortOid"=>"1.2.616.1.113527.4.3.2.5.2","sortOrder"=>"DESC");//nazwa pliku
//$sortInfo[] = array("sortOid"=>"1.2.616.1.113527.4.3.3.5","sortOrder"=>"ASC");
//$sortInfo[] = array("sortOid"=>"1.2.616.1.113527.4.3.3.12","sortOrder"=>"DESC");
//$mtdsparams[] = array("mtdOid" => "1.2.616.1.113527.4.3.3.13");
//$mtdsparams[] = array("mtdOid" => "1.2.616.1.113527.4.3.3.6");
//$mtdsparams[] = array("mtdOid" => "1.2.616.1.113527.4.3.3.7");
//$mtdsparams[] = array("mtdOid" => "1.2.616.1.113527.4.3.3.11");
//$mtdsparams[] = array("mtdOid" => "1.2.616.1.113527.4.3.3.8");
//$mtdsparams[] = array("mtdOid" => "1.2.616.1.113527.4.3.2.5.1");
//$params[] = array("mtdOid" => "1.2.616.1.113527.4.3.3.8","mtdValue" => "Kos","mtdDesc"=>"ILIKE");
/*limit - ilość wyników*/
//$limit = 100;
/*offset - indeks pierwszego wyniku*/
//$offset = 0;

//$params=NULL;
//$sortInfo=NULL;
//$cert=NULL;
/*  wywołanie serwisu*/
$params = array();
$sortInfo = array();
$mtdsparams = array();

$_SESSION['kol_filename']=false;

$_SESSION['offset']=$_POST['offset'];
$_SESSION['limit']= $_POST['limit'];

$idx=0;
while ($idx < $_SESSION['ilosc']){
//parametry wyszukiwania
$index="operwyb".$idx;
$szukaj="szukaj".$idx;
$zakres="zakres".$idx;

if(isset($_POST[$zakres]) && $_POST[$zakres] == 'on'){
//przyjmuje,że jeśli występuje checkbox zakresowy do drugie pole daty istnieje i nie sprawdzam istnienia
$szukaj2="szukajdo".$idx;

if(!empty($_POST[$szukaj]) && empty($_POST[$szukaj2])){
$params[] = array("mtdOid"=>$_SESSION['mtdOid'][$idx].'.2', "mtdValue"=>$_POST[$szukaj], "mtdDesc"=>$_POST[$index],"mtdStatement"=>0);
}
elseif(empty($_POST[$szukaj]) && !empty($_POST[$szukaj2])){
$params[] = array("mtdOid"=>$_SESSION['mtdOid'][$idx].'.1', "mtdValue"=>$_POST[$szukaj2], "mtdDesc"=>$_POST[$index],"mtdStatement"=>0);
}
elseif(!empty($_POST[$szukaj]) && !empty($_POST[$szukaj2])){
$params[] = array("mtdOid"=>$_SESSION['mtdOid'][$idx].'.2', "mtdValue"=>$_POST[$szukaj], "mtdDesc"=>$_POST[$index],"mtdStatement"=>0);
$params[] = array("mtdOid"=>$_SESSION['mtdOid'][$idx].'.1', "mtdValue"=>$_POST[$szukaj2], "mtdDesc"=>$_POST[$index],"mtdStatement"=>0);
}else{
//$params=null;
$params[] = array("mtdOid"=>$_SESSION['mtdOid'][$idx], "mtdValue"=>null, "mtdDesc"=>$_POST[$index],"mtdStatement"=>0);
}
/*znak ">" to dodanie na końcu oidu interesującej nas metadanej przyrostka ".2"*/
//data wystawienia od
/*znak "<" to dodanie na końcu oidu interesującej nas metadanej przyrostka ".1"*/
//data wystawienia do

}
elseif( !empty($_POST["szukaj".$idx]))
{
$index="operwyb".$idx;
$index2="negacja".$idx;
$neguj=0;
if(isset($_POST[$index2]) && ($_POST[$index2] == 'on' || $_POST[$index2] == '1' || $_POST[$index2] == 'checked' )){$neguj=1;}

$params[] = array("mtdOid"=>$_SESSION['mtdOid'][$idx], "mtdValue"=>$_POST[$szukaj], "mtdDesc"=>$_POST[$index],"mtdStatement"=>$neguj);

}

//kolumny do wyświetlenia
$index="mtdsparams".$idx;
if(isset($_POST[$index]) && $_POST[$index] == 'on'){
//print("<p>".$_SESSION['mtdOid'][$idx-1]."</p><br />");

$mtdsparams[] = array("mtdOid" => $_SESSION['mtdOid'][$idx],"mtdStatement"=>0);

if ( $_SESSION['mtdOid'][$idx]==='1.2.616.1.113527.4.3.2.5.2') 
{$_SESSION['kol_filename']=true;}
}

$index="kierunek".$idx;
if($_POST[$index] != 'NULL'){
	$sortInfo[] = array("sortOid"=>$_SESSION['mtdOid'][$idx], "sortOrder"=>$_POST[$index]);
}

$idx+=1;
}

if (empty($params)){ $params[] = array("mtdOid"=>null, "mtdValue"=>null, "mtdDesc"=>null,"mtdStatement"=>0);}
if (empty($mtdsparams)){ $mtdsparams=null;}
if (empty($sortInfo)){ $sortInfo=null;}

//print_r($mtdsparams);
//print("<br />");
//print_r($params);	
//print("<br />");
//print_r($sortInfo);	
//print("<br />");

if (isset($_POST['CSVresult']) && $_POST['CSVresult'] == 'on'){

$ret = $BMDSOAP->bmdSearchFilesCSV($cert, $params, $mtdsparams,  $_SESSION['offset'], $_SESSION['limit'], $_POST['logical'],$_POST['goper'], $sortInfo);
	if(is_object($ret)) {
		
		header("Content-type: application/octet-stream");
		header("Content-Disposition: attachment; filename=wyniki.".$ret->filename);
		/*
		foreach ($ret->file as $obj){*/
			header("Content-Length: " . strlen($ret->file));
			print( $ret->file );// $obj);
			//}
		
		} //end if else is_array
		else {
		//header("Content-type: text/plain; charset=utf-8");
		print("Error: ".$ret);	
		}
	}else{
	  $range=null;
	  if(isset($_POST['katalog']) && $_POST['katalog'] == 'on') {$range="DIR";}
/////////////////////////////////////////////////////////////////////////////////
	  if(!empty($_POST['corespID'])) {
	  	$OID="1.2.616.1.113527.4.3.3.26";
	  	if (isset($_SESSION['SOAPver']) && $_SESSION['SOAPver']=='trunk') {
	  		$OID="1.2.616.1.113527.4.3.2.5.7";
	  	}
	  	$params[] = array("mtdOid"=>$OID,  "mtdValue"=>$_POST['corespID'],"mtdDesc"=>null, "mtdStatement"=>0);
	  	$_SESSION['corespid']=$_POST['corespID'];
	  }
/////////////////////////////////////////////////////////////////////////////////
	  if(!empty($_POST['filetype']) && $_POST['filetype']=="DIR")
	  {
	  	$OID="1.2.616.1.113527.4.3.3.25";
	  	if (isset($_SESSION['SOAPver']) && $_SESSION['SOAPver']=='trunk') {
	  		$OID="1.2.616.1.113527.4.3.2.5.6";
	  	}
	  	$params[] = array("mtdOid"=>$OID,  "mtdValue"=>$_POST['filetype'],"mtdDesc"=>null, "mtdStatement"=>0);
	  }
	  
	$_SESSION['params']=$params;
	$_SESSION['mtdsparams']=$mtdsparams;
	$_SESSION['logical']=$_POST['logical'];
	$_SESSION['goper']=$_POST['goper'];
	$_SESSION['sortInfo']=$sortInfo;
	$_SESSION['range']=$range;
	
	if (isset($_POST['drzewo']) && $_POST['drzewo'] == 'on') {
	$_SESSION['range']="DIR";
	header("Location:searchresultstree.php");
	}else{
	header("Location:searchresults.php");
	}
}
//require_once "tail.php";

?>
