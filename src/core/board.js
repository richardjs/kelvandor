import * as constants from './constants.js';
import * as adj from './adjacent.js';
import {Tile} from './tile.js';
import {Node} from './node.js';
import {Road} from './road.js';

const WIDTH_BOARD = 5;
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
		this.phase = constants.PHASE_PLAY;		
		this.tiles = [];
		this.nodes = [];
		this.roads = [];
		this.scores = [0, 0];
		this.res = [[4,4,2,2,],[4,4,2,2]]; 
			
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
		return false;
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
			var side = Number.parseInt(boardStr[s]);			
			var node = new Node(side);
			board.nodes.push(node);					
			s++;
		}
		
		//Roads
		for (var i = 0; i < constants.COUNT_ROADS; i++) {			
			var side = Number.parseInt(boardStr[s]);
			var orientation = ORIENTATIONS[board.roads.length];
			var road = new Road(side, orientation);
			board.roads.push(road);
						
			s++;
		}
		
		//Phase
		board.phase = boardStr[s];
		s++;
		
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
				
		
		return (
			tilesStr + 		//Tiles
			nodesStr + 		//Nodes
			roadsStr + 		//Roads
			this.phase +   //Phase
			this.turn + 	//Turn
			resStr1  +		//Player 1 resources
			resStr2  		//Player 2 resources
		);
	}
	
	addNode = (nid) => {		
		var side = this.turn;		
		if (!nidInBounds(nid)) return false;
		else if (this.nodes[nid].side != constants.SIDE_NONE) return false;
		

		if (!this.canAffordNode(side)) return false;

		//Add node
		this.nodes[nid].side = this.turn; 
		
		//Pay for node
		this.res[side][constants.RES_GREEN]-=2;
		this.res[side][constants.RES_YELLOW]-=2;
		

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
		
		return true;
	}
	
	addRoad = (rid) => {		
		var side = this.turn;

		if (!ridInBounds(rid)) return false;
		else if (this.roads[rid].side != constants.SIDE_NONE) return false;
		else if (!this.canAffordRoad(side)) return false;

		//Add road
		this.roads[rid].side = this.turn;
		
		//Pay for road
		this.res[side][constants.RES_BLUE]--;
		this.res[side][constants.RES_RED]--;


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
				
		return true;
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

	
	
	harvest = () => {
		var side = this.turn;
		//Update resources for each element held
		for (var tid = 0; tid < this.tiles.length; tid++) {
			var tile = this.tiles[tid];
			//if (tile.isCaptured) this.res[side][tile.color] += tile.value; //Captured
			
			if (tile.isExhausted) continue; //Exhausted
			
			var nids = adj.NODES_ADJ_TILE[tid];
			for (var n = 0; n < constants.COUNT_NODES_PER_TILE; n++) {
				var nid = nids[n];
				var node = this.nodes[nid];				
				if (node.side != constants.SIDE_NONE) {
					this.res[node.side][tile.color]++;					
				}
			}
			
		}
	}
	
	changeTurn = () => {
		this.calcScore(constants.SIDE_1);
		this.calcScore(constants.SIDE_2);
		if (this.scores[this.turn] >= constants.SCORE_WIN) {
			alert('Game Over');
		}
		else {
			this.turn = (this.turn == constants.SIDE_1)? constants.SIDE_2 : constants.SIDE_1;
			this.harvest();
		}
	}
	
	calcScore = (side) => {
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
		
		this.scores[side] = score;
	}
	
	calcLongest = () => {
		
	}

	trade = (tradeResids) => {
		//console.log(tradeResids);
		var side = this.turn;
		this.res[side][tradeResids[0]]--;
		this.res[side][tradeResids[1]]--;
		this.res[side][tradeResids[2]]--;
		this.res[side][tradeResids[3]]++;
		return true;

	}

	canAffordNode = (side) => {
		if (this.res[side][constants.RES_GREEN] >= 2 && this.res[side][constants.RES_YELLOW] >= 2) return true;
		else return false;
	}

	canAffordRoad = (side) => {
		if (this.res[side][constants.RES_BLUE] >= 1 && this.res[side][constants.RES_RED] >= 1) return true;
		else return false;
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
