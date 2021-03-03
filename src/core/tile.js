import {RES_VACANT, RES_YELLOW, RES_BLUE, RES_RED, RES_GREEN, SIDE_NONE} from './constants.js';

const I = -1; //Invalid shorthand for instantiation
const RC_TO_TID = [ //Note even though there are no tiles in the last row/column, this includes them to avoid undefined errors
	[I, I, I, I, I,  I,  I,  I,  I, I, I],//Row 0
	[I, I, I, I, I,  0,  I,  I,  I, I, I],//Row 1
	[I, I, I, I, I,  I,  I,  I,  I, I, I],//Row 2
	[I, I, I, 1, I,  2,  I,  3,  I, I, I],//Row 3
	[I, I, I, I, I,  I,  I,  I,  I, I, I],//Row 4
	[I, 4, I, 5, I,  6,  I,  7,  I, 8, I],//Row 5
	[I, I, I, I, I,  I,  I,  I,  I, I, I],//Row 6
	[I, I, I, 9, I, 10,  I, 11,  I, I, I],//Row 7
	[I, I, I, I, I,  I,  I,  I,  I, I, I],//Row 8
	[I, I, I, I, I, 12,  I,  I,  I, I, I],//Row 9
	[I, I, I, I, I,  I,  I,  I,  I, I, I],//Row 10
];

var COLOR_CHARS = new Array(5);
COLOR_CHARS[RES_VACANT] = 'V';
COLOR_CHARS[RES_BLUE] = 'B';
COLOR_CHARS[RES_RED] = 'R'
COLOR_CHARS[RES_GREEN] = 'G';
COLOR_CHARS[RES_YELLOW] = 'Y';

export class Tile {
	constructor(color, value) {		
		
		this.color = color;
		this.value = value;				
		
		this.isExhausted = false;
		this.captured = SIDE_NONE;	
	}
	
	static fromString(colVal) {
		var colChar = colVal[0];
		var col;
		switch (colChar) {
			case 'V': col = RES_VACANT; break;
			case 'B': col = RES_BLUE; break;
			case 'R': col = RES_RED; break;
			case 'G': col = RES_GREEN; break;
			case 'Y': col = RES_YELLOW; break;		
			default: throw new Error('Invalid tile char: ' + colChar);
		}
		var val = Number.parseInt(colVal[1]);
		var tile = new Tile(col, val);
		return tile;
	}
	
	toString = () => {		
		return COLOR_CHARS[this.color] + this.value;
	}
}