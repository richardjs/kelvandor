const I = -1; //Invalid shorthand for instantiation
const RC_TO_NID = [
	[ I,  I,  I,  I,  0,  I,  1,  I,  I,  I,   I], 	//Row 0
	[ I,  I,  I,  I,  I,  I,  I,  I,  I,  I,   I], 	//Row 1
	[ I,  I,  2,  I,  3,  I,  4,  I,  5,  I,   I],	//Row 2
	[ I,  I,  I,  I,  I,  I,  I,  I,  I,  I,   I], 	//Row 3
	[ 6,  I,  7,  I,  8,  I,  9,  I, 10,  I,  11], 	//Row 4
	[ I,  I,  I,  I,  I,  I,  I,  I,  I,  I,   I], 	//Row 5
	[12,  I, 13,  I, 14,  I, 15,  I, 16,  I,  17], 	//Row 6
	[ I,  I,  I,  I,  I,  I,  I,  I,  I,  I,   I],	//Row 7
	[ I,  I, 18,  I, 19,  I, 20,  I, 21,  I,   I],	//Row 8
	[ I,  I,  I,  I,  I,  I,  I,  I,  I,  I,   I],	//Row 9
	[ I,  I,  I,  I, 22,  I, 23,  I,  I,  I,   I],	//Row 10
];

export class Node {
	constructor(side) {
				
		this.side = side;
		
	}	
	
	toString = () => {
		return this.side;
	}

}