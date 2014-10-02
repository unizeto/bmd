<?php
function GetListTree($cert,$ret,$odstepy){
	
	if(is_object($ret))
	{
			
				foreach ($ret->item as $key1 => $object) {
						//var_dump($object);  
					    foreach ($object as $key2 => $value) {
					    	
					        if ($value instanceof stdClass) {
					        	
					            foreach ($value->item as $key3 => $item) {
					            	
					                if ($key3 == 0) {
					                    $newArray[$key1][$key2] = $item;
					                } else {
					                    $newArray[$key1][$key2] .= $odstepy.$item;
					                }
					                
					            }
					        } else {
					            $newArray[$key1][$key2] = $value;
					        }
					    }
					}
		return($newArray);
		
		
	}else{
		return($ret);
	}
	
}
?>