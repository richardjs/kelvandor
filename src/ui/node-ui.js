import * as CONST_UI from './constants-ui.js';
import * as stage from '../index.js';

const SIZE_NODE = 15
const UNIT_NODE = 50;
const OFFSET_NODE = 50;

const COLOR_NODE = 0xAAAAAA;
const COLOR_HOVER_OVER = 0xFF0000;
const COLOR_HOVER_OUT = 0xFFFFFF;



export function renderNode(app, r, c) {
	var node = new PIXI.Graphics();
	var x = (c * UNIT_NODE) + OFFSET_NODE;
	var y = (r * UNIT_NODE) + OFFSET_NODE;
	

	node.lineStyle(0); // draw a circle, set the lineStyle to zero so the circle doesn't have an outline
	node.beginFill(COLOR_NODE, 1);
	node.drawCircle(x, y, SIZE_NODE);
	node.endFill();
	app.stage.addChild(node);
	node.interactive = true;	
	node.r = r;
	node.c = c;
	node.on('pointerover', onHoverOver);
	node.on('pointerout', onHoverOut);
	node.on('pointerdown', onClick);
}

function onHoverOver() {	
	this.tint = COLOR_HOVER_OVER;
	stage.onNodeHover(this.r, this.c);	
}

function onHoverOut() {
	this.tint = COLOR_HOVER_OUT;
}

function onClick() {
	stage.onNodeClick(this.r, this.c);
}