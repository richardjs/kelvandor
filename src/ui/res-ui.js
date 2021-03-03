import { html, Component } from '../../lib/preact.js';
import {TradeUI} from './trade-ui.js';
import {COUNT_RES} from '../core/constants.js';
import {UNIT_TRADE} from './constants-ui.js';


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

	onDotAdded = (resid) => {
		var res = this.props.res;
		
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

	onDotRemoved = (resid) => {

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
		var resid;
		//if (keyChar == 'B') resid = constants.RES_BLUE;
		//else if (keyChar == 'G') resid = constants.RES_GREEN;
		//else if (keyChar == 'R') resid = constants.RES_RED;
		//else if (keyChar == 'Y') resid = constants.RES_YELLOW;
		
		//if (resid) this.setState({lastRes:vid});
		//console.log('here');
			

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