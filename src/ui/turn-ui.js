import { html } from '../../lib/preact.js';
import { PHASE_PLAY, PHASE_PLACE1_1, PHASE_PLACE2_1, PHASE_PLACE2_2, PHASE_PLACE1_2} from '../core/constants.js';

export function TurnUI (props) {	
	var text;
	var y = Number.parseInt(props.y) + 20;
	if (props.phase == PHASE_PLAY) {
		text = props.label +  props.value;
		y -= 20;
	}
	else if (props.phase == PHASE_PLACE1_1) text = 'Player 1: Place first node and road';
	else if (props.phase == PHASE_PLACE2_1) text = 'Player 2: Place first node and road';
	else if (props.phase == PHASE_PLACE2_2) text = 'Player 2: Place second node and road';
	else if (props.phase == PHASE_PLACE1_2) text = 'Player 1: Place second node and road';
	return (
		html`
			<text class="label"
				x=${props.x}
				y=${y}
			>${text}</text>
		`
	);	
	
}