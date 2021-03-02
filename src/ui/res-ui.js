import { html, Component } from '../../lib/preact.js';
import {TradeUI} from './trade-ui.js';
import {RES_COLORS, UNIT_TRADE} from './constants-ui.js';


export class ResUI extends Component {
	
	constructor() {
		super();
		document.addEventListener('keydown', this.onKeyDown);
	}
	
	onTrade = (vid) => {
		//this.props.click(this.props.nid);	
		console.log('here2', vid);
	}
	
	onKeyDown = (e) => {	
		var keyChar = e.key.toUpperCase();
		var vid;
		//if (keyChar == 'B') vid = constants.RES_BLUE;
		//else if (keyChar == 'G') vid = constants.RES_GREEN;
		//else if (keyChar == 'R') vid = constants.RES_RED;
		//else if (keyChar == 'Y') vid = constants.RES_YELLOW;
		
		//if (vid) this.setState({lastRes:vid});
		//console.log('here');
			

	}
	
	render() {
		//console.log(this.props.last);
		var x = Number.parseInt(this.props.x);
		var y = Number.parseInt(this.props.y);
		var tradeUIs = [];		
		for (var vid = 0; vid < 4; vid++) {
			var vx =  x+ (vid * UNIT_TRADE);			
			var value = this.props.res[vid];
			var color = RES_COLORS[vid+1];
			tradeUIs.push(html`<${TradeUI} vid=${vid} x=${vx} y=${y} color=${color} value=${value} onTrade=${this.onTrade}/>`);
		}
		return tradeUIs;
	}
	
}