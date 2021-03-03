import {ORIENTATION_VERTICAL, ORIENTATION_HORIZONTAL} from './constants.js';

const I = -1; //Invalid shorthand for instantiation
const V = ORIENTATION_VERTICAL; //Vertical 
const H = ORIENTATION_HORIZONTAL; //Horizontal


const RC_TO_RID = [
	[ I,  I,  I,  I,  I,  0,  I,  I,  I,  I,  I], 	//Row 0
	[ I,  I,  I,  I,  1,  I,  2,  I,  I,  I,  I], 	//Row 1
	[ I,  I,  I,  3,  I,  4,  I,  5,  I,  I,  I],	    //Row 2
	[ I,  I,  6,  I,  7,  I,  8,  I,  9,  I,  I], 	//Row 3
	[ I, 10,  I, 11,  I, 12,  I, 13,  I, 14,  I], 	//Row 4
	[15,  I, 16,  I, 17,  I, 18,  I, 19,  I, 20], 	//Row 5
	[ I, 21,  I, 22,  I, 23,  I, 24,  I, 25,  I], 	//Row 6
	[ I,  I, 26,  I, 27,  I, 28,  I, 29,  I,  I],		//Row 7
	[ I,  I,  I, 30,  I, 31,  I, 32,  I,  I,  I],		//Row 8
	[ I,  I,  I,  I, 33,  I, 34,  I,  I,  I,  I],		//Row 9
	[ I,  I,  I,  I,  I, 35,  I,  I,  I,  I,  I],		//Row 10
];



export class Road {
	constructor(side, orientation) {
			
		this.side = side;
		this.orientation = orientation;
	}
	
	
	toString = () => {
		return this.side;
	}
}