import * as constants from '../core/constants.js';
import { PLAYER_HUMAN, PLAYER_KELVANDOR} from './menu.js';
import {UNIT_TILE, UNIT_NODE, UNIT_ROAD, RES_COLORS, SIDE_COLORS} from './constants-ui.js';
import {Board, DEFAULT_BOARD_STATE, residFromChar} from '../core/board.js';
import * as networkPlayer from '../../lib/network-player.js';
import * as Url from '../../lib/url-lib.js';


import { html, Component } from '../../lib/preact.js';
import { TileUI } from './tile-ui.js';
import { NodeUI } from './node-ui.js';
import { RoadUI } from './road-ui.js';
import { ResUI } from './res-ui.js';
import { ResMiniUI } from './res-mini-ui.js';
import { ScoreUI } from './score-ui.js';
import { TurnUI } from './turn-ui.js';
import { AnalyzeUI } from './analyze-ui.js';


const KEY_ENTER = 13;
const DELAY_FLASH_MSG = 2000; //MS
const DELAY_PLAY_NEXT = 500;


export class BoardUI extends Component {
	state = {
		msg:'', 
		analyzeNodes:new Array(constants.COUNT_NODES),
		analyzeRoads:new Array(constants.COUNT_ROADS), 
		analyzeRes:new Array(constants.COUNT_RES), 				
	}

	constructor() {
		super();				
		document.addEventListener('keydown', this.onKeyDown);
		document.addEventListener('contextmenu', this.onRightClick);			
		

		var boardStr = DEFAULT_BOARD_STATE;
		if (performance.navigation.type == 0) { //First time on this page
			var hash = window.location.hash.replace('#', '');
			if (hash.length > 50) boardStr = hash;		
			else Url.setHashNonVolatile(''); //Clear state
		}
		else Url.setHashNonVolatile('');  //Refresh - clear state

		this.board = Board.fromString(boardStr);
			
		this.history = [this.board.toString()];
		this.historyIndex = 0;		
	}
		
	
	//Events				
	onKeyDown = (e) => {	
		
		if (e.keyCode == KEY_ENTER) {
			this.board.changeTurn();
			this.forceUpdate();
			this.playNext();
		}
		
		if (e.key == 'k' && !document.getElementById('btnPlayNetwork').disabled) {
			this.playNetwork();
		}

		else if (e.key == 'p') {
			if (!this.isHumanTurn()) this.playNetwork();			
		}

		if (e.key == 's') {
			this.onShuffle();			
		}
		
		if (e.key == 'f') { //Hack to refresh for menu events
			this.forceUpdate();
		}

		//Conflict with Red trade resource
		if (e.key == 'n') { //new
			this.onReset();			
		}

        if (e.key == 'm') {
            this.analyze();
        }

		if (e.key == 'ArrowLeft' || (e.key == 'z' && e.ctrlKey)) { //undo
			this.undo();
		}
		else if (e.key == 'ArrowRight' || (e.key == 'y' && e.ctrlKey)) { //redo
			this.redo();
		}		
	}
	
	onNodeClick = (e, nid) => {		
		if (!this.isHumanTurn()) return;
		var action = this.board.addNode(nid, e.ctrlKey);		
		if (action.status) {			
			this.recordState();
			this.playNext();
		}
		this.flashMsg(action.msg);	
	}
	
	onRoadClick = (e, rid) => {
		if (!this.isHumanTurn()) return;

		var action = this.board.addRoad(rid, e.ctrlKey);
		if (action.status) {			
			this.recordState();
			this.playNext();
		}
		this.flashMsg(action.msg);	
	}

	onRightClick = (e) => {
		e.preventDefault(); //Prevent context menu when right clicking to cancel resource trading
	}


	onShuffle = (e) => {
		this.board.reset();
		this.board.shuffle();
		this.forceUpdate();
		this.recordState();		
	}

	onReset = (e) => {
		this.board.reset();
		this.forceUpdate();
		this.recordState();
	}


	onChangeTurn = (e) => {
		if (!this.isHumanTurn()) return;
		var action = this.board.changeTurn(true);
		if (action.status) {
			this.recordState();			
			document.dispatchEvent(new KeyboardEvent('keydown',{'keyCode':27})); //Hack to clear trade UI - TODO: replace with game event				
			this.playNext();
		}
		this.flashMsg(action.msg);
	}

	onTrade = (e, tradeResids) => {		
		if (!this.isHumanTurn()) return;
		
		var action = this.board.trade(tradeResids, e.ctrlKey);
		this.recordState();		
		this.flashMsg(action.msg);
		this.playNext();
	}


	playNext = () => {
		if (this.board.phase == constants.PHASE_GAME_OVER) return;
		
		this.setState({				
			analyzeEnd:undefined,
			analyzeNodes:[],
			analyzeRoads:[],
			analyzeRes:[]
		});

		if (!this.isHumanTurn()) {
			var self = this;
			setTimeout(function() {			
				self.playNetwork();				
			}, DELAY_PLAY_NEXT);
		}			
	}

	playNetwork = (e) => {	
		var btnDone = document.getElementById('btnDone');
		var btnPlayNetwork = document.getElementById('btnPlayNetwork');
		btnPlayNetwork.disabled = true;
		btnPlayNetwork.style.display = 'block';
		if (btnDone) {
			btnDone.disabled = true;			
		}

		var self = this;
		networkPlayer.getMove(this.board.toString(), menu.iterations, function(actions) {
			var actionResult = self.board.playActions(actions);
			self.recordState();
			
			if (actionResult.msg) {
				console.log(actionResult.msg);
			}
			self.flashMsg(actionResult.msg);
			
			if (btnDone) btnDone.disabled = false;							
			btnPlayNetwork.disabled = false;
			btnPlayNetwork.style.display = 'none';
			self.playNext();
		});
	}

    analyze = () => {
		var self = this;
        networkPlayer.getMoveScan(this.board.toString(), menu.iterations, function(actions) {
			var analyzeNodes = new Array(constants.COUNT_NODES);
			var analyzeRoads = new Array(constants.COUNT_ROADS); 
			var analyzeRes = new Array(constants.COUNT_RES);
			var analyzeEnd;			
            for (var a = 0; a < actions.length; a++){
				var item = actions[a];
				var action = item.action;
				var actionChar = action[0];
				var val = item.value;
				
				//Start
				if (actionChar == 's') { 
					var nid = Number.parseInt(action.substr(1,2));
					var rid = Number.parseInt(action.substr(3));					
					analyzeNodes[nid] = val;
					analyzeRoads[rid] = val;					
				}

				//Trade
				else if (actionChar == 't') {
					action = action.toUpperCase();
					var colors = action.substr(1,3).toUpperCase();						
					var target = residFromChar(action[4]);
					if (!analyzeRes[target]) analyzeRes[target] = [];
					analyzeRes[target].push({colors:colors, val:val});
				}

				//Build node
				else if (actionChar == 'n') {
					var nid = Number.parseInt(action.substr(1));		
					analyzeNodes[nid] = val;
				}

				//Build branch (road)
				else if (actionChar == 'b') {
					
					var rid = Number.parseInt(action.substr(1));						
					analyzeRoads[rid] = val;
				}

				//End
				else if (actionChar == 'e') {
					analyzeEnd = val;
				}
			}
			//console.log(analyzeRoads);
			self.setState({				
				analyzeEnd:analyzeEnd,
				analyzeNodes:analyzeNodes,
				analyzeRoads:analyzeRoads,
				analyzeRes:analyzeRes
			});
        });
    }

	

	recordState = () => {
		var boardStr = this.board.toString();
		Url.setHashNonVolatile(boardStr);
		this.history = this.history.slice(0, this.historyIndex + 1);
		this.history.push(boardStr);
		this.historyIndex++;

		// Debugging check
		if (this.board.toString() != Board.fromString(this.board.toString()).toString()) {
			console.log('Warning: serialization and deserialization results in a different board string!');
			console.log(this.board.toString())
			console.log(Board.fromString(boardStr).toString());
		}
	}

	undo = () => {
		if (this.historyIndex > 0) {
			var boardStr = this.history[--this.historyIndex];
			this.board = Board.fromString(boardStr);
			Url.setHashNonVolatile(boardStr);
			this.forceUpdate();
		}
	}

	redo = () => {
		if (this.historyIndex < this.history.length - 1) {
			var next = this.history[++this.historyIndex];
			this.board = Board.fromString(next);
			Url.setHashNonVolatile(next);
			this.forceUpdate();
		}
	}


	flashMsg = (msg) => {
		this.setState({msg:msg});
		var self = this;
		setTimeout(function() {
			if (self.state.msg == msg) self.setState({msg:''});
		}, DELAY_FLASH_MSG);
	}

	isHumanTurn = () => {
		var players = [parseInt(menu.player1), parseInt(menu.player2)];	
		if (players[this.board.turn] == PLAYER_HUMAN  ) return true;
		else return false;
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
			var last = (this.board.lastNids.indexOf(nid) >= 0)? true : false;
			
			nodeUIs.push(html`<${NodeUI} nid=${nid} side=${side} x=${x} y=${y} last=${last} color=${color} click=${this.onNodeClick} />`);
			nodeUIs.push(html`<${AnalyzeUI} x=${x+35} y=${y+UNIT_NODE} val=${this.state.analyzeNodes[nid]} />`);
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
			var last = (this.board.lastRids.indexOf(rid) >= 0)? true : false;
			var inLongest = (this.board.longestRids.indexOf(rid) >= 0)? true : false;
			
			roadUIs.push(html`<${RoadUI} rid=${rid} side=${side} orient=${orient} last=${last} x=${x} y=${y} color=${color} click=${this.onRoadClick} inLongest=${inLongest} />`);
			roadUIs.push(html`<${AnalyzeUI} x=${x+35} y=${y+UNIT_NODE} val=${this.state.analyzeRoads[rid]} />`);
		}
		return roadUIs;
	}
	
	
	
	renderRes = (x, y, side) => {		
		var res = this.board.res[side];
		if (this.board.phase != constants.PHASE_PLAY) return;
		return html`<${ResUI} side=${side} res=${res} x=${x} y=${y} onTrade=${this.onTrade} analyzeRes=${this.state.analyzeRes}/>`;
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



	render () {						
		var turn = this.board.turn == constants.SIDE_1? 'Player 1' : 'Player 2';
		var side = this.board.turn;
		var oppSide = this.board.turn == constants.SIDE_1? constants.SIDE_2 : constants.SIDE_1;
		
		return (		
			html`
				<div id="panel">														
					<button id="btnPlayNetwork" readonly="readonly" style="display:none">Kelvandor playing</button>				
				</div>
				${this.renderDone()}				
				
				<svg width="800" height="800">
					<${ScoreUI} x="10" y="20" label="Score 1:" value=${this.board.scores[constants.SIDE_1]} info=${this.board.scoreBreakDowns[constants.SIDE_1]}/>
					<${ScoreUI} x="650" y="20" label="Score 2:" value=${this.board.scores[constants.SIDE_2]} info=${this.board.scoreBreakDowns[constants.SIDE_2]}/>
					<${TurnUI} x="10" y="575" label="Turn:" value=${turn} phase=${this.board.phase}/>												
					<${AnalyzeUI} x=${600} y=${717.5} val=${this.state.analyzeEnd} />
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