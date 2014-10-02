<?
//session_start();
ini_set("soap.wsdl_cache_enabled", "0"); // disabling WSDL cache 
ini_set("display_errors",1);
error_reporting(E_STRICT|E_ALL);
require_once('DAO/com_func.php');
draw_htmlhead('Przegląd zawartości bazy w postaci drzewa katalogów - SOE SOAP WWW','searchtree');

print('<div class="searchtree_column w_lewo"><dl>');
require_once "funkcja_tree.php";
//print('</div>');
require_once "tailtree.php";
?>