import { html } from '../../lib/preact.js';

export function TurnUI (props) {		
	return (
		html`
			<text class="label"
				x=${this.props.x}
				y=${this.props.y} 							
			>${this.props.label}${this.props.value}</text>
		`
	);	
}