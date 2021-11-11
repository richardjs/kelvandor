import { html } from '../../lib/preact.js';


const PRECISION = 2;
export function AnalyzeUI (props) {	
	
	if (typeof(props.val) == 'undefined')	return;
	var x = Number.parseInt(props.x);
	var y = Number.parseInt(props.y);
	var val = Number.parseFloat(props.val).toFixed(PRECISION);
	var text = (props.text)? props.text + val : val;
	return (
		html`
			<text class="lblAnalyze"
				x=${x}
				y=${y}
			>${text}</text>
		`
	);	
	
}