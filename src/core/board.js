import * as constants from './constants.js';
import * as adj from './adjacent.js';
import {Tile} from './tile.js';
import {Node} from './node.js';
import {Road} from './road.js';

//const WIDTH_BOARD = 5;
const WIDTH_GRID = 11;

const ITEM_NONE = 0;
const ITEM_TILE = 1;
const ITEM_NODE = 2;
const ITEM_ROAD = 3;

const BASE_RES = 16;



//Shorthand for array initialization
var T = ITEM_TILE; 
var N = ITEM_NODE;
var R = ITEM_ROAD;




var V = constants.ORIENTATION_VERTICAL; //Vertical 
const H = constants.ORIENTATION_HORIZONTAL; //Horizontal
const ORIENTATIONS = [H,V,V,H,H,H,V,V,V,V,H,H,H,H,H,V,V,V,V,V,V,H,H,H,H,H,V,V,V,V,H,H,H,V,V,H];

var grid = [
	[0,0,0,0,N,R,N,0,0,0,0],
	[0,0,0,0,R,T,R,0,0,0,0],
	[0,0,N,R,N,R,N,R,N,0,0],
	[0,0,R,T,R,T,R,T,R,0,0],
	[N,R,N,R,N,R,N,R,N,R,N],
	[R,T,R,T,R,T,R,T,R,T,R],
	[N,R,N,R,N,R,N,R,N,R,N],
	[0,0,R,T,R,T,R,T,R,0,0],
	[0,0,N,R,N,R,N,R,N,0,0],
	[0,0,0,0,R,T,R,0,0,0,0],	
	[0,0,0,0,N,R,N,0,0,0,0],	
];

R = constants.RES_RED; //Switch to loading state
var B = constants.RES_BLUE;
var G = constants.RES_GREEN;
var Y = constants.RES_YELLOW;
V = constants.RES_VACANT;


var defaultTileCols = [
	[0,0,0,0,0,0,0,0,0,0,0],
	[0,0,0,0,0,R,0,0,0,0,0],
	[0,0,0,0,0,0,0,0,0,0,0],
	[0,0,0,G,0,B,0,R,0,0,0],
	[0,0,0,0,0,0,0,0,0,0,0],
	[0,G,0,Y,0,V,0,G,0,Y,0],
	[0,0,0,0,0,0,0,0,0,0,0],
	[0,0,0,B,0,R,0,B,0,0,0],
	[0,0,0,0,0,0,0,0,0,0,0],
	[0,0,0,0,0,Y,0,0,0,0,0],	
	[0,0,0,0,0,0,0,0,0,0,0],
];

var defaultTileVals = [
	[0,0,0,0,0,0,0,0,0,0,0],
	[0,0,0,0,0,2,0,0,0,0,0],
	[0,0,0,0,0,0,0,0,0,0,0],
	[0,0,0,1,0,2,0,3,0,0,0],
	[0,0,0,0,0,0,0,0,0,0,0],
	[0,2,0,2,0,0,0,3,0,1,0],
	[0,0,0,0,0,0,0,0,0,0,0],
	[0,0,0,3,0,1,0,1,0,0,0],
	[0,0,0,0,0,0,0,0,0,0,0],
	[0,0,0,0,0,3,0,0,0,0,0],	
	[0,0,0,0,0,0,0,0,0,0,0],
];



export const DEFAULT_BOARD_STATE = 'R2G1B2R3G2Y2V0G3Y1B3R1B1Y3222222222222222222222222222222222222222222222222222222222222000000000000000000';
export class Board {
	constructor() {			
		this.turn = constants.SIDE_1;
		this.phase = constants.PHASE_PLACE1_1;// PHASE_PLAY;		
		this.tiles = [];
		this.nodes = [];
		this.roads = [];
		this.scores = [0, 0];
		this.res = [[4,4,2,2],[4,4,2,2]]; 
		this.hasAlreadyTraded = false;		
			
	}
	
	init = () => {				
		for (var r = 0; r < WIDTH_GRID; r++) {
			for (var c = 0; c < WIDTH_GRID; c++) {
				var item = grid[r][c];
				if (item == ITEM_NONE) continue;
				
				if (item == ITEM_TILE) {					
					var resCol = defaultTileCols[r][c];
					var resVal = defaultTileVals[r][c];
					var tile = new Tile(resCol, resVal);
					this.tiles.push(tile);					
					
				}				
				else if (item == ITEM_NODE) {
					var node = new Node(constants.SIDE_NONE);
					this.nodes.push(node);
				}
				else if (item == ITEM_ROAD) {		
					var orientation = ORIENTATIONS[this.roads.length];
					var road = new Road(constants.SIDE_NONE, orientation);
					this.roads.push(road);
				}
			}
		}		
		//console.log(this.toString());		
	}
	
	shuffle = () => { //Fisher-Yates
		
		var tmp;
		var index;
	  
		for (var n = 0; n < constants.COUNT_TILES; n++) {

			//Choose random
			index = Math.floor(Math.random() * n);			

			//Swap
			tmp = this.tiles[n];
			this.tiles[n] = this.tiles[index];
			this.tiles[index] = tmp;
		}
  
	}
	
	isGameOver = () => {
		if (this.scores[constants.SIDE_1] >= constants.SCORE_WIN) return true;
		else if (this.scores[constants.SIDE_2] >= constants.SCORE_WIN) return true;
		else return false;
	}
	
	static fromString = (boardStr) => { //Parse
		if (boardStr.length < 88) throw('Invalid board string: ', boardStr);
		
		var board = new Board();				
		var s = 0;
		
		//Tiles
		for (var i = 0; i < constants.COUNT_TILES; i++) {
			var colVal = boardStr[s] + boardStr[s+1];			
			var tile = Tile.fromString(colVal);
			board.tiles.push(tile);
						
			s+=2;
		}
		
		
		//Nodes
		for (var i = 0; i < constants.COUNT_NODES; i++) {
			var side = constants.PROTOCOL_TO_SIDES[boardStr[s]];
			var node = new Node(side);
			board.nodes.push(node);					
			s++;
		}
		
		//Roads
		for (var i = 0; i < constants.COUNT_ROADS; i++) {			
			var side = constants.PROTOCOL_TO_SIDES[boardStr[s]];
			var orientation = ORIENTATIONS[board.roads.length];
			var road = new Road(side, orientation);
			board.roads.push(road);
						
			s++;
		}
		
		//Phase
		//board.phase = boardStr[s];
		//s++;
		
		//Turn 
		board.turn = boardStr[s];
		s++;
		
		//Resources			
		for (var i = 0; i < constants.COUNT_RES; i++) {
			var resCount = Number.parseInt(boardStr.substr(s,2), BASE_RES);
			board.res[constants.SIDE_1][i] = resCount //Hexadecimal counts			
			s += 2;
		}
		for (var i = 0; i < constants.COUNT_RES; i++) {		
			var resCount = Number.parseInt(boardStr.substr(s,2), BASE_RES);	
			board.res[constants.SIDE_2][i] = resCount //Hexadecimal counts
			s += 2;
		}
		
		//Traded Status
		var tradedStatus = boardStr[s+1];
		board.hasAlreadyTraded = tradedStatus? true : false;
		
		//Calculate score, other attributes?
		return board;
	}
	
	toString = () => {
		//Tiles
		var tilesStr = '';
		for (var i = 0; i < this.tiles.length; i++) {
			tilesStr += this.tiles[i].toString();
		}
		
		//Nodes
		var nodesStr = '';
		for (var i = 0; i < this.nodes.length; i++) {
			nodesStr += this.nodes[i].toString();
		}
		
		//Roads
		var roadsStr = '';
		for (var i = 0; i < this.roads.length; i++) {
			roadsStr += this.roads[i].toString();
		}
		
		//Resources
		var resStr1 = '';		
		var resStr2 = '';
		for (var i = 0; i < constants.COUNT_RES; i++) {
			resStr1 += this.res[constants.SIDE_1][i].toString(BASE_RES).padStart(2, '0');			
			resStr2 += this.res[constants.SIDE_2][i].toString(BASE_RES).padStart(2, '0');			
		}

		//Already traded
		var tradedStatusStr = this.hasAlreadyTraded? '1' : 0;
				
		
		return (
			tilesStr + 		//Tiles
			nodesStr + 		//Nodes
			roadsStr + 		//Roads
			//this.phase +   //Phase
			constants.SIDES_TO_PROTOCOL[this.turn] + 	//Turn
			resStr1  +		//Player 1 resources
			resStr2  +		//Player 2 resources
			tradedStatusStr //Already traded
		).toLowerCase();
	}
	
	addNode = (nid) => {		
		var side = this.turn;		
		if (!nidInBounds(nid)) return {status:false, msg:'Node out of bounds:' + nid + '...'};
		else if (this.nodes[nid].side != constants.SIDE_NONE) return {status:false, msg:'There is already a node there...'};
		
		//PLACE Player 1 - first
		if (this.phase == constants.PHASE_PLACE1_1) {
			if (this.res[side][constants.RES_GREEN] == 4 && this.res[side][constants.RES_YELLOW] == 4) { //Check if they have the resources to build exactly 2 nodes
				this.buyNode(side, nid);
				return {status:true, msg:'Node placed...'};
			}
			else return {status:false, msg:'Road must be placed next...'};
		}

		//PLACE Player 2 - first
		else if (this.phase == constants.PHASE_PLACE2_1) {
			if (this.res[side][constants.RES_GREEN] == 4 && this.res[side][constants.RES_YELLOW] == 4) {
				this.buyNode(side, nid);
				return {status:true, msg:'Node placed...'};
			}
			else return {status:false, msg:'Road must be placed next...'};			
		}

		//PLACE Player 2 - second
		else if (this.phase == constants.PHASE_PLACE2_2) {
			if (this.res[side][constants.RES_BLUE] != 1 || this.res[side][constants.RES_RED] != 1) return {status:false, msg:'Unable to place node...'};
			else if (this.res[side][constants.RES_GREEN] == 2 && this.res[side][constants.RES_YELLOW] == 2) {
				this.buyNode(side, nid);
				return {status:true, msg:'Node placed...'};
			}
			else return {status:false, msg:'Unable to place node...'};
		}

		//PLACE Player 1 - second
		else if (this.phase == constants.PHASE_PLACE1_2) {
			if (this.res[side][constants.RES_BLUE] != 1 || this.res[side][constants.RES_RED] != 1) return {status:false, msg:'Unable to place node...'};
			else if (this.res[side][constants.RES_GREEN] == 2 && this.res[side][constants.RES_YELLOW] == 2) {
				this.buyNode(side, nid);
				return {status:true, msg:'Node placed...'};
			}
			else return {status:false, msg:'Unable to place node...'};
		}

		else if (this.phase != constants.PHASE_PLAY) return {status:false, msg:'Not able to add node...'};
		

		//PLAY Phase
		if (!this.canAffordNode(side)) return {status: false, msg: 'Not enough resources to add a node...'};

		if (!this.isNodeAdjacentToOwnRoad(side, nid)) return {status:false, msg:'Node must be adjacent to another road...'};

		//Add node
		this.buyNode(side, nid);
		

		//Exhaust tiles
		var tids = adj.TILES_ADJ_NODE[nid];		
		for (var t = 0; t < tids.length; t++) {
			var tid = tids[t];			
			var tile = this.tiles[tid];
			if (tile.captured != constants.SIDE_NONE) continue;//Don't exhaust if captured
			else if (tile.color == constants.RES_VACANT) continue;			
			var nodeCount = this.countNodesOnTile(tid);			
			if (nodeCount > tile.value) this.tiles[tid].isExhausted = true;
			
		}
		
		return {status:true, msg:'Node added...'};
	}
	
	addRoad = (rid) => {		
		var side = this.turn;

		if (!ridInBounds(rid)) return {status: false, msg:'Road ' + rid + ' is out of bounds...'};
		else if (this.roads[rid].side != constants.SIDE_NONE) return {status: false, msg: 'There is already a road there...'};
		
		//PLACE Player 1 - first
		if (this.phase == constants.PHASE_PLACE1_1) {
			if (!this.isRoadAdjacentToOwnNode(side, rid)) return {status:false, msg:'Road must be adjacent to own node...'};
			else if (this.res[side][constants.RES_BLUE] == 2 && this.res[side][constants.RES_RED] == 2) {
				this.buyRoad(side, rid);
				this.turn = constants.SIDE_2;
				this.phase = constants.PHASE_PLACE2_1;
				return {status:true, msg:'Road placed...'};
			} 
			else return {status:false, msg:'Unable to place road...'};
						
		}

		//PLACE Player 2 - first
		else if (this.phase == constants.PHASE_PLACE2_1) {
			if (!this.isRoadAdjacentToOwnNode(side, rid)) return {status:false, msg:'Road must be adjacent to own node...'};
			else if (this.res[side][constants.RES_BLUE] == 2 && this.res[side][constants.RES_RED] == 2) {
				this.buyRoad(side, rid);		
				this.phase = constants.PHASE_PLACE2_2;
				return {status:true, msg:'Road placed...'};
			} 
			else return {status:false, msg:'Unable to place road...'};
		}

		//PLACE Player 2 - second
		else if (this.phase == constants.PHASE_PLACE2_2) {
			if (this.res[side][constants.RES_GREEN] != 0 || this.res[side][constants.RES_YELLOW] != 0) return {status:false, msg:'Second node must be placed first...'};
			else if (!this.isRoadAdjacentToOwnNode(side, rid)) return {status:false, msg:'Road must be adjacent to own node'};
			else if (this.res[side][constants.RES_BLUE] == 1 && this.res[side][constants.RES_RED] == 1) {
				this.buyRoad(side, rid);
				this.turn = constants.SIDE_1;
				this.phase = constants.PHASE_PLACE1_2;
				return {status:true, msg:'Road placed...'};
			}
			else return {status:false, msg:'Unable to place road...'}; 
		}

		//PLACE Player 1 - second
		else if (this.phase == constants.PHASE_PLACE1_2) {
			if (this.res[side][constants.RES_GREEN] != 0 || this.res[side][constants.RES_YELLOW] != 0) return {status:false, msg:'Second node must be placed first...'};
			else if (!this.isRoadAdjacentToOwnNode(side, rid)) return {status:false, msg:'Road must be adjacent to own node'};
			else if (this.res[side][constants.RES_BLUE] == 1 && this.res[side][constants.RES_RED] == 1) {
				this.buyRoad(side, rid);
				this.turn = constants.SIDE_2;
				this.harvest(this.turn);
				this.phase = constants.PHASE_PLAY;
				return {status:true, msg:'Road placed...'};
			}
			else return {status:false, msg:'Unable to place road...'};
		}
		else if (this.phase != constants.PHASE_PLAY) return {status:false, msg:'Not able to add node...'};

		//PLAY phase
		if (!this.canAffordRoad(side)) return {status:false, msg:'Not enough resources to add road...'};

		if (!this.isRoadAdjacentToOwnRoad(side, rid)) return {status:false, msg:'Road must be adjacent to another road...'};

		//Add road
		this.buyRoad(side, rid);


		//Check for capture
		var tids = adj.TILES_ADJ_ROAD[rid];
		for (var t = 0; t < tids.length; t++) {
			var tid = tids[t];			
			var tile = this.tiles[tid];
					
			var roadCount = this.countRoadsOnTile(tid, this.turn);					
			if (roadCount == 4) {
				this.tiles[tid].captured = this.turn;
				this.tiles[tid].isExhausted = false; //Un-exhaust if captured
			}
			
		}
				
		return {status:true, msg:'Road added'};
	}

	buyNode = (side, nid) => {
		this.res[side][constants.RES_GREEN]-=2;
		this.res[side][constants.RES_YELLOW]-=2;
		this.nodes[nid].side = side;
	}

	buyRoad = (side, rid) => {
		this.res[side][constants.RES_BLUE]--;
		this.res[side][constants.RES_RED]--;
		this.roads[rid].side = side;
	}
	
	changeTurn = () => {

		var side = this.turn;
		if (this.phase != constants.PHASE_PLAY) return {status:false, msg:'Turn can not be changed until the play phase...'};		
		if (this.scores[this.turn] >= constants.SCORE_WIN) return {status:false, msg:'Game Over'};
		
		//Change turn
		this.calcScores();		
		if (this.isGameOver()) {
			alert('Game Over!');
			return {status:false, msg:'Game Over!'};
		}
		this.turn = (this.turn == constants.SIDE_1)? constants.SIDE_2 : constants.SIDE_1;
		this.hasAlreadyTraded = false;
		this.harvest(this.turn);
		return {status:true, msg:'Changing Player...'};
		
	}

	countNodesOnTile = (tid) => {
		var count = 0;
		var nids = adj.NODES_ADJ_TILE[tid];
		for (var n = 0; n < constants.COUNT_NODES_PER_TILE; n++) {
			var nid = nids[n];
			if (this.nodes[nid].side != constants.SIDE_NONE) count++;			
		}
		return count;
	}
	
	countRoadsOnTile = (tid, side) => {
		var count = 0;
		var rids = adj.ROADS_ADJ_TILE[tid];
		for (var r = 0; r < rids.length; r++) {
			var rid = rids[r];
			if (this.roads[rid].side == side) count++;			
		}
		return count;
	}

	
	
	harvest = (side) => {
		
		//Update resources for each element held
		for (var tid = 0; tid < this.tiles.length; tid++) {
			var tile = this.tiles[tid];			
			
			if (tile.isExhausted) continue; //Exhausted
			
			var nids = adj.NODES_ADJ_TILE[tid];
			for (var n = 0; n < constants.COUNT_NODES_PER_TILE; n++) {
				var nid = nids[n];
				var node = this.nodes[nid];				
				if (node.side == side) {
					this.res[node.side][tile.color]++;					
				}
			}
			
		}
	}
	
	
	calcScores = () => {
		var roadLen1 = this.calcLongestRoad(constants.SIDE_1);
		var roadLen2 = this.calcLongestRoad(constants.SIDE_2);

		var longestRoad;
		if (roadLen1 > roadLen2) longestRoad = constants.SIDE_1;
		else if (roadLen1 < roadLen2) longestRoad = constants.SIDE_2;
		else longestRoad = constants.SIDE_NONE;


		this.calcScore(constants.SIDE_1, longestRoad);
		this.calcScore(constants.SIDE_2, longestRoad);
	}

	calcScore = (side, longestRoad) => {
		var score = 0;
		//+1 per each node		
		for (var nid = 0; nid < constants.COUNT_NODES; nid++) {
			var node = this.nodes[nid];
			if (node.side == side) score++;
		}
		
		//+1 each captured tile
		for (var tid = 0; tid < constants.COUNT_TILES; tid++) {
			var tile = this.tiles[tid];
			if (tile.captured == side) score++;
		}
			
		//+2 longestRoad
		if (longestRoad == side) score += 2;

		this.scores[side] = score;
	}
	
	calcLongestRoad = (side) => {
		var queue = [];
		var visited = {};
		var maxLength = 0;
		//Loop through all rids
		for (var rid = 0; rid < constants.COUNT_ROADS; rid++) {
			if (!visited[rid] && this.roads[rid].side == side) { 
				var length = 1;
				visited[rid] = true;				
				//BFS follow road to find length
				queue.push(rid);
				while (queue.length) {
					//Remove front
					var frontRid = queue.shift();
					var adjRids = adj.ROADS_ADJ_ROAD[frontRid];
					for (var i = 0; i < adjRids.length; i++) {
						var adjRid = adjRids[i];
						if (!visited[adjRid] && this.roads[adjRid].side == side) {
							visited[adjRid] = true;
							length++;
							queue.push(adjRid);
						}
					}
					
				}

				//Finished BFS road length search
				if (length > maxLength) maxLength = length;				
			}
		
		}
		
		return maxLength;
	}

	trade = (tradeResids) => {		
		if (this.hasAlreadyTraded) return {status:false, msg:'Player has already traded once'};
		var side = this.turn;
		this.res[side][tradeResids[0]]--;
		this.res[side][tradeResids[1]]--;
		this.res[side][tradeResids[2]]--;
		this.res[side][tradeResids[3]]++;
		this.hasAlreadyTraded = true;
		return {status:true, msg:'Resources traded...'};

	}

	playActions = (actions) => { //Actions is an array of KMN action strings
		console.log(actions);
		for (var a = 0; a < actions.length; a++){
			var action = actions[a].toUpperCase();
			var actionChar = action[0];

			//Start
			if (actionChar == 'S') { 
				var nid = Number.parseInt(action.substr(1,2));
				var rid = Number.parseInt(action.substr(3));
				var actionAddNode = this.addNode(nid);				
				if (actionAddNode.status) {
					console.log('Added node', nid);
					var actionAddRoad = this.addRoad(rid);	
					if (!actionAddRoad.status) return actionAddRoad;	
					else console.log('Added road', rid);				
				}
				else return actionAddNode;
			}

			//Trade
			else if (actionChar == 'T') {
				var tradeResids = [
					residFromChar(action[1]), 
					residFromChar(action[2]), 
					residFromChar(action[3]),
					residFromChar(action[4])
				];
				var tradeAction = this.trade(tradeResids);
				if (!tradeAction.status) return tradeAction;
				else console.log('traded', tradeResids);
			}

			//Build node
			else if (actionChar == 'N') {
				var nid = Number.parseInt(action.substr(1,2));
				var actionAddNode = this.addNode(nid);
				if (!actionAddNode.status) return actionAddNode;
				else console.log('Added node', nid);
			}

			//Build branch (road)
			else if (actionChar == 'B') {
				var rid = Number.parseInt(action.substr(1));
				var actionAddRoad = this.addRoad(rid);	
					if (!actionAddRoad.status) return actionAddRoad;
					else console.log('Added road', rid);
			}

			//End
			else if (actionChar == 'E') {
				return this.changeTurn();				
			}

			else return {status:false, msg:'Unrecognized action: ' + actionChar};
		}
		return {status:true, msg:''};
	}

	canAffordNode = (side) => {
		if (this.res[side][constants.RES_GREEN] >= 2 && this.res[side][constants.RES_YELLOW] >= 2) return true;
		else return false;
	}

	canAffordRoad = (side) => {
		if (this.res[side][constants.RES_BLUE] >= 1 && this.res[side][constants.RES_RED] >= 1) return true;
		else return false;
	}



	isRoadAdjacentToOwnNode = (side, rid) => {
		var adjNids = adj.NODES_ADJ_ROAD[rid];
		for (var n = 0; n < adjNids.length; n++ ){
			var nid = adjNids[n];
			if (this.nodes[nid].side == side) return true;
		}		
		return false;		
	}

	isRoadAdjacentToOwnRoad = (side, rid) => {
		var adjRids = adj.ROADS_ADJ_ROAD[rid];
		for (var r = 0; r < adjRids.length; r++ ){
			var rid = adjRids[r];
			if (this.roads[rid].side == side) return true;
		}		
		return false;		
	}

	isNodeAdjacentToOwnRoad = (side, nid) => {
		var adjRids = adj.ROADS_ADJ_NODE[nid];
		for (var r = 0; r < adjRids.length; r++ ){
			var rid = adjRids[r];
			if (this.roads[rid].side == side) return true;
		}		
		return false;		
	}
}

let nidInBounds = (nid) => {
	if (nid >= 0 && nid < constants.COUNT_NODES) return true;
	else return false;
}

let ridInBounds = (rid) => {
	if (rid >= 0 && rid < constants.COUNT_ROADS) return true;
	else return false;
}

let itemAt = (r, c) => { //Bounded
	if (r >= 0 && r < WIDTH_GRID && c >= 0 && c < WIDTH_GRID) return grid[r][c];
	else return ITEM_NONE;
}

let residFromChar = (c) => {
	switch(c) {
		case 'B': return constants.RES_BLUE;
		case 'G': return constants.RES_GREEN;
		case 'Y': return constants.RES_YELLOW;
		case 'R': return constants.RES_RED;
		default: throw new Error('Invalid color char' + c);
	}
}