import numpy as np


def make_node_bitboard(bits):
    bitboard = np.uint32()
    for bit in bits:
        bitboard |= (1 << bit)
    return bitboard


def make_branch_bitboard(bits):
    bitboard = np.uint64()
    for bit in bits:
        bitboard |= (1 << bit)
    return bitboard


# branch_nodes[x] is a bitboard of nodes connected to branch x
branch_nodes = np.empty(36, dtype=np.uint32)
branch_nodes[0] = make_node_bitboard([0, 1])
branch_nodes[1] = make_node_bitboard([0, 3])
branch_nodes[2] = make_node_bitboard([1, 4])
branch_nodes[3] = make_node_bitboard([2, 3])
branch_nodes[4] = make_node_bitboard([3, 4])
branch_nodes[5] = make_node_bitboard([4, 5])
branch_nodes[6] = make_node_bitboard([2, 7])
branch_nodes[7] = make_node_bitboard([3, 8])
branch_nodes[8] = make_node_bitboard([4, 9])
branch_nodes[9] = make_node_bitboard([5, 10])
branch_nodes[10] = make_node_bitboard([6, 7])
branch_nodes[11] = make_node_bitboard([7, 8])
branch_nodes[12] = make_node_bitboard([8, 9])
branch_nodes[13] = make_node_bitboard([9, 10])
branch_nodes[14] = make_node_bitboard([10, 11])
branch_nodes[15] = make_node_bitboard([6, 12])
branch_nodes[16] = make_node_bitboard([7, 13])
branch_nodes[17] = make_node_bitboard([8, 14])
branch_nodes[18] = make_node_bitboard([9, 15])
branch_nodes[19] = make_node_bitboard([10, 16])
branch_nodes[20] = make_node_bitboard([11, 17])
branch_nodes[21] = make_node_bitboard([12, 13])
branch_nodes[22] = make_node_bitboard([13, 14])
branch_nodes[23] = make_node_bitboard([14, 15])
branch_nodes[24] = make_node_bitboard([15, 16])
branch_nodes[25] = make_node_bitboard([16, 17])
branch_nodes[26] = make_node_bitboard([13, 18])
branch_nodes[27] = make_node_bitboard([14, 19])
branch_nodes[28] = make_node_bitboard([15, 20])
branch_nodes[29] = make_node_bitboard([16, 21])
branch_nodes[30] = make_node_bitboard([18, 19])
branch_nodes[31] = make_node_bitboard([19, 20])
branch_nodes[32] = make_node_bitboard([20, 21])
branch_nodes[33] = make_node_bitboard([19, 22])
branch_nodes[34] = make_node_bitboard([20, 23])


# branch_branches[x] is a bitboard of branches adjacent to branch x
branch_branches = np.zeros(36, dtype=np.uint64)
branch_branches[0] = make_branch_bitboard([1, 2])
branch_branches[1] = make_branch_bitboard([0, 3, 4])
branch_branches[2] = make_branch_bitboard([0, 4, 5, 8])
branch_branches[3] = make_branch_bitboard([1, 4, 6, 7])
branch_branches[4] = make_branch_bitboard([1, 2, 3, 5, 7, 8])
branch_branches[5] = make_branch_bitboard([2, 4, 8, 9])
branch_branches[6] = make_branch_bitboard([3, 7, 10, 11, 16])
branch_branches[7] = make_branch_bitboard([1, 3, 4, 11, 12, 17])
branch_branches[8] = make_branch_bitboard([2, 4, 5, 12, 13, 18])
branch_branches[9] = make_branch_bitboard([5, 13, 14, 19])
branch_branches[10] = make_branch_bitboard([6, 11, 15, 16])
branch_branches[11] = make_branch_bitboard([6, 7, 10, 12, 16, 17])
branch_branches[12] = make_branch_bitboard([7, 8, 11, 13, 17, 18])
branch_branches[13] = make_branch_bitboard([8, 9, 12, 14, 18, 19])
branch_branches[14] = make_branch_bitboard([9, 13, 19, 20])
branch_branches[15] = make_branch_bitboard([10, 21])
branch_branches[16] = make_branch_bitboard([6, 10, 11, 21, 22, 26])
branch_branches[17] = make_branch_bitboard([7, 11, 12, 16, 18, 22, 23])
branch_branches[18] = make_branch_bitboard([8, 12, 13, 23, 24, 28])
branch_branches[19] = make_branch_bitboard([9, 13, 14, 24, 25, 29])
branch_branches[20] = make_branch_bitboard([14, 25])
branch_branches[21] = make_branch_bitboard([15, 16, 22, 26])
branch_branches[22] = make_branch_bitboard([16, 17, 21, 23, 26, 27])
branch_branches[23] = make_branch_bitboard([17, 17, 22, 24, 27, 28])
branch_branches[24] = make_branch_bitboard([18, 19, 23, 25, 28, 29])
branch_branches[25] = make_branch_bitboard([19, 20, 24, 29])
branch_branches[26] = make_branch_bitboard([21, 22, 30])
branch_branches[27] = make_branch_bitboard([17, 22, 23, 30, 31, 33])
branch_branches[28] = make_branch_bitboard([18, 23, 24, 31, 32, 34])
branch_branches[29] = make_branch_bitboard([24, 25, 28, 32])
branch_branches[30] = make_branch_bitboard([26, 27, 31, 33])
branch_branches[31] = make_branch_bitboard([27, 28, 30, 32, 33, 34])
branch_branches[32] = make_branch_bitboard([28, 29, 31, 34])
branch_branches[33] = make_branch_bitboard([30, 31, 35])
branch_branches[34] = make_branch_bitboard([31, 32, 33, 35])
branch_branches[35] = make_branch_bitboard([33, 34])
