function ajaxMakeRequest(url, callback, mode, params, xml, id) {
    var http_request = false;
    oldmode = mode;
    mode = new String(mode).toUpperCase();
    if(mode != 'POST' && mode != 'GET') {
        alert('mode: should be POST or GET (current value is: '+oldmode+')');
        return;
    }
    
    if (window.XMLHttpRequest) { // Mozilla, Safari, ...
        http_request = new XMLHttpRequest();
        if (http_request.overrideMimeType) {
            http_request.overrideMimeType('text/xml');
            // See note below about this line
        }
    } else if (window.ActiveXObject) { // IE
        try {
            http_request = new ActiveXObject("Msxml2.XMLHTTP");
        } catch (e) {
            try {
                http_request = new ActiveXObject("Microsoft.XMLHTTP");
            } catch (e) {}
        }
    }
 
    if (!http_request) {
        alert('Giving up :( Cannot create an XMLHTTP instance');
        return false;
    }
    http_request.onreadystatechange = function() { eval('ajaxCallBack(http_request, callback, xml, id)'); };
 
    http_request.open(mode, url, true);

    if(mode == 'POST' && params != null)
        http_request.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
        http_request.send(params);
}
 
function ajaxCallBack(http_request, callback, xml, id) { 
    if(http_request.readyState == 4) {
        if (http_request.status == 200) {
            if(xml == true) 
                eval(callback+'(http_request.responseXML,id);');
            else 
                eval(callback+'(http_request.responseText,id);');
        }
        else {
            //alert('There was a problem with the request.');
        }
    }
}
