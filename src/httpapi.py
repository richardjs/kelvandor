from subprocess import Popen, PIPE

from flask import Flask, jsonify


KELVANDOR = './kelvandor'


app = Flask(__name__)


@app.route('/<board>')
def think(board):
    assert board.isalnum()

    p = Popen((KELVANDOR, board), stdout=PIPE, stderr=PIPE)

    actions = []
    for action in p.stdout.readlines():
        actions.append(action.strip().decode('utf-8'))

    log = p.stderr.read().decode('utf-8')

    with open('httpapi.log', 'a') as f:
        f.write(log)

    response = jsonify({
        'actions': actions,
        'log': log,
    })

    response.headers.add("Access-Control-Allow-Origin", "*")
    return response
