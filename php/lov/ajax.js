// $Header$
//
// Ajax helper object
//
// Code is CLL, see http://permalink.de/tino/cll
// (Only ideas stolen from diverse sources.)
//
// $Log$
// Revision 1.1  2009-03-15 03:56:18  tino
// Added
//

function $(E){if(typeof E=='string')E=document.getElementById(E);return E}
function $$(E){return $(E).innerHTML}
function $$$(E,S){$(E).innerHTML=S}

var ajax={}

// Helper
ajax.html = function(s) { return s.replace(/&/g,"&amp;").replace(/</g,"&lt;") }

// Debug
ajax.dump = function(name,o,n)
{
  if (typeof o=="string")
    return name+"("+(typeof o)+")='"+o.replace(/'/g,"''")+"'";
  if (typeof o!="object")
    return name+"("+(typeof o)+")="+o;
  if (--n<=0)
    return name+" is "+(typeof o);
  var s="";
  for (var i in o)
    {
      try {
        s	+= "\n"+this.dump(name+"."+i,o[i],n);
      } catch (e) {
        s	+= "\n"+name+"."+i+" exception "+e.toString();
      }
    }
  return s=="" ? name+" empty "+(typeof o) : s.substring(1);
}
ajax.debug = function(name,o,lev) { return this.dump(name,o,lev ? lev : 2) }

ajax.ob={};
ajax.ob.ajax=ajax;

// XMLHttpRequest
ajax.ajax=function(){
	this.ajax=function(){return new ActiveXObject('Microsoft.XMLHTTP')}
	try{return this.ajax()}catch(e){}
	this.ajax=function(){return new ActiveXObject('Msxml2.XMLHTTP')}
	try{return this.ajax()}catch(e){}
	this.ajax=function(){return new XMLHttpRequest()}
	return this.ajax()
	}

ajax.open=function(m,u,b){var x=this.ajax();x.open(m,u,b);return x}
ajax.cb=function(x,f,us){x.onreadystatechange=function(){if(x.readyState==4)f(x.responseText,us)};return x}
ajax.head=function(x,h,p){x.setRequestHeader(h,p);return x}
ajax.send=function(x,a){x.send(a);return x}

// Async: GET/POST
ajax.get=function(u,fn,usr){return this.send(this.cb(this.open('GET',u,true),fn,usr),"")}
ajax.post=function(u,fn,args,usr){return this.send(this.head(this.cb(this.open('POST',u,true),fn,usr),'Content-type','application/x-www-form-urlencoded'), args)}

// Sync: GET/POST
ajax.read=function(u){return this.send(this.open('GET',u,false), "").responseText}
ajax.write=function(u,args){return this.send(this.open('POST',u,false),args).responseText}

// Form
ajax.nv=function(e){return e.name?encodeURIComponent(e.name)+'='+encodeURIComponent(e.value):null}
ajax.iv=function(i){return(i.type!='radio'&&i.type!='checkbox')||i.checked?this.nv(i):null}
ajax.fe=function(frm){var f=$(frm);return function(n){return f.getElementsByTagName(n)}}
ajax.collect=function(a,f){var n=[];for(var i=0;i<a.length;i++){var v=f(a[i]);if(v!=null)n.push(v)}return n}
ajax.serialize=function(f){var g=this.fe(f);return this.collect(g('input'),this.iv).concat(this.collect(g('select'),this.nv)).concat(this.collect(g('textarea'),this.nv)).join('&')}

// Quick
ajax.put=function(el){var e=$(el);return function(r){e.innerHTML=r}}
ajax.upd=function(u,e){return this.get(u,this.put(e))}
ajax.submit=function(u,e,frm){return this.post(u,this.put(e),this.serialize(frm))}

