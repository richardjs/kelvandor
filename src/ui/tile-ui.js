import * as CONST_UI from './constants-ui.js';
const SIZE_TILE = CONST_UI.SIZE_TILE;
const UNIT_TILE = 50;

const SIZE_DOT = 10;

const TILE_HALF = SIZE_TILE/2;
const TILE_THIRD = SIZE_TILE/3;
const TILE_TWO_THIRDS = 2*SIZE_TILE/3;

const COLOR_OUTLINE = 0xFFFFFF;
const COLOR_DOT = 0xFFFFFF;
const RES_COLORS = [0xC0C0C0, 0xFF0000, 0x00FF00, 0x0000FF, 0xFFFF00];



//Create tile 
export function renderTile(app, r, c, resColor, resVal) {
	var tile = new PIXI.Graphics();
	
	//The x,y coordinates are the top,left of the tile
	var x = c * UNIT_TILE; 
	var y = r * UNIT_TILE;
	
	//Resource colors
	tile.lineStyle(2, COLOR_OUTLINE, 1);
	tile.beginFill(RES_COLORS[resColor]);
	tile.drawRect(x, y, SIZE_TILE, SIZE_TILE);
	tile.endFill();	
	
	//Add Resource values (dots)
	tile.lineStyle(0); // draw a circle, set the lineStyle to zero so the circle doesn't have an outline	
	
	if (resVal == 1) { //Single Dot
		createDot(tile, x+TILE_HALF, y+TILE_HALF); 
	}
	else if (resVal == 2) { //Double Dots
		createDot(tile, x+TILE_TWO_THIRDS, y+TILE_HALF); 
		createDot(tile, x+TILE_THIRD, y+TILE_HALF); 	
	}
	else if (resVal == 3) { //Triple Dots		
		createDot(tile, x+TILE_HALF, y+TILE_THIRD);
		createDot(tile, x+TILE_TWO_THIRDS, y+TILE_TWO_THIRDS); 
		createDot(tile, x+TILE_THIRD, y+TILE_TWO_THIRDS); 
	}
	//else vacant
	
	app.stage.addChild(tile);
	
	return tile;
}

function createDot(tile, x, y) {
	tile.beginFill(COLOR_DOT, 1);
	
	tile.drawCircle(x, y, SIZE_DOT);	
	tile.endFill();
}
