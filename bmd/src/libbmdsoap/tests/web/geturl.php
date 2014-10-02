<?php

/**
 * Główny kontroler systemu. Odpowiada za uruchomienie odpowiednich plg-inów.
 */

 $server_name = strtolower($_SERVER["SERVER_NAME"]);
 $server_port = (":" . $_SERVER["SERVER_PORT"])==":80"?"":$_SERVER["SERVER_PORT"];
 $server_dir  = dirname($_SERVER["SCRIPT_NAME"]);
 $url = 'http://'.$server_name.''.$server_port.''.$server_dir.'/';
 //$dir = dirname($_SERVER["SCRIPT_FILENAME"]);
 //print($url);
 
$name = $_REQUEST["name"];
//$name = $url.$name;


header("Expires: ".date("r",time()+60)); 
header("Cache-control: public");
header("Pragma: cache");

if ($name == "") {
	header("HTTP/1.1 404 not found");
	header("Status: 404 not found");
	die("404 not found");
}
$modified = stat($path);
header("Last-Modified: ".date("r",$modified["mtime"]));
header('HTTP/1.1 301 Moved Permanently'); 
header('Status: 301 Moved Permanently');

header("Location: $name?".$_SERVER['QUERY_STRING']);

die();
?>