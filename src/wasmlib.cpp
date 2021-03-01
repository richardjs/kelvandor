extern "C" {
#include "state.h"
}

#include <emscripten/bind.h>

#include <stdio.h>


class NodeState {
    public:
    NodeState() {
        State_randomStart(&state);
    }

    void print() {
        State_printDetail(&state);
    }

    int actionCount() {
        return state.actionCount;
    }

    private:
    State state;
};


EMSCRIPTEN_BINDINGS(kelvandor) {
    emscripten::class_<NodeState>("NodeState")
        .constructor<>()
        .function("print", &NodeState::print)
        .function("actionCount", &NodeState::actionCount)
    ;
}
