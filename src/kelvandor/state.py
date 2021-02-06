import numpy as np


PLAYER_1, PLAYER_2 = range(2)


class State:
    def __init__(self):
        # TODO figure out how to store tiles

        self.nodes = np.zeros(2, dtype=np.uint32)
        self.branches = np.zeros(2, dype=np.uint64)

        self.turn = PLAYER_1

        # cached information
        tile_nodes = np.zeros(13, dtype=np.uint8)


    def open_node_spaces(self, player)
        return 
