import { html, Component } from '../../lib/preact.js';
import {TradeUI} from './trade-ui.js';
import {COUNT_RES, RES_BLUE, RES_GREEN, RES_RED, RES_VACANT, RES_YELLOW} from '../core/constants.js';
import {UNIT_TRADE} from './constants-ui.js';

const BUTTON_LEFT = 0;
const BUTTON_RIGHT = 2;
const KEY_ESCAPE = 27;

export class ResUI extends Component {
	
	state = {dots:[0,0,0,0]};
	constructor() {
		super();
		document.addEventListener('keydown', this.onKeyDown);
	}

	componentDidMount = () => {						
	}
	
	onTrade = (targetResid) => {		
		var tradeResids = [];
		var dots = this.state.dots;		
		for (var resid = 0; resid < dots.length; resid++) {
			var dotCount = dots[resid];
			for (var d = 0; d < dotCount; d++) {
				tradeResids.push(resid);
			}
		}
		if (tradeResids.length == 3) {
			tradeResids.push(targetResid);			
			this.props.onTrade(tradeResids);
			this.setState({dots:[0,0,0,0]});
		}

	}

	onDotAdded = (btn, resid) => {
		var res = this.props.res;
		
		if (btn == BUTTON_LEFT) {
			if (res[resid]) {
				var dots = this.state.dots;
				if (res[resid]-dots[resid] > 0) {
					if (this.totalDotCount() < 3) {					
						dots[resid]++;		
						this.setState({dots:dots});
					}
				}
			}
		}
		else if (btn == BUTTON_RIGHT) { //Cancel 
			var dots = this.state.dots;
			dots[resid] = 0;
			this.setState({dots:dots});
		}
	}


	totalDotCount = () => {
		var dots = this.state.dots;
		var count = 0;
		for (var resid = 0; resid < dots.length; resid++) {
			if (dots[resid] > 0) count += dots[resid];
		}
		return count;
	}
	
	onKeyDown = (e) => {	
		var keyChar = e.key.toUpperCase();
		
		if (e.keyCode == KEY_ESCAPE) this.setState({dots:[0,0,0,0]});
		else {		
			var resid = RES_VACANT;
			if (keyChar == 'B') resid = RES_BLUE;			
			else if (keyChar == 'G') resid = RES_GREEN;
			else if (keyChar == 'R') resid = RES_RED; 
			else if (keyChar == 'Y') resid = RES_YELLOW;

			if (resid != RES_VACANT) {
				if (this.totalDotCount() == 3) this.onTrade(resid);
				else this.onDotAdded(BUTTON_LEFT, resid);
			}
		}

	}
	
	
	render() {		
		var x = Number.parseInt(this.props.x);
		var y = Number.parseInt(this.props.y);
		var tradeUIs = [];		
		var res = this.props.res;
		var dots = this.state.dots;

			
		var tradeX = x;
		for (var resid = 0; resid < COUNT_RES; resid++) {			
			var value = res[resid];			
			var color = resid;		
			var dotCount = dots[resid];
			tradeUIs.push(html`<${TradeUI} resid=${resid} x=${tradeX} y=${y} color=${color} value=${value} dots=${dotCount} onTrade=${this.onTrade} onDotAdded=${this.onDotAdded}/>`);
			tradeX += UNIT_TRADE;
			if (resid == 1) tradeX += 20;
		}		
		
		
		return tradeUIs;
	}
	
}