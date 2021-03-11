import * as constants from '../core/constants.js';
import {UNIT_TILE, UNIT_NODE, UNIT_ROAD, RES_COLORS, SIDE_COLORS} from './constants-ui.js';
import {Board, DEFAULT_BOARD_STATE} from '../core/board.js';
import * as networkPlayer from '../../lib/network-player.js';


import { html, Component } from '../../lib/preact.js';
import { TileUI } from './tile-ui.js';
import { NodeUI } from './node-ui.js';
import { RoadUI } from './road-ui.js';
import { ResUI } from './res-ui.js';
import { ResMiniUI } from './res-mini-ui.js';
import { ScoreUI } from './score-ui.js';
import { TurnUI } from './turn-ui.js';

const KEY_ENTER = 13;
const DELAY_FLASH_MSG = 2000; //MS


export class BoardUI extends Component {
	state = {msg:''}

	constructor() {
		super();
		document.addEventListener('keydown', this.onKeyDown);
		document.addEventListener('contextmenu', this.onRightClick);		    
		
		//this.board = Board.fromString(DEFAULT_BOARD_STATE);
		this.board = new Board();
		this.board.defaultSetup();
		
		//let state = new kelvandor.State();
		//console.log(state.toString());						
		//state.delete();
	}
		
	
	//Events		
	onNodeClick = (nid) => {
		var action = this.board.addNode(nid);		
		if (action.status) {			
			//window.location.hash = this.board.toString();			
		}
		this.flashMsg(action.msg);	
	}
	
	onRoadClick = (rid) => {
		var action = this.board.addRoad(rid);
		if (action.status) {			
			//window.location.hash = this.board.toString();			
		}
		this.flashMsg(action.msg);	
	}
	
	onKeyDown = (e) => {
		if (e.keyCode == KEY_ENTER) {
			this.board.changeTurn();
		}

	}
	
	onRightClick = (e) => {
		e.preventDefault();
	}

	
	onShuffle = (e) => {
		this.board.reset();
		this.board.shuffle();
		this.forceUpdate();
	}

	onReset = (e) => {
		this.board.reset();
		this.forceUpdate();
	}

	playRandom = (e) => {		
		var self = this;
		networkPlayer.getMove(this.board.toString(), function(actions) {
			var actionResult = self.board.playActions(actions);
			if (actionResult.msg)console.log(actionResult.msg);
			self.flashMsg(actionResult.msg);	
		});

	}

	onHarvest = (e) => {
		this.board.harvest();
		this.forceUpdate();		
	}
	
	onChangeTurn = (e) => {
		var action = this.board.changeTurn();
		if (action.status) {
			var boardStr = this.board.toString();				
			//window.location.hash = boardStr;
			document.dispatchEvent(new KeyboardEvent('keydown',{'keyCode':27})); //Hack to clear trade UI						
		}
		this.flashMsg(action.msg);
	}
	
	

	onTrade = (tradeResids) => {
		var action = this.board.trade(tradeResids);
		this.flashMsg(action.msg);
	}

	flashMsg = (msg) => {
		this.setState({msg:msg});
		var self = this;
		setTimeout(function() {
			if (self.state.msg == msg) self.setState({msg:''});
		}, DELAY_FLASH_MSG);
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
	
	
	
	renderRes = (x, y, side) => {		
		var res = this.board.res[side];
		if (this.board.phase != constants.PHASE_PLAY) return;
		return html`<${ResUI} side=${side} res=${res} x=${x} y=${y} onTrade=${this.onTrade}/>`;
	}
	
	renderResMini = (x, y, side) => {				
		if (this.board.phase != constants.PHASE_PLAY) return;
		return html`<${ResMiniUI} res=${this.board.res[side]} x=${x} y=${y} />`;
	}
	

	renderDone = () => {
		if (this.board.phase != constants.PHASE_PLAY) return;
		return (
			html `
				<button id="btnDone" onclick=${this.onChangeTurn}>Done</button>
				`
		);
	}

	//renderPlayers = () => {
	//	return (
	//		html `
	//			<label for="selectPlayers">Player 1: </label>
	//			<select id="selectPlayers">
	//				<option>Human</option>
	//			</select>
	//		`
	//	);
	//}

	render () {						
		var turn = this.board.turn == constants.SIDE_1? 'Player 1' : 'Player 2';
		var side = this.board.turn;
		var oppSide = this.board.turn == constants.SIDE_1? constants.SIDE_2 : constants.SIDE_1;
		return (		
			html`
				<div id="panel">				
					<button id="btnShuffle" onclick=${this.onShuffle}>Shuffle</button> <br/>
					<button id="btnShuffle" onclick=${this.onReset}>Reset</button> <br/>
					<button id="btnPlayRandom" onclick=${this.playRandom}>Play Kelvandor</button>
				</div>
				${this.renderDone()}
				
				<svg width="800" height="800" style="border:1px solid black;">
					<${ScoreUI} x="10" y="20" label="Score 1:" value=${this.board.scores[0]}/>
					<${ScoreUI} x="650" y="20" label="Score 2:" value=${this.board.scores[1]}/>
					<${TurnUI} x="10" y="575" label="Turn:" value=${turn} phase=${this.board.phase}/>												
					${this.renderResMini(415, 5, oppSide)}																

					${this.renderTiles()}
					${this.renderRoads()}
					${this.renderNodes()}
					${this.renderRes(70, 620, side)}																
				</svg>				
				<div id="msg">${this.state.msg}</div>
			`
		);
	}
	
}