import { html, Component } from '../../lib/preact.js';
import {SIZE_TRADE_W, SIZE_TRADE_H, COUNT_TRADE, RES_COLORS, RES_COLORS_HOVER, RES_COLORS_DISABLED} from './constants-ui.js';

const SIZE_DOT = 10;
const UNIT_DOT = SIZE_DOT*2;



export class TradeUI extends Component {			
	
	constructor() {
		super();				
	}

	componentDidMount = () => {		
		var color = RES_COLORS[this.props.color];
		this.setState({fillCenter:color, fillTrade:color});
	}
	
	onMouseOverCenter = (e) => {		
		this.setState({fillCenter:RES_COLORS_HOVER[this.props.color]});		
	}
	
	onMouseOutCenter = (e) => {					
		this.setState({fillCenter:RES_COLORS[this.props.color]});		
	}

	onMouseOverTrade = (e) => {		
		this.setState({fillTrade:RES_COLORS_HOVER[this.props.color]});		
	}
	
	onMouseOutTrade = (e) => {		
		this.setState({fillTrade:RES_COLORS[this.props.color]});		
	}

	onClick = (e) => {		
							
		this.props.onDotAdded(e.button, this.props.resid);		
	}
	
	onTrade = (e) => {		
		this.props.onTrade(this.props.resid);		
	}
	
	renderDots = (y) => {
		var dots = [];
		var color = RES_COLORS[this.props]
		for (var i = 0; i < this.props.dots; i++) {
			var x = this.props.x + (i*UNIT_DOT);
			
			dots.push(html`<rect x=${x} y=${y} width=${SIZE_DOT} height=${SIZE_DOT} fill=${this.state.fillCenter}/>`);
		}
		return dots;
	}
	renderCenter = (x,y) => {
		
		return (
			html`<rect class="btn"
					x=${x}
					y=${y}
					width=${SIZE_TRADE_W}
					height=${SIZE_TRADE_H}
					fill=${this.state.fillCenter}
					onclick=${this.onClick}
					oncontextmenu=${this.onClick}
					onmouseover=${this.onMouseOverCenter}
					onmouseout=${this.onMouseOutCenter}
				/>`					
		);
	}
	
	renderCenterText = (x,y) => {
		return (
			html `
				<text class="label lblTrade"
					x=${x+10}
					y=${y+30} 							
				>${this.props.value-this.props.dots}</text>
			`
		);
	}
	
	renderTrade = (x,y) => {
		return (
			html`<rect 
					class="btn"
					x=${x}
					y=${y}
					rx="5"
					ry="5"
					width=${SIZE_TRADE_W}
					height=${25}
					fill=${this.state.fillTrade}
					onclick=${this.onTrade}	
					onmouseover=${this.onMouseOverTrade}
					onmouseout=${this.onMouseOutTrade}				
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