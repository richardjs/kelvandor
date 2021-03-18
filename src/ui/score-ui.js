import { html } from '../../lib/preact.js';

export function ScoreUI (props) {
	var x = Number.parseInt(props.x);			
	var y = Number.parseInt(props.y);		
	
	return (
		html`
			<text class="label"
				x=${x}
				y=${y} 							
			>${this.props.label} ${props.value}</text>
			${showBreakdowns(x, y, props.info)}
		`
	);	
}

function showBreakdowns(x, y, info) {
	
	if (!menu.scoreInfo) return [];
	return [
		showInfoItem(x, y + 20, '- nodes:' + info.nodes),
		showInfoItem(x, y + 35, '- captured:' + info.captured),
		showInfoItem(x, y + 50, '- longest:' + info.longest),
	];
}

function showInfoItem(x, y, item){	
	return (
		html`
			<text class="label small"
				x=${x}
				y=${y} 							
			>${item}</text>			
		`
	);
}