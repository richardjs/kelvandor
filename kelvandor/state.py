from enum import Enum
from itertools import product
from random import shuffle


MAX_LIMIT = 3


class Player(Enum):
    ONE = 0
    TWO = 1


class Resource(Enum):
    RED = 0
    BLUE = 1
    YELLOW = 2
    GREEN = 3


class Square:
    def __init__(self, resource: Resource, limit: int):
        self.resource = resource
        self.limit = limit


def shuffled_squares() -> [Square]:
    """Create a standard randomized starting layout of squares."""
    squares = [Square(r, l) for r, l in product(Resource, range(1, MAX_LIMIT))]
    squares.append(Square(Resource.RED, 0))
    shuffle(squares)
    return squares


class State:
    def __init__(self):
        # Static information
        self.squares = shuffled_squares()

        # Core information
        self.nodes = [0 for player in Player]
        self.branches = [0 for _ in Player]
        self.resources = [[0 for _ in Resource] for _ in Player]
        self.turn = Player.ONE

        # Information derived from above
        self.income = [[0 for _ in Resource] for _ in Player]
        self.captured = [0 for _ in Player]
        self.score = [0 for _ in Player]


State()
