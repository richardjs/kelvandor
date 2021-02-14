//http://qndgames.com/wp-content/uploads/2018/10/Node-Manual-V4.55-Rule-Sheet-Website-Full-copy.pdf
//http://qndgames.com/wp-content/uploads/2018/10/Node-Demo-Rule-Sheet-FandB-Combined-V1.01-11x17.pdf

import * as CONST from './core/constants.js';
//import {Board} from './core/board.js';
import {Game, EVENT_MESSAGE, EVENT_PLAYED, EVENT_GAME_OVER, EVENT_BOARD_UPDATE} from './core/game.js';

import {renderTile} from './ui/tile-ui.js';
import {renderNode} from './ui/node-ui.js';
import {renderRoad} from './ui/road-ui.js';



//Init
var game = new Game();
var board = game.board;

PIXI.utils.skipHello();
const app = new PIXI.Application({ antialias: true });
document.body.appendChild(app.view);
//app.renderer.backgroundColor = 0xFF00FF;


//Callbacks
export let onNodeHover = (nodeR, nodeC) => {	
	message.text = 'Node: ' + nodeR + ',' + nodeC;
}

export let onNodeClick = (nodeR, nodeC) => {
	console.log(nodeR, nodeC);	
}

let onMessageReceived = (msg) => {	
	message.text = msg;
};

let onPlayed = (msg) => {	
	console.log('onplayed');
}

let onGameOver = (msg) => {	
	alert('Game over!');
}

let onBoardUpdate = (newBoard) => {	
	board = newBoard;
}

//Register Events
game.addEventListener(EVENT_MESSAGE, onMessageReceived);
game.addEventListener(EVENT_PLAYED, onPlayed);
game.addEventListener(EVENT_GAME_OVER, onGameOver);
game.addEventListener(EVENT_BOARD_UPDATE, onBoardUpdate);


//Tiles
for (var i = 0; i < board.tiles.length; i++) {
	var tile = board.tiles[i];
	renderTile(app, tile.r, tile.c, tile.resCol, tile.resVal);
}

//Nodes
for (var i = 0; i < board.nodes.length; i++) {
	var node = board.nodes[i];
	renderNode(app, node.r, node.c);
}

//Roads
for (var i = 0; i < board.roads.length; i++) {
	var road = board.roads[i];
	renderRoad(app, road);
}

//Message text
var message = new PIXI.Text('');
message.style.fill = 0xffffff;
message.x = 5;
message.y = 570;
message.style.fontSize = 16;
app.stage.addChild(message);



