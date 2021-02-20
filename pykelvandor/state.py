from enum import IntEnum
from itertools import product
from random import shuffle


MAX_SQUARE_LIMIT = 3
NUM_SQUARES = 13
NUM_CORNERS = 24
NUM_EDGES = 36


class Player(IntEnum):
    ONE = 0
    TWO = 1


class Resource(IntEnum):
    RED = 0
    BLUE = 1
    YELLOW = 2
    GREEN = 3

    @property
    def char(self):
        return 'RBYG'[self]


class Square:
    def __init__(self, resource: Resource, limit: int):
        self.resource = resource
        self.limit = limit

    def __str__(self):
        return f'{self.resource.char}{self.limit}'


def shuffled_squares() -> [Square]:
    """Create a standard randomized starting layout of squares."""
    squares = [Square(r, l) for r, l in product(
        Resource, range(1, MAX_SQUARE_LIMIT+1))]
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

    def __str__(self):
        c = []
        for i in range(NUM_CORNERS):
            corner_chars = '.'
            if (1 << i) & self.nodes[Player.ONE]:
                corner_chars = 'X'
            if (1 << i) & self.nodes[Player.TWO]:
                if corner_chars == 'X':
                    corner_chars = '?'
                else:
                    corner_chars = 'O'
            c.append(corner_chars)

        e = []
        for i in range(NUM_EDGES):
            edge_char = ' '
            if (1 << i) & self.branches[Player.ONE]:
                edge_char = 'x'
            if (1 << i) & self.branches[Player.TWO]:
                if edge_char == 'x':
                    edge_char = '?'
                else:
                    edge_char = 'O'
            e.append(edge_char)

        r = []
        for i in range(NUM_SQUARES):
            square = self.squares[i]
            if square.limit > 0:
                square_string = str(square)
            else:
                square_string = '  '
            r.append(square_string)

        return f'''      {c[0]}{e[0]}{e[0]}{c[1]}
      {e[1]}{r[0]}{e[2]}
   {c[2]}{e[3]}{e[3]}{c[3]}{e[4]}{e[4]}{c[4]}{e[5]}{e[5]}{c[5]}
   {e[7]}{r[1]}{e[7]}{r[2]}{e[8]}{r[3]}{e[9]}
{c[6]}{e[10]}{e[10]}{c[7]}{e[11]}{e[11]}{c[8]}{e[12]}{e[12]}{c[9]}{e[13]}{e[13]}{c[10]}{e[14]}{e[14]}{c[11]}
{e[15]}{r[4]}{e[16]}{r[5]}{e[17]}{r[6]}{e[18]}{r[7]}{e[19]}{r[8]}{e[20]}
{c[12]}{e[21]}{e[21]}{c[13]}{e[22]}{e[22]}{c[14]}{e[23]}{e[23]}{c[15]}{e[24]}{e[24]}{c[16]}{e[25]}{e[25]}{c[17]}
   {e[26]}{r[9]}{e[27]}{r[10]}{e[28]}{r[11]}{e[29]}
   {c[18]}{e[30]}{e[30]}{c[19]}{e[31]}{e[31]}{c[20]}{e[32]}{e[32]}{c[21]}
      {e[33]}{r[12]}{e[34]}
      {c[22]}{e[35]}{e[35]}{c[23]}'''


s = State()
print(s)
