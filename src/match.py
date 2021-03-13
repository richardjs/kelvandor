import argparse
from subprocess import Popen, PIPE


class Engine:
    def __init__(self, cmd):
        self.cmd = cmd

    def run(self, args):
        print(self.cmd, args)
        p = Popen(self.cmd.split() + args.split(), stdout=PIPE, stderr=PIPE)

        stderr = p.stderr.read().decode('utf-8').strip()
        print(stderr)

        return (
            p.stdout.read().decode('utf-8').strip(),
            stderr,
        )

    def move(self, state):
        _, stderr = self.run(state)
        lines = stderr.split('\n')

        state = lines[-1]
        winner = False
        if state.startswith('winner:'):
            winner = True
            state = lines[-2]

        return state, winner


def play_game(start, engine1, engine2):
    engines = [engine1, engine2]
    state = start

    while True:
        engine = engines.pop(0)
        engines.append(engine)

        state, winner = engine.move(state)

        if winner:
            return engine


def play_two_games(engine1, engine2):
    start, _ = engine1.run('-g')
    play_game(start, engine1, engine2)
    #play_game(start, engine2, engine1)


parser = argparse.ArgumentParser()
parser.add_argument('engine_cmd1')
parser.add_argument('engine_cmd2')
args= parser.parse_args()


engine1 = Engine(args.engine_cmd1)
engine2 = Engine(args.engine_cmd2)

play_two_games(engine1, engine2)
