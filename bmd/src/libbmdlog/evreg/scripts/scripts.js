var glob;

	function ajaxFunction(id)
	{
		var xmlHttp;
		
		try
		{
			xmlHttp=new XMLHttpRequest();
  	}
		catch (e)
		{
			try
			{
				xmlHttp=new ActiveXObject("Msxml2.XMLHTTP");
			}
			catch (e)
			{
				try
				{
					xmlHttp=new ActiveXObject("Microsoft.XMLHTTP");
				}
				catch (e)
				{
					alert("Your browser does not support AJAX!");
      		return false;
				}
			}
		}
		xmlHttp.onreadystatechange=function()
    {
			if(xmlHttp.readyState==4)
      {
      	glob = xmlHttp.responseText;
      	textArray = xmlHttp.responseText.split("|");
      	alert(textArray[1]);
      	imageReplace("img_" + id, textArray[0]);
       imageReplace2("imgt_" + id, textArray[0]);
      }
    }
    xmlHttp.open("GET","pages/verify.php?id="+id,true);
		xmlHttp.send(null);
  }

	function imageReplace(id, status)
	{
  	img = document.getElementById(id);
  	
  	if (!img) return;
  
  	switch(status)
  	{
    	case '0' : img.src = "pictures/agt_action_success.png"; img.title="Verification success";  break;
    	default : img.src = "pictures/agt_action_fail.png"; img.title="Verification fail";
  	}
	}

 function imageReplace2(id, status)
 {
     img = document.getElementById(id);
   
     if (!img) return;

     if (status == 0 || status == 1 ) {
       img.style.visibility = "visible";
      }
 }

	function ShowHide(id)
	{
  	obj = document.getElementById(id);
  	
		if ( obj.style.display == "none" )
   	{
			document.getElementById('colsMenu').src = "pictures/1rihgtarrow2.png"
			obj.style.display = "block";
		} 
		else
		{
			document.getElementById('columns').submit();
			document.getElementById('colsMenu').src = "pictures/1downarrow1.png";
			obj.style.display = "none";
		}
	}


	function changeoffset(num)
	{
		document.getElementById('offset').value = num;
		document.getElementById('panel').submit();
	}

	function changeAppState()
	{
		switch ( arguments.length )
		{
			case 6 :	{
									document.getElementById('opType').value = arguments[2];
									document.getElementById('remotePid').value = arguments[3];
									document.getElementById('remotePort').value = arguments[4];
									document.getElementById('logId').value = arguments[5];
								}
			case 2 :  {
									document.getElementById('user').value = arguments[1];
								}
			case 1 : 	{
									document.getElementById('appState').value = arguments[0];
								}
		}

		document.getElementById('offset').value = 0;
		document.getElementById('panel').submit();
	}

	function sortOrder(columnName)
	{
		document.getElementById('sortBy').value = columnName;
		document.getElementById('panel').submit();
	}

 function gotoTreeView(id)
 {
     var form = document.createElement("form");
     form.action = "tree/index.php";
     form.method = "post";
     var inp1 = document.createElement("input");
     inp1.type = "hidden";
     inp1.name = "logBranch";
     inp1.value = id;
     form.appendChild(inp1);
     document.body.appendChild(form);
     form.submit();
 }

 