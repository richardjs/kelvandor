//http://qndgames.com/wp-content/uploads/2018/10/Node-Manual-V4.55-Rule-Sheet-Website-Full-copy.pdf
//http://qndgames.com/wp-content/uploads/2018/10/Node-Demo-Rule-Sheet-FandB-Combined-V1.01-11x17.pdf
import { html, render, Component } from '../lib/preact.js';
import { BoardUI } from './ui/board-ui.js';
import { MenuManager} from './ui/menu.js';

var menuManager = new MenuManager();
menu = menuManager.properties;	//global


function App (props) {		
	return (
		html`
			<${BoardUI}/>
		`
	);	
}


render(html`<${App}/>`, document.getElementById('app'));

