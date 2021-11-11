var nonVolatileHashChange = false;
var hashChangedFn;
	
export function init(onHashChanged) {	
	hashChangedFn = onHashChanged;
	window.onhashchange = onHashChangedManager;
}
	
function onHashChangedManager(e) {			
	//Non-volatile
	if (nonVolatileHashChange) {
		nonVolatileHashChange = false;
		e.preventDefault();
		e.stopPropagation();
		
	}
	else hashChangedFn(e); //Regularly scheduled hash event		
}
	
export function setHashNonVolatile(val) {	
	if (window.location.hash.replace('#') != val) {
		nonVolatileHashChange = true;
		window.location.hash = val; //This will trigger a hash event
	}
}
	
