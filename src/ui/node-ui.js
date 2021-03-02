import { html, Component } from '../../lib/preact.js';
import {SIDE_NONE} from '../core/constants.js';
import {SIZE_NODE} from './constants-ui.js';

const OFFSET_NODE = 50;

export class NodeUI extends Component {
	
	onClick = (e) => {
		this.props.click(this.props.nid);	
	}
	
	render() {
		var cssClass = (this.props.side == SIDE_NONE)? 'node empty' : 'node';
		return (
			html`<circle 
					class=${cssClass}
					cx=${OFFSET_NODE + this.props.x} 
					cy=${OFFSET_NODE + this.props.y}
					r=${SIZE_NODE}
					fill=${this.props.color} 
					onclick=${this.onClick}
			/>`
		);
	}
	
}