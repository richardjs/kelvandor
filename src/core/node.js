export class Node {
	constructor(r, c, side) {
		
		this.r = r; //Row
		this.c = c; //Column
		this.side = side;
		
	}
	
	toString = () => {
		return this.side;
	}

}