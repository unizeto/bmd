<?php
ini_set("soap.wsdl_cache_enabled", "0"); // disabling WSDL cache 
function microtime_float()
{
    list($utime, $time) = explode(" ", microtime());
    return ((float)$utime + (float)$time);
}
$script_start = microtime_float();


try {
$client = new SoapClient("http://127.0.0.1:8080/MessageRouter-Router/QueueManager?wsdl", array('trace' => true, 'exceptions' => true));

//$XML = file_get_contents("formularz.xml");
//$XML = file_get_contents("formularz.xad");
$XML = file_get_contents("simplysimple.xml");
$XML = file_get_contents("samplelogs.xml");


    //for($i=0;$i<1000;$i++) {
 	$response = $client->put($XML, "Logi");
//		$response = $client->put($XML, "test1");
    //}
    //header("Content-type:text/xml");
	 print("OKAY!\n");
	 print_r("<pre>".htmlspecialchars($XML)."</pre>");
    //print_r($client->__getLastRequest());
    //print_r(base64_decode($response->status->komunikat));
    //print_r($client->__getLastResponse());
} catch(Exception $e) {
    //header("Content-type:text/xml");
	 echo "EXCEPTION!";
    print("Last REQUEST:<br/>\n");
	 pr($client->__getLastRequest());
    print("Last RESPONSE:<br/>\n");
	 pr($client->__getLastResponse());
    print("Exception details:<br/>\n");
	 pr($e);
}
$script_end = microtime_float();
//echo "Script executed in ".bcsub($script_end, $script_start, 4)." seconds.";
?>