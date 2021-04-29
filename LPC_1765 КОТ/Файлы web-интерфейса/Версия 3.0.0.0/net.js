function CreateRequest(){
    var Request = createRequestObject();
	if (Request == null || Request == undefined)
		alert("Ваш браузер не поддерживает XMLHttpRequest");
	return Request;
}

function createRequestObject() {
	if(XMLHttpRequest)
		return new XMLHttpRequest();
	else {
		try { return new ActiveXObject("Msxml2.XMLHTTP.6.0"); }
        catch (e) { }
        try { return new ActiveXObject("Msxml2.XMLHTTP.3.0"); }
        catch (e) { }
        try { return new ActiveXObject("Msxml2.XMLHTTP"); }
        catch (e) { }
        try { return new ActiveXObject("Microsoft.XMLHTTP"); }
        catch (e) { }
		return null;
	}
}

function SendRequest(r_method, r_path, r_args, r_handler, r_tout_handler, r_tout){
	var Request = CreateRequest();
	var RequestTimeout = r_tout_handler == null ? null : setTimeout(RequestTimedOut, r_tout == null ? 1000 : r_tout);
	if (!Request)
		return;
	
	function RequestTimedOut(){
		Request.abort();
		r_tout_handler();
	}
	
	Request.onreadystatechange = function(){
		if (Request.readyState == 4 && (Request.status == 200 || Request.status == 0)){
		
			function StringtoXML(text){
				
                //if (typeof (ActiveXObject) != "undefined"){
				try{
                  var doc=new ActiveXObject("Microsoft.XMLDOM");
                  doc.async='false';
                  doc.loadXML(text);
				  return doc;
                } 
				catch (e) {}
				var parser=new DOMParser();
                var doc=parser.parseFromString(text,"text/xml");
                return doc;
            }
		
			clearTimeout(RequestTimeout);
			
			var resp = {};
			resp.responseText = Request.responseText;
			resp.responseXML = StringtoXML(Request.responseText);
			
			if(r_handler != null)
				r_handler(resp);
		}
	}
	if (r_method.toLowerCase() == "get" && r_args.length > 0)
		r_path += "?" + r_args;
	Request.open(r_method, r_path, true);
	if (r_method.toLowerCase() == "post"){
		Request.setRequestHeader("Content-Type","application/x-www-form-urlencoded; charset=utf-8");
		Request.send(r_args);
	}
	else
		Request.send(null);
}
function GetXMLFile(filename, handler, tout_handler, tout){	
	SendRequest("POST",filename,"",handler, tout_handler, tout);
}