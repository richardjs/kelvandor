const URL = 'http://127.0.0.1:5000/'; //For other domains add "Access-Control-Allow-Origin: *" to the header

	
//var playerId = +(new Date()); //Timestamp
	
export function getMove(boardStr, onPlayed) {		
	//if (!networkUrl) networkUrl = prompt('Enter a service URL', URL);

			
	var url = URL + boardStr;
	
	ajax(url, function(data, status) {
		//Optional argument to log info 
		if (data.hasOwnProperty('log') && data.log) console.log(data.log); 
		
		//Expect json data that has an array of kmn action strings
		var actions = data.actions;
		if (actions) return onPlayed(actions);		
		else alert ('Invalid move!');
	});
	
}	

			
//Vanilla J/S equivalent of jQuery's $.ajax
function ajax(url, callback) { 
	var xhr = new XMLHttpRequest();
	xhr.open('GET', encodeURI(url));
	xhr.onload = function() {
		var data = JSON.parse(xhr.responseText);
		callback(data, xhr.status);			
	};
	xhr.send();
}