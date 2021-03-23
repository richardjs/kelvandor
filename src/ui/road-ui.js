import { html, Component } from '../../lib/preact.js';
import {ORIENTATION_HORIZONTAL, ORIENTATION_VERTICAL, SIDE_NONE} from '../core/constants.js';
import {SIZE_ROAD_W, SIZE_ROAD_H, SIZE_ROAD_EMPTY_W, SIZE_ROAD_EMPTY_H} from './constants-ui.js';


const OFFSET_ROAD_X = 50;
const OFFSET_ROAD_Y = 50;

const LONGEST_W = SIZE_ROAD_W-10;
const LONGEST_H = SIZE_ROAD_H-5;

const LONGEST_COLORS = ['#e58100', '#cebdda'];


export class RoadUI extends Component {
	
	onClick = (e) => {
		this.props.click(e, this.props.rid);	
	}
	
	renderInLongest = (inLongest, x, y, orient, fill) => {
		if (!menu.showLongest || !inLongest) return;
		
		var w;
		var h;
		if (orient == ORIENTATION_HORIZONTAL) {
			x += 5;
			y += 2.5;
			w = LONGEST_W;
			h = LONGEST_H;
		}
		else {
			x += 2.5;
			y += 5;
			w = LONGEST_H;
			h = LONGEST_W;
		}
		return (
			html`<rect 					
					x=${x} 
					y=${y}
					width=${w}
					height=${h}
					fill=${LONGEST_COLORS[this.props.side]} 					
				/>
			`
		);
	}


	render() {		
		var orient = this.props.orient;
		var side = this.props.side;
		var cssClass = (side == SIDE_NONE)? 'road empty' : 'road';
		if (this.props.last) cssClass += ' last';
		
		var roadW = (side == SIDE_NONE)? SIZE_ROAD_EMPTY_W : SIZE_ROAD_W;
		var roadH = (side == SIDE_NONE)? SIZE_ROAD_EMPTY_H : SIZE_ROAD_H;
		var width = (orient == ORIENTATION_HORIZONTAL)? roadW : roadH;
		var height = (orient == ORIENTATION_HORIZONTAL)? roadH : roadW;
		var halfW = width/2;
		var halfH = height/2;
		var x = OFFSET_ROAD_X + this.props.x - halfW;
		var y = OFFSET_ROAD_Y + this.props.y - halfH;
		var inLongest = side != SIDE_NONE && this.props.inLongest;		
		return (
			html`<rect 
					class=${cssClass}
					x=${x} 
					y=${y}
					width=${width}
					height=${height}
					fill=${this.props.color} 					
					onclick=${this.onClick}
				/>
				${this.renderInLongest(inLongest, x, y, orient)}				
				`
		);
	}
	
}