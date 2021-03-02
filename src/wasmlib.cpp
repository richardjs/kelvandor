extern "C" {
#include "state.h"
}

#include <emscripten/bind.h>

#include <stdio.h>


using namespace emscripten;
using namespace std;


class WasmState {
    public:
    WasmState() {
        State_randomStart(&state);
    }

    WasmState(string str) {
        State_fromString(&state, str.c_str());
    }

    void fromString(string str) {
        State_fromString(&state, str.c_str());
    }

    string toString() {
        char cstr[STATE_STRING_SIZE];
        State_toString(&state, cstr);
        string str = cstr;
        return str;
    }

    int actionCount() {
        return state.actionCount;
    }

    Action getAction(int index) {
        return state.actions[index];
    }

    void act(Action action) {
        State_act(&state, &action);
    }

    void undo(Action action) {
        State_undo(&state, &action);
    }

    int score(int player) {
        return state.score[player];
    }

    void print() {
        State_printDetail(&state);
    }

    private:
    State state;
};


EMSCRIPTEN_BINDINGS(kelvandor) {
    enum_<ActionType>("ActionType")
        .value("START_PLACE", START_PLACE)
        .value("TRADE", TRADE)
        .value("BRANCH", BRANCH)
        .value("NODE", NODE)
        .value("END", END)
        ;

    value_object<Action>("Action")
        .field("type", &Action::type)
        .field("data", &Action::data)
        ;

    class_<WasmState>("State")
        .constructor<>()
        .constructor<string>()
        .function("fromString", &WasmState::fromString)
        .function("toString", &WasmState::toString)
        .function("actionCount", &WasmState::actionCount)
        .function("getAction", &WasmState::getAction)
        .function("act", &WasmState::act)
        .function("undo", &WasmState::undo)
        .function("print", &WasmState::print)
        ;
}

