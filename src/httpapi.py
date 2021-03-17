from subprocess import Popen, PIPE

from flask import Flask, jsonify, request, Response


KELVANDOR = './kelvandor'
MAX_ITERATIONS = 300000
MIN_ITERATIONS = 100


app = Flask(__name__)


@app.route('/<board>', methods=['OPTIONS'])
def options(board):
    response = Response()
    response.headers['Access-Control-Allow-Origin'] = '*'
    response.headers['Access-Control-Allow-Headers'] = 'Iterations'

    return response


@app.route('/<board>')
def think(board):
    assert board.isalnum()

    args = []
    if 'Iterations' in request.headers:
        try:
            args += ['-i', str(
                max(min(int(request.headers['Iterations']),
                        MAX_ITERATIONS), MIN_ITERATIONS)
            )]
        except ValueError:
            pass

    p = Popen([KELVANDOR, board] + args, stdout=PIPE, stderr=PIPE)

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

    response.headers['Access-Control-Allow-Origin'] = '*'
    response.headers['Access-Control-Allow-Headers'] = 'Iterations'

    return response
