import { html, Component } from '../../lib/preact.js';
import {ORIENTATION_HORIZONTAL, ORIENTATION_VERTICAL, SIDE_NONE} from '../core/constants.js';
import {SIZE_ROAD_W, SIZE_ROAD_H, SIZE_ROAD_EMPTY_W, SIZE_ROAD_EMPTY_H} from './constants-ui.js';


const OFFSET_ROAD_X = 50;
const OFFSET_ROAD_Y = 50;



export class RoadUI extends Component {
	
	onClick = (e) => {
		this.props.click(this.props.rid);	
	}
	
	render() {		
		var cssClass = (this.props.side == SIDE_NONE)? 'road empty' : 'road';
		if (this.props.last) cssClass += ' last';
		
		var roadW = (this.props.side == SIDE_NONE)? SIZE_ROAD_EMPTY_W : SIZE_ROAD_W;
		var roadH = (this.props.side == SIDE_NONE)? SIZE_ROAD_EMPTY_H : SIZE_ROAD_H;
		var width = (this.props.orient == ORIENTATION_HORIZONTAL)? roadW : roadH;
		var height = (this.props.orient == ORIENTATION_HORIZONTAL)? roadH : roadW;
		var halfW = width/2;
		var halfH = height/2;
		var x = OFFSET_ROAD_X + this.props.x - halfW;
		var y = OFFSET_ROAD_Y + this.props.y - halfH;
		
		return (
			html`<rect 
					class=${cssClass}
					x=${x} 
					y=${y}
					width=${width}
					height=${height}
					fill=${this.props.color} 
					onclick=${this.onClick}
			/>`
		);
	}
	
}