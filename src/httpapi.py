import json
from subprocess import Popen, PIPE

from flask import Flask


KELVANDOR = './kelvandor'


app = Flask(__name__)


@app.route('/<board>')
def think(board):
    assert board.isalnum()

    p = Popen((KELVANDOR, board), stdout=PIPE, stderr=PIPE)

    actions = []
    for action in p.stdout.readlines():
        actions.append(action.strip().decode('utf-8'))

    return json.dumps({
        'actions': actions,
        'log': p.stderr.read().decode('utf-8'),
    })
