import {ORIENTATION_VERTICAL} from '../core/constants.js';
const SIZE_ROAD_W = 53;
const SIZE_ROAD_H = 12;


const UNIT_ROAD = 50;
const OFFSET_ROAD_W = 25;
const OFFSET_ROAD_H = 44;

const COLOR_ROAD = 0x00A0AA;

export function renderRoad(app, road) {
	var node = new PIXI.Graphics();
	var x = (road.c * UNIT_ROAD);
	var y = (road.r * UNIT_ROAD);
		

	node.lineStyle(0); // draw a circle, set the lineStyle to zero so the circle doesn't have an outline
	node.beginFill(COLOR_ROAD, 1);
	if (road.orientation == ORIENTATION_VERTICAL) { //Vertically oriented road
		node.drawRect(x+OFFSET_ROAD_H, y+OFFSET_ROAD_W, SIZE_ROAD_H, SIZE_ROAD_W);
	}
	else { //Horizontally oriented road
		node.drawRect(x+OFFSET_ROAD_W, y+OFFSET_ROAD_H, SIZE_ROAD_W, SIZE_ROAD_H);		
	}
	node.endFill();
	app.stage.addChild(node);		
}
