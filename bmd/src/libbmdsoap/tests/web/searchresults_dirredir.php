<?php
session_start();
$params[] = array("mtdOid"=>"1.2.616.1.113527.4.3.2.5.7",  "mtdValue"=>urldecode(bsae64_decode($_GET['cid'])),"mtdDesc"=>null, "mtdStatement"=>0);
$_SESSION['params']=$params;
header("Location:searchresults.php");

?>