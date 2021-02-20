from enum import Enum, auto

from kelvandor.state import Resource


class Actions(Enum):
    TRADE = auto()
    BRANCH = auto()
    NODE = auto()
    END = auto()


class Action:
    pass


class TradeAction(Action):
    def __init__(self, trade_in: [Resource], trade_out: Resource):
        self.trade_in = trade_in
        self.trade_out = trade_out

    def apply(self, state):
        for resource in self.trade_in:
            state.resources[state.turn][resource] -= 1
        state.resources[state.turn][self.trade_out] += 1


class BranchAction(Action):
    def __init__(self):
        self.type = Actions.BRANCH


class NodeAction(Action):
    def __init__(self):
        self.type = Actions.NODE


class EndAction(Action):
    def __init__(self):
        super().__init__()
        self.type = Actions.END
