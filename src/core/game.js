import * as CONST from './constants.js';
import {Board} from './board.js';
//About: Class to manage players and events
//Constants
const PLAYER_HUMAN = CONST.PLAYER_HUMAN;

export const EVENT_INVALID = 0;
export const EVENT_PLAYED = 1;
export const EVENT_GAME_OVER = 2;
export const EVENT_BOARD_UPDATE = 3;
export const EVENT_MESSAGE = 4;
export const EVENT_NODE_ADDED = 5;
export const EVENT_ROAD_ADDED = 6;

const MODE_PLACE = 0;
const MODE_PLAY = 1;
const MODE_UNDO = 2;

export class Game {
	constructor(boardStr) {
		//this.board = new Board(); //The main (current) board instance		
		//this.board.init();
		this.board = Board.fromString('R2G1B2R3G2Y2V0G3Y1B3R1B1Y3000000000000000000000000000000000000000000000000000000000000010;0;0;0;0;0;0;0');
		//boardStr = this.board.toString(); //Update
		
		//Add initial state
		//this.history = [boardStr]; //History is for game log
		//this.memory = {}; //Memory is for detecting repeats
		//this.memory[boardStr] = true;
		//this.undoHistory = [];
		
		this.players = [PLAYER_HUMAN, PLAYER_HUMAN];
		
		this.gameEvents = {}; //Callbacks to update UI				
		this.mode = MODE_PLAY;
		//this.moves = [];
	}

	/*load(gameStr) {
	
	var moves = gameStr.split(',');
	
	for (var m = 0; m < moves.length; m++) {
		var qmn = moves[m];
		var move = this.board.qmnToMove(qmn);
		var moveCode = this.board.makeMove(move);
		var boardStr = this.board.toString();
		if (moveCode != VALID) throw new Error('Error loading move (' + m + '): ' + qmn + ', ' + boardStr);					
		
		this.history.push(boardStr);
		this.memory[boardStr] = true;
		this.board.changeTurn();
	}
	return true;


	save() { //Serialize entire game
		//var gameStr = '';
		//for (var m = 0; m < this.moves.length; m++) {
		//	var moveStr = this.moves[m];
		//	gameStr += moveStr + ',';
		//}
		//return gameStr.substr(0, gameStr.length-1);
	}
	*/


	updateBoard(newBoard) {
		this.board = newBoard;
		this.gameEvents[EVENT_BOARD_UPDATE](newBoard);
	}

	//Event methods
	addEventListener(name, callback) {	
		this.gameEvents[name] = callback;
	}


	onGameOver(winner) {
			
		//this.logCurrentState(boardCopy);
		
		//Draw the win and other hoopla...
		this.gameEvents[EVENT_GAME_OVER](winner, +(!winner));
			
	}
	
	canHumanPlay() {
		//if (this.players[this.board.turn] == PLAYER_HUMAN || this.mode == MODE_UNDO) return true;
		//else
			return false;

	}

	undoMove() {
		
		//if (this.history.length > 1) {	
		//	var oldTurn = this.board.turn;
		//	var oldStr = this.history.pop();
		//	this.undoHistory.push(oldStr);
		//	delete this.memory[oldStr];
		//	var boardStr = this.history[this.history.length-1];
		//	
		//	this.board = new Board(boardStr);		
		//	this.board.turn = +(!oldTurn);		
		//	Url.setHash(boardStr);
		//	this.mode = MODE_UNDO;
		//	this.gameEvents[EVENT_BOARD_UPDATE](this.board);
		//	return true;		
		//}
		//return false;
	}


	redoMove =() =>{	
		//if (this.undoHistory.length > 0) {	
		//	var oldTurn = this.board.turn;
		//	var savedStr = this.undoHistory.pop();
		//	this.history.push(savedStr);
		//	this.memory[savedStr] = true;
		//	this.board = new Board(savedStr);							
		//	this.board.turn = +(!oldTurn);
		//	Url.setHash(savedStr);
		//	
		//	//Check for Game over		
		//	if (this.board.isGameOver()) this.onGameOver(this.board.turn);	
		//	this.mode = MODE_UNDO;
		//	this.gameEvents[EVENT_BOARD_UPDATE](this.board);
		//	return true;
		//}
		return false;
	}



	//Helper function keep track of game history
	logCurrentState =(board) => {
		//var boardStr = board.toString();
		//this.history.push(boardStr);
        //
		//this.memory[boardStr] = true;
	}

	changeTurn = () => {
		this.board.changeTurn();
		
	}

	addNode = (nid) => {			
		if (this.board.addNode(nid)) {
			this.gameEvents[EVENT_NODE_ADDED](nid, this.board.turn);
		}
		else {
			this.gameEvents[EVENT_MESSAGE]('Invalid Node');
		}
	}

	addRoad = (rid) => {
		if (this.board.addRoad(rid)) {
			this.gameEvents[EVENT_ROAD_ADDED](rid, this.board.turn);
		}
		else {
			this.gameEvents[EVENT_MESSAGE]('Invalid Road');
		}
	}
	
	shuffleTiles = () => {
		this.board.shuffle();		
		this.gameEvents[EVENT_BOARD_UPDATE](this.board);
	}

	//Player functions
	play =() => {
		
		var board = this.board;
		var turn = board.getTurn();
		var player = this.players[turn];
		
		if (player == PLAYER_HUMAN) return; //Ignore
		
		//Handle no-move, and one move
		//var moves = board.getMoves();	
		//if (moves.length == 0) throw new Error('No moves available');//return this.onPlayed();
		//else if (moves.length == 1) return this.onPlayed(moves[0]);
		
		
		//All Async - expect onPlayed callback	
		//switch (player) {		
		//	case PLAYER_RANDOM: RandomPlayer.getPlay(board, this.onPlayed); break;	//Random					
		//	default: alert('Invalid player');
		//}		
	}

	onPlayed =(move) => {
		var self = game;	
		self.mode = MODE_PLAY;			
		var board = self.board;	
		var turn = board.getTurn();
		var player = self.players[turn];
		
		var moveCode = board.makeMove(move);
		if (moveCode != VALID) return self.gameEvents[EVENT_INVALID]('', moveCode);	
		self.moves.push(board.qmnFromMove(move));
		
		//History and Memory
		self.logCurrentState(board);	
		
		//Check for game over
		if (board.isGameOver()) self.onGameOver(board.turn);
		else {
			board.changeTurn();		
			self.gameEvents[EVENT_PLAYED](player, move, +(!board.turn));
		}
	}
	
	broadcast =(event, args) => {
		this.gameEvents[event](args);
	}


	sendMessage = (msg) => {
		this.gameEvents[EVENT_MESSAGE](msg);
	}

	
	
	


}
//end class Game
