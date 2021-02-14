import {RES_VACANT, RES_YELLOW, RES_BLUE, RES_RED, RES_GREEN} from './constants.js';

var COLOR_CHARS = new Array(5);
COLOR_CHARS[RES_VACANT] = 'V';
COLOR_CHARS[RES_BLUE] = 'B';
COLOR_CHARS[RES_RED] = 'R'
COLOR_CHARS[RES_GREEN] = 'G';
COLOR_CHARS[RES_YELLOW] = 'Y';

export class Tile {
	constructor(r, c, resCol, resVal) {
		
		this.r = r; //Row
		this.c = c; //Column
		this.resCol = resCol;
		this.resVal = resVal;				
		this.isExhausted = false;
		this.isCaptured = false;	
	}
	
	static fromString(r, c, colVal) {
		var colChar = colVal[0];
		var col;
		switch (colChar) {
			case 'V': col = RES_VACANT; break;
			case 'B': col = RES_BLUE; break;
			case 'R': col = RES_RED; break;
			case 'G': col = RES_GREEN; break;
			case 'Y': col = RES_YELLOW; break;			
		}
		var val = Number.parseInt(colVal[1]);
		var tile = new Tile(r, c, col, val);
		return tile;
	}
	
	toString = () => {		
		return COLOR_CHARS[this.resCol] + this.resVal;
	}
}