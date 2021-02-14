//import {SIDE_NONE, SIDE_1, SIDE_2} from './constants.js';

export class Road {
	constructor(r, c, side, orientation) {
		
		this.r = r; //Row
		this.c = c; //Column
		this.side = side;
		this.orientation = orientation
		
	}
	
	
	toString = () => {
		return this.side;
	}
}