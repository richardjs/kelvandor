import * as CONST from './constants.js';
import {Tile} from './tile.js';
import {Node} from './node.js';
import {Road} from './road.js';

const WIDTH_BOARD = 5;
const WIDTH_GRID = 11;

const COUNT_TILES = 13;
const COUNT_NODES = 24;
const COUNT_ROADS = 36;

const ITEM_NONE = 0;
const ITEM_TILE = 1;
const ITEM_NODE = 2;
const ITEM_ROAD = 3;

//Shorthand for array initialization
var T = ITEM_TILE; 
var N = ITEM_NODE;
var R = ITEM_ROAD;


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

R = CONST.RES_RED; //Switch to loading state
var B = CONST.RES_BLUE;
var G = CONST.RES_GREEN;
var Y = CONST.RES_YELLOW;
var V = CONST.RES_VACANT;

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
const defaultState = 'R2G1B2R3G2Y2V0G3Y1B3R1B1Y300000000000000000000000000000000000000000000000000000000000001';
export class Board {
	constructor() {
		this.turn = CONST.SIDE_1;
		this.tiles = []
		this.phase = CONST.PHASE_PLACE1_1;
		this.tiles = [];
		this.nodes = [];
		this.roads = [];
		this.scores = [0, 0];
		
		var tileId = 0;		
		for (var r = 0; r < WIDTH_GRID; r++) {
			for (var c = 0; c < WIDTH_GRID; c++) {
				var item = grid[r][c];
				if (item == ITEM_NONE) continue;
				
				if (item == ITEM_TILE) {					
					var resCol = defaultTileCols[r][c];//Math.floor(Math.random()*4)+1;
					var resVal = defaultTileVals[r][c];//Math.floor(Math.random()*3)+1;
					var tile = new Tile(r, c, resCol, resVal);
					this.tiles.push(tile);
					tileId++;
				}				
				else if (item == ITEM_NODE) {
					var node = new Node(r,c, CONST.SIDE_NONE);
					this.nodes.push(node);
				}
				else if (item == ITEM_ROAD) {
					var orientation = this.itemAt(r-1,c) == N || this.itemAt(r+1, c) == N ? CONST.ORIENTATION_VERTICAL : CONST.ORIENTATION_HORIZONTAL;
					var road = new Road(r, c, CONST.SIDE_NONE, orientation);
					this.roads.push(road);
				}
			}
		}
		
	}
	
	itemAt = (r, c) => { //Bounded
		if (r >= 0 && r < WIDTH_GRID && c >= 0 && c < WIDTH_GRID) return grid[r][c];
		else return ITEM_NONE;
	}
	
	shuffle = () => { //Fisher-Yates
		
	}
	
	isGameOver = () => {
		return false;
	}
	
	fromString = (boardStr) => { //Parse
		if (boardStr.length != 88) throw('Invalid board string: ', boardStr);
		var r = 0;
		var c = 0;
		for (var i = 0; i < COUNT_TILES; i+=2) {
			var colVal = boardStr[i] + boardStr[i+1];
			var tile = Tile.fromString(r,c, colVal);
			this.tiles.push(tile);
			c++;			
			if (c > WIDTH_GRID) {
				c = 0;
				r++;
			}
		}
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
		
		return (
			tilesStr + //Tiles
			nodesStr + //Nodes
			roadsStr + //Roads
			0 +   //Phase
			this.turn  //Turn
		
		);
	}
}




