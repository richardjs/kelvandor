const URL = 'http://127.0.0.1:5000/'; //For other domains add "Access-Control-Allow-Origin: *" to the header

const MOVE_SCAN_ITERATIONS = 100000;

	
//var playerId = +(new Date()); //Timestamp
	
export function getMove(boardStr, iterations, onPlayed) {
	var url = URL + boardStr;
	
	ajax(url, function(data, status) {
		//Optional argument to log info 
		if (data.hasOwnProperty('log') && data.log) {
			console.log(data.log);

			if (menu.valueAlert) {
				var lines = data.log.split('\n');
				for (var i = 0; i < lines.length; i++) {
					if (lines[i].startsWith('e\t') || lines[i].match('s\\d\\d\\d\\d\t')) {
						// Only alert for the second start place, if P2 start turn
						if (i + 1 < lines.length && lines[i + 1].match('s\\d\\d\\d\\d\t')) {
							continue;
						}

						var value = Number(lines[i].split('\t')[1]);
						console.log(value);
						if (value >= menu.alertThreshold) {
							alert('Value: ' + value);
						}
					}
				}
			}
		}
		
		//Expect json data that has an array of kmn action strings
		var actions = data.actions;
		if (actions) return onPlayed(actions);		
		else alert ('Invalid move!');
	}, {'Iterations': iterations});
}	

export function getMoveScan(boardStr, iterations, onMoveScan) {
	var url = URL + boardStr;

	ajax(url, function(data, status) {
		onMoveScan(data);
	}, {'Iterations': MOVE_SCAN_ITERATIONS, 'Mode': 'move scan'});
}
			
//Vanilla J/S equivalent of jQuery's $.ajax
function ajax(url, callback, headers) {
	var xhr = new XMLHttpRequest();
	xhr.open('GET', encodeURI(url));

	if (typeof(headers) != 'undefined') {
		for (var header in headers) {
			xhr.setRequestHeader(header, headers[header]);
		}
	}

	xhr.onload = function() {
		var data = JSON.parse(xhr.responseText);
		callback(data, xhr.status);			
	};
	xhr.send();
}