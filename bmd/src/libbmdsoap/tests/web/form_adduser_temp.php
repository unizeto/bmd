<?php
session_start();

$info=shell_exec('openssl x509 -inform DER -in '.$_FILES['plik']['tmp_name'].' -serial -issuer -noout');

$tmp=explode("\n",$info);
print_r($tmp);
$tmp[1]=str_replace("/","> ",$tmp[1]);
$tmp[1]=str_replace("=","=<",$tmp[1]);
$tmp[0]=mb_substr($tmp[0],7);
$temp=str_split($tmp[0],2);
$tmp[0]=null;

for($i=0;$i<sizeof($temp);$i++) 
	$tmp[0].=$temp[$i]." ";
$tmp[0].=$temp[$i];

$tmp[1]=mb_substr($tmp[1],11).">";
$_SESSION['certinfo']=$tmp;
header("Location:form_adduser.php");
?>