import { html } from '../../lib/preact.js';
//import { } from '../core/constants.js';

const PRECISION = 2;
export function AnalyzeUI (props) {	
	
	if (typeof(props.val) == 'undefined')	return;
	var x = Number.parseInt(props.x);
	var y = Number.parseInt(props.y);
	var val = Number.parseFloat(props.val);
	return (
		html`
			<text class="lblAnalyze"
				x=${x}
				y=${y}
			>${val.toFixed(PRECISION)}</text>
		`
	);	
	
}