<?php
session_start();

//check_siteblock();//testowo wylaczone - cos nie dziala poprawnie, 090331

ini_set("soap.wsdl_cache_enabled", "0"); // disabling WSDL cache 
ini_set("display_errors",1);
error_reporting(E_STRICT|E_ALL);

//require_once('DAO/com_func.php');
require_once "certyfikat.php";
draw_htmlhead('Portal SOE WWW - panel administracyjny','');
?>
<div class="glowny w_lewo">
<fieldset>
<legend>Panel administracyjny</legend>
<br />
<div style="margin-left: auto; margin-right: auto; text-align: center;">
<button type="button" onclick="javascript:window.location.href='roles.php'">Lista ról</button>
<button type="button" onclick="javascript:window.location.href='groups.php'">Lista grup</button>
<button type="button" onclick="javascript:window.location.href='users.php'">Lista użytkowników</button>
</div>
<?php 


?>
<br />
</fieldset>
</div>
<?php 
require_once "tail.php";
?>