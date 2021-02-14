import {PLAYER_HUMAN} from '../core/constants.js';

var MENU_PREFIX = 'nar.';


//Struct MenuProperties
function MenuProperties() {	
	this.player1 = PLAYER_HUMAN;
	this.player2 = PLAYER_HUMAN;
	this.showGrid = true;
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
function MenuManager() {
	var PLAYER_OPTIONS = {
		Human:PLAYER_HUMAN, 	
	};	
		
	this.properties = new MenuProperties();
	this.rootMenu = new dat.GUI();	
	
	//Options - secondary root
	var optionsMenu = this.rootMenu.addFolder('Options');			
	optionsMenu.add(this.properties, 'showGrid');//.onChange(this.persistChange);		
	
	//Root menu			
	this.rootMenu.add(this.properties, 'player1', PLAYER_OPTIONS);//.onChange(this.onChangePlayer);
	this.rootMenu.add(this.properties, 'player2', PLAYER_OPTIONS);//.onChange(this.onChangePlayer);
}

//Events
var sayOnce = false;
MenuManager.prototype.onChangePlayer = function(val) {		
	game.players = [parseInt(menu.player1), parseInt(menu.player2)]; //Information hiding - pshaww...		
	game.play();
}

MenuManager.prototype.persistChange = function(val) {
	var propertyName = MENU_PREFIX + this.property;	
	localStorage.setItem(propertyName, val);	
}

var menuManager = new MenuManager();			
var menu = new MenuProperties();
