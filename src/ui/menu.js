export const PLAYER_HUMAN = 0;
export const PLAYER_KELVANDOR = 1;

var MENU_PREFIX = 'nar.';
//Struct MenuProperties
function MenuProperties() {	
	this.player1 = 0;
	this.player2 = 0;
	this.showLongest = this.getDefault('showLongest', false);
	this.scoreInfo = this.getDefault('scoreInfo', false);
	this.shuffle = onShuffle;
	this.reset = onReset;
	this.play = onPlay;
	this.iterations = this.getDefault('iterations', 1000);
	this.valueAlert = this.getDefault('valueAlert', false);
	this.alertThreshold = this.getDefault('alertThreshold', .9);
	this.rules = function() {window.location = 'http://qndgames.com/wp-content/uploads/2018/10/Node-Manual-V4.55-Rule-Sheet-Website-Full-copy.pdf'; }	
}


MenuProperties.prototype.getDefault = function(propertyName, defaultValue) {
	propertyName = MENU_PREFIX + propertyName;
	if (localStorage.getItem(propertyName) !== null) {
		var val = localStorage.getItem(propertyName);
		if (val == 'true') return true;
		else if (val == 'false') return false;
		else return val;
	}
	else return defaultValue;
}
//End struct MenuProperties

//Class MenuManager
export function MenuManager() {
	var PLAYER_OPTIONS = {
		Human:PLAYER_HUMAN,
		Kelvandor:PLAYER_KELVANDOR
	};		
	this.properties = new MenuProperties();
	this.rootMenu = new dat.GUI();	
		

	//Options - secondary root
	var optionsMenu = this.rootMenu.addFolder('Options');			
	optionsMenu.add(this.properties, 'showLongest').onChange(this.persistChange);	
	optionsMenu.add(this.properties, 'scoreInfo').onChange(this.persistChange);	
	optionsMenu.add(this.properties, 'iterations').onChange(this.persistChange);
	optionsMenu.add(this.properties, 'valueAlert').onChange(this.persistChange);
	optionsMenu.add(this.properties, 'alertThreshold').onChange(this.persistChange);
		
	//Links menu
	var linksMenu = this.rootMenu.addFolder('Links');			
	linksMenu.add(this.properties, 'rules');

	//Root menu			
	this.rootMenu.add(this.properties, 'player1', PLAYER_OPTIONS).onChange(this.onChangePlayer); this.rootMenu.add(this.properties, 'player2', PLAYER_OPTIONS).onChange(this.onChangePlayer);
	this.rootMenu.add(this.properties, 'shuffle');
	this.rootMenu.add(this.properties, 'reset');
	//this.rootMenu.add(this.properties, 'play');
}

//Events
MenuManager.prototype.persistChange = function(val) {
	var propertyName = MENU_PREFIX + this.property;	
	localStorage.setItem(propertyName, val);	
	document.dispatchEvent(new KeyboardEvent('keydown',{'key':'f'})); 
}

MenuManager.prototype.onChangePlayer = function(val) {				
	document.dispatchEvent(new KeyboardEvent('keydown',{'key':'p'})); 
}


function onShuffle() {
	document.dispatchEvent(new KeyboardEvent('keydown',{'key':'s'})); 
}

function onReset() {
	document.dispatchEvent(new KeyboardEvent('keydown',{'key':'n'})); 
}

function onPlay() {
	document.dispatchEvent(new KeyboardEvent('keydown',{'key':'p'})); 
}
