
function common_clearForms(form_name){
    
    var frm_elements = form_name.elements;
    
    for(i=0; i<frm_elements.length; i++) {
	
		field_type = frm_elements[i].type.toLowerCase();
	/*
		if (frm_elements[i].name.toLowerCase().indexOf(form_name)!=0) {
			continue;
		}
	*/
		switch(field_type) {
		
		case "text":
		case "password":
		case "textarea":
		case "hidden":
			frm_elements[i].value = "";
			break;
		
		case "radio":
		case "checkbox":			
		if (frm_elements[i].checked) {
		frm_elements[i].checked = false;
			}
			break;
	
	}        
  }
} 

function common_grayinput(inputnr,inputname){
	if (inputnr.checked=='checked' || inputnr.checked){
		document.getElementById(inputname).disabled=false;
	}else
	{
		document.getElementById(inputname).disabled=true;
	}
}

function formChanged(form,user) {
	var frm_elements = form.elements;
	var zmiana=0;
	
	if (document.getElementById('userRoleList').value!=document.getElementById('userRoleList').defaultValue){
    	zmiana = 1 ;
    }
	
    for(var i=0; i<frm_elements.length; i++) {
	    if (frm_elements[i].type == "text"){
	      if(frm_elements[i].value != frm_elements[i].defaultValue){
	    	 zmiana = 1 ;
	      }      
	    }
	        
	    if (frm_elements[i].type == "checkbox"){
	    	if (frm_elements[i].checked != document.getElementById('checkbox_disabled').checked){
	    		zmiana += 2 ;
	    	}
	    }    
    }    
 
    if (zmiana != 0){
    	form.action='edituser.php?status='+zmiana;
    	
    }else
    {
    	form.action='form_edituser.php?user='+user;
    	// alert('Brak zmian w formularzu.');
    }
}

function formDeleteUser(form,user,url){
	if (confirm("Czy na pewno chcesz usunąć użytkownika?")){
		alert('Usuwanie użytkownika '+user);
		form.action='edituser.php?status='+url;
	}
}
function changeAction(form,url){
	form.action=url;
}

function wybrane(skad,dokad,chosenone){
	var zrodlo= document.getElementById(skad);
	var cel=document.getElementById(dokad);
	var licznik=0;
	
	if (zrodlo.length>0){
		for (var x=0;x<zrodlo.length;x++){
			if(zrodlo.options[x].selected){
				var NowyElem = new Option(zrodlo.options[x].firstChild.nodeValue,zrodlo.options[x].firstChild.nodeValue,0,0);
				cel.options[licznik]=NowyElem;
				if (chosenone!=null ){
					if(zrodlo.options[x].firstChild.nodeValue == chosenone){
						cel.options[licznik].selected="selected";
					}
				}
				licznik=licznik+1;
			}			
		}
		// removing unused positions		
		for(var y=licznik;y<cel.options.length;y+1){
			cel.options[y]=null;						
		}			
		licznik=0;
	}
}

