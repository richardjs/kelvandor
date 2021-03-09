import { html, Component } from '../../lib/preact.js';
import {COUNT_RES} from '../core/constants.js';
import {UNIT_TRADE, RES_COLORS} from './constants-ui.js';

const SIZE_RES = 30;
const UNIT_RES = 40;

//This is just a small one to display the oposing player's resources
export class ResMiniUI extends Component {
				
	
	renderResource = (x, y, color, value) => {
		return (
			html`<rect class="btn"
					x=${x}
					y=${y}
					width=${SIZE_RES}
					height=${SIZE_RES}
					fill=${color}					
				/>
				<text class="label small"
					x=${x+2}
					y=${y+20} 							
				>${value}</text>
				`					
		);
	}

	render() {		
		var x = Number.parseInt(this.props.x);
		var y = Number.parseInt(this.props.y);
		
		var elements = [];	
		
		for (var resid = 0; resid < COUNT_RES; resid++) {			
			var resX = x + (resid * UNIT_RES);
			var value = this.props.res[resid];			
			var color = RES_COLORS[resid];		
			elements.push(this.renderResource(resX, y, color, value));
		}		
		
		
		return elements;
	}
	
}