let callback = Module['onRuntimeInitialized'];
Module['onRuntimeInitialized'] = function() {
    Module.State.prototype.actions = function() {
        let array = new Array();
        let actions = this.actionVector();
        for (let i = 0; i < actions.size(); i++) {
            array.push(actions.get(i));
        }
        actions.delete();
        return array;
    };
    callback();
}
