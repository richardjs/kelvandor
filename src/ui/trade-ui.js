import { html, Component } from '../../lib/preact.js';
import {SIZE_TRADE_W, SIZE_TRADE_H, COUNT_TRADE} from './constants-ui.js';

const SIZE_DOT = 10;
const UNIT_DOT = SIZE_DOT*2;

const BUTTON_LEFT = 0;
const BUTTON_RIGHT = 2;

export class TradeUI extends Component {
	state = {dots:0}
	
	onClick = (e) => {
		var dots;
		
		if (e.button == BUTTON_LEFT) {
			dots = this.state.dots+1;
			if (dots > COUNT_TRADE) dots = 0;
		}
		else {
			dots = this.state.dots-1;
			if (dots < 0) dots = 0;
		}
		this.setState({dots:dots});
	}
	
	onTrade = (e) => {
		console.log('trade');
		this.props.onTrade(this.props.vid);
	}
	
	renderDots = (y) => {
		var dots = [];
		var color = this.props.color;				
		for (var i = 0; i < this.state.dots; i++) {
			var x = this.props.x + (i*UNIT_DOT);
			
			dots.push(html`<rect x=${x} y=${y} width=${SIZE_DOT} height=${SIZE_DOT} fill=${color}/>`);
		}
		return dots;
	}
	renderCenter = (x,y) => {
		
		return (
			html`<rect class="trade"
					x=${x}
					y=${y}
					width=${SIZE_TRADE_W}
					height=${SIZE_TRADE_H}
					fill=${this.props.color}
					onclick=${this.onClick}
					oncontextmenu=${this.onClick}
				/>`					
		);
	}
	
	renderCenterText = (x,y) => {
		return (
			html `
				<text class="label"
					x=${x+10}
					y=${y+30} 							
				>${this.props.value}</text>
			`
		);
	}
	
	renderTrade = (x,y) => {
		return (
			html`<rect 
					class="btnTrade"
					x=${x}
					y=${y}
					rx="5"
					ry="5"
					width=${SIZE_TRADE_W}
					height=${25}
					fill=${this.props.color}
					onclick=${this.onTrade}					
				/>
				<text class="lblTrade"
					x=${x+10}
					y=${y+20} 							
				>Trade</text>						
			`
		);
	}
	
	render() {
		var x = Number.parseInt(this.props.x);
		var y = Number.parseInt(this.props.y);		
		return (
			html`				
				${this.renderDots(y-UNIT_DOT)}
				${this.renderCenter(x,y)}
				${this.renderCenterText(x,y)}
				${this.renderTrade(x,y+60)}
			`
		);
	}
	
}