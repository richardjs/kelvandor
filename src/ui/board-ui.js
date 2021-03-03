import * as constants from '../core/constants.js';
import {UNIT_TILE, UNIT_NODE, UNIT_ROAD, RES_COLORS, SIDE_COLORS} from './constants-ui.js';
import {Board, DEFAULT_BOARD_STATE} from '../core/board.js';


import { html, Component } from '../../lib/preact.js';
import { TileUI } from './tile-ui.js';
import { NodeUI } from './node-ui.js';
import { RoadUI } from './road-ui.js';
import { ResUI } from './res-ui.js';
import { ScoreUI } from './score-ui.js';
import { TurnUI } from './turn-ui.js';

const KEY_ENTER = 13;



export class BoardUI extends Component {
	
	constructor() {
		super();
		document.addEventListener('keydown', this.onKeyDown);
		document.addEventListener('contextmenu', this.onRightClick);		    
		
		//this.board = Board.fromString(DEFAULT_BOARD_STATE);
		this.board = new Board();
		this.board.init();
	}
		
	
	//Events		
	onNodeClick = (nid) => {
		if (this.board.addNode(nid)) {
			//this.gameEvents[EVENT_NODE_ADDED](nid, this.board.turn);
			//window.location.hash = this.board.toString();
			this.forceUpdate();	
		}
		else {
			alert('Unable to add node');
			//this.gameEvents[EVENT_MESSAGE]('Invalid Node');
		}		
	}
	
	onRoadClick = (rid) => {
		if (this.board.addRoad(rid)) {			
			this.forceUpdate();	
		}
		else {
			alert('unable to add road');
			//this.gameEvents[EVENT_MESSAGE]('Invalid Node');
		}		
	}
	
	onKeyDown = (e) => {
		if (e.keyCode == KEY_ENTER) {
			this.board.changeTurn();
		}

	}
	
	onRightClick = (e) => {
		//e.preventDefault();
	}

	
	onShuffle = (e) => {
		this.board.shuffle();
		this.forceUpdate();		
	}

	onHarvest = (e) => {
		this.board.harvest();
		this.forceUpdate();		
	}
	
	onChangeTurn = (e) => {
		this.board.changeTurn();
		var boardStr = this.board.toString().toLowerCase();
		console.log('nar', boardStr);
		
		let state = new kelvandor.State();
		state.fromString(boardStr);
		console.log('kel', state.toString());			
		
		 
		state.delete();
		this.forceUpdate();		
	}
	
	

	onTrade = (tradeResids) => {
		
		if (this.board.trade(tradeResids)) {

		}
		//else alert('invalid trade');
	}
	
	//Rendering methods
	renderTiles = () => {
		var tileUIs = [];		
		
		for (var tid = 0; tid < this.board.tiles.length; tid++){
			var tile = this.board.tiles[tid];
			var x = constants.TID_TO_C[tid] * UNIT_TILE;
			var y = constants.TID_TO_R[tid] * UNIT_TILE;
			var color = tile.color;
			var value = tile.value;
			var exhausted = tile.isExhausted;
			var captured = tile.captured;
			tileUIs.push(html`<${TileUI} x=${x} y=${y} color=${color} value=${value} exhausted=${exhausted} captured=${captured}/>`)
		}
		return tileUIs;
	}
	
	renderRoads = () => {
		var roadUIs = [];
		for (var rid = 0; rid < this.board.roads.length; rid++) {					
			var road = this.board.roads[rid];
			var x = constants.RID_TO_C[rid] * UNIT_ROAD;
			var y = constants.RID_TO_R[rid] * UNIT_ROAD;
			var color = SIDE_COLORS[road.side];			
			var orient = road.orientation;
			var side = road.side;
			roadUIs.push(html`<${RoadUI} rid=${rid} side=${side} orient=${orient} x=${x} y=${y} color=${color} click=${this.onRoadClick}/>`);
		}
		return roadUIs;
	}
	
	renderNodes = () => {
		var nodeUIs = [];
		for (var nid = 0; nid < this.board.nodes.length; nid++) {					
			var node = this.board.nodes[nid];
			var x = constants.NID_TO_C[nid] * UNIT_NODE;
			var y = constants.NID_TO_R[nid] * UNIT_NODE;
			var color = SIDE_COLORS[node.side];		
			var side = node.side;
			nodeUIs.push(html`<${NodeUI} nid=${nid} side=${side} x=${x} y=${y} color=${color} click=${this.onNodeClick}/>`);
		}
		return nodeUIs;
	}
	
	
	renderRes = (side) => {		
		var res = this.board.res[side];
		if (this.board.phase != constants.PHASE_PLAY) return;
		return html`<${ResUI} side=${side} res=${res} x="65" y="620" onTrade=${this.onTrade}/>`;
	}
	
	

	render () {						
		var turn = this.board.turn == constants.SIDE_1? 'Player 1' : 'Player 2';
		return (		
			html`
				<div style="float:right">
				<button id="btnShuffle" onclick=${this.onShuffle}>Shuffle</button><br/>
				
				</div>
				<button id="btnDone" onclick=${this.onChangeTurn}>Done</button>
				
				<svg width="800" height="800" style="border:1px solid black;">
					<${ScoreUI} x="10" y="20" label="Score 1:" value=${this.board.scores[0]}/>
					<${ScoreUI} x="650" y="20" label="Score 2:" value=${this.board.scores[1]}/>
					<${TurnUI} x="10" y="575" label="Turn:" value=${turn}/>
					
					${this.renderTiles()}
					${this.renderRoads()}
					${this.renderNodes()}
					${this.renderRes(this.board.turn)}								
				</svg>				
			`
		);
	}
	
}