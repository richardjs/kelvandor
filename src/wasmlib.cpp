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

    vector<string> actions() {
        vector<string> vec;
        for (int i = 0; i < state.actionCount; i++) {
            char cstr[ACTION_STRING_SIZE];
            Action_toString(&state.actions[i], cstr);
            string str = cstr;
            vec.push_back(str);
        }
        return vec;
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
    register_vector<string>("vector<string>");

    class_<WasmState>("State")
        .constructor<>()
        .constructor<string>()
        .function("fromString", &WasmState::fromString)
        .function("toString", &WasmState::toString)
        .function("actions", &WasmState::actions)
        .function("print", &WasmState::print)
        ;
}

