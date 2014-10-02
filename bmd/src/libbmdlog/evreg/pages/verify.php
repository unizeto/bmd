<?php

function __autoload($class_name) {

   if (file_exists("../classes/".strtolower($class_name) . '.php')) {
    require_once "../classes/".strtolower($class_name) . '.php';
   } else if (file_exists("../interfaces/".strtolower($class_name) . '.php')) {
    require_once "../interfaces/".strtolower($class_name) . '.php';
   } else return false;

   return true;
}

if (isset($_GET['id'])){
 $id = $_GET['id'];
 }
else{
 echo 'Brak identyfikatora logu';
 }

if(file_exists('../config/config.php'))
 include('../config/config.php');

exec($VERIFIER.' -h'.$DB_HOST.' -p'.$DB_PORT.' -d'.$DB_NAME.' -u'.$DB_LOGIN.' -m'.$DB_PASSWORD.' -l'.$id, $output, $return_var );

//($return_var & 127)-($return_var & 128)

switch ( $return_var )
{
case 0 :
    echo $return_var. '|';
    echo 'LOG O ID '.$id." NIE BYL MODYFIKOWANY\n";
    echo 'WERYFIKACJA ZAKONCZONA POMYSLNIE';
    break;
case 1:
    echo $return_var. '|';
    echo 'UWAGA!!! LOG O ID '.$id.' JEST ZMODYFIKOWANY!!!';
    break;
case 2:
case 3:
    echo $return_var. '|';
    echo 'UWAGA!!! DOKONANO MODYFIKACJI W GALEZI LOGOW (NIEZGODNOSC SKROTOW)';
    break;

default:
    echo $return_var. '|';
    echo 'BLAD!!! NIE MOZNA PRZEPROWADZIC WERYFIKACJI LOGU O ID '.$id. '.!!! NARUSZONO STRUKTURĘ DRZEWA';
}

?>