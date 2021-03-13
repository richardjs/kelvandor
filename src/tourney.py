import argparse
import pickle
import time
from datetime import timedelta
from itertools import combinations
from random import shuffle
from statistics import mean
from subprocess import Popen, PIPE
from sys import stdout


results = []


class Results:
    def __init__(self, players, winner, loser, turns, time):
        self.players = players
        self.winner = winner
        self.loser = loser
        self.turns = turns
        self.time = time

    def __repr__(self):
        if self.winner == self.players[0]:
            winner_order = 'P1'
            loser_order = 'P2'
        else:
            winner_order = 'P2'
            loser_order = 'P1'
        return f'{winner_order} {self.winner} beat {loser_order} {self.loser} in {self.turns} turns ({self.time:.2f}s)'


class Engine:
    def __init__(self, cmd):
        self.cmd = cmd
        self.times = []

    def run(self, args, record_time=False):
        start = time.perf_counter()

        p = Popen(self.cmd.split() + args.split(), stdout=PIPE, stderr=PIPE)
        p.wait()

        if record_time:
            self.times.append(time.perf_counter() - start)

        return (
            p.stdout.read().decode('utf-8').strip(),
            p.stderr.read().decode('utf-8').strip(),
        )

    def move(self, state):
        _, stderr = self.run(state, record_time=True)

        lines = stderr.split('\n')
        state = lines[-1]
        winner = False
        if state.startswith('winner:'):
            winner = True
            state = lines[-2]

        return state, winner

    def __repr__(self):
        return self.cmd


def play_game(engine1, engine2, initial_state):
    start = time.perf_counter()

    engines = [engine1, engine2]
    state = initial_state

    turns = 0
    while True:
        engine = engines.pop(0)
        engines.append(engine)

        turns += 1
        stdout.write((f'\r{engine1} versus {engine2} turn {turns}'))
        state, winner = engine.move(state)

        if winner:
            results.append(Results(
                players=(engine1, engine2),
                winner=engine,
                loser=engines[0],
                turns=turns,
                time=time.perf_counter() - start,
            ))
            print(f'\r{results[-1]}')
            return engine


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-e', '--engine', action='append')
    parser.add_argument('-n', '--game-pairs', type=int, default=10)
    parser.add_argument('-w', '--results-file', type=argparse.FileType('wb'))
    args = parser.parse_args()

    engines = []
    for engine_cmd in args.engine:
        engines.append(Engine(engine_cmd))

    initial_states = []
    for _ in range(args.game_pairs):
        initial_state, _ = engines[0].run('-g')
        initial_states.append(initial_state)

    total_games = 2 * len(initial_states) * len(list(combinations(engines, 2)))
    played_games = 0

    matchups = list(combinations(engines, 2))
    shuffle(matchups)
    for engine1, engine2 in matchups:
        for initial_state in initial_states:
            for p1, p2 in [(engine1, engine2), (engine2, engine1)]:
                play_game(p1, p2, initial_state)
                played_games += 1

                percentage = 100 * played_games / total_games
                mean_time = mean([result.time for result in results])
                remaining_games = total_games - played_games
                etr = timedelta(seconds=int(mean_time * remaining_games))
                print(f'{played_games}/{total_games}\t{percentage:.2f}%\tapproximately {etr} remaining')

    if args.results_file:
        pickle.dump(results, args.results_file)


if __name__ == '__main__':
    main()
