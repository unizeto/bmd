<?php

header("Cache-Control: no-cache, must-revalidate");
header('Pragma: no-cache');

set_time_limit(0);
ignore_user_abort(1);


if (isset($_GET['id'])){
 $id = $_GET['id'];
 }
else{
 echo 'Brak identyfikatora węzła z podpisem';
 }

if(file_exists('../../config/config.php'))
 include('../../config/config.php');
echo ($AUDITOR.' -h'.$DB_HOST.' -p'.$DB_PORT.' -d'.$DB_NAME.' -u'.$DB_LOGIN.' -m'.$DB_PASSWORD.' -s'.$id );
$out =  exec($AUDITOR.' -h'.$DB_HOST.' -p'.$DB_PORT.' -d'.$DB_NAME.' -u'.$DB_LOGIN.' -m'.$DB_PASSWORD.' -s'.$id, $output, $return_var );

?>