import { html, Component } from '../../lib/preact.js';
import { SIDE_NONE } from '../core/constants.js';
import {SIZE_TILE, SIZE_TILE_DOT, SIDE_COLORS, RES_COLORS, RES_COLORS_HOVER} from './constants-ui.js';

const TILE_HALF = SIZE_TILE/2;
const TILE_THIRD = SIZE_TILE/3;
const TILE_TWO_THIRDS = 2*SIZE_TILE/3;

const SIZE_EXHAUSTED = 20;
const SIZE_CAPTURED = 15;

export class TileUI extends Component {
	state = {}
	
	constructor() {
		super();				
	}
	
	componentDidMount = () => {		
		this.setState({fill:RES_COLORS[this.props.color]});
	}
	
	onMouseOver = (e) => {
		
		//this.setState({fill:RES_COLORS_HOVER[this.props.color]});
	}
	
	onMouseOut = (e) => {
		//this.setState({fill:RES_COLORS[this.props.color]});
	}
	
	renderDot(x, y) {
		
		return (
			html`<circle cx=${x} cy=${y} r=${SIZE_TILE_DOT} fill="#ffffff" />`
		);
	}
	
	renderCenter(x, y, color) {
		
		return (
			html`<circle cx=${x+TILE_HALF} cy=${y+TILE_HALF} r=${SIZE_EXHAUSTED} fill=${color} />`
		);
	}
	
	renderBackground = () => {	
		//var captured = this.props.captured;
		//var style = (captured == SIDE_NONE)? '' : 'stroke:' + SIDE_COLORS[captured] + '; stroke-width:' + SIZE_CAPTURED;		
		
		return (
			html`
				<rect 
					class="tile"
					x=${this.props.x} 
					y=${this.props.y} 		
					width=${SIZE_TILE}
					height=${SIZE_TILE}
					fill=${this.state.fill}
					onclick=${this.props.click}
					onmouseover=${this.onMouseOver}
					onmouseout=${this.onMouseOut}					
				/>				
			`
		);
	}
	
	render() {
		
		var value = this.props.value;
		var x = this.props.x;
		var y = this.props.y;
		
		var dots = [];
		if (value == 1) { //Single Dot
			dots.push(this.renderDot(x+TILE_HALF, y+TILE_HALF)); 
		}
		else if (value == 2) { //Double Dots			
			dots.push(this.renderDot(x+TILE_TWO_THIRDS, y+TILE_HALF));
			dots.push(this.renderDot(x+TILE_THIRD, y+TILE_HALF));			
		}
		else if (value == 3) { //Triple Dots		
			dots.push(this.renderDot(x+TILE_HALF, y+TILE_THIRD));
			dots.push(this.renderDot(x+TILE_TWO_THIRDS, y+TILE_TWO_THIRDS)); 
			dots.push(this.renderDot(x+TILE_THIRD, y+TILE_TWO_THIRDS)); 
		}
		var shapes = [this.renderBackground(), dots];
		if (this.props.captured != SIDE_NONE ) shapes.push(this.renderCenter(x,y, SIDE_COLORS[this.props.captured]));
		else if (this.props.exhausted) shapes.push(this.renderCenter(x,y, '#333333'));
		
		return shapes;
	}
	
}