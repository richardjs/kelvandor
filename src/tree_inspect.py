class Node:
    def __init__(self, node_id):
        self.id = node_id
        self.string = None
        self.value = None
        self.visits = None
        self.children = []

    @property
    def score(self):
        if self.value and self.visits:
            return self.value/self.visits
        else:
            return 0.0

    def __str__(self):
        return f'node {self.id}, score {self.score:.2f}, visits {self.visits}, {len(self.children)} children'

    def __repr__(self):
        return str(self)


print('Loading tree...')

nodes = {}

node_id = None
node = None
for line in open('tree.txt'):
    line = line.strip().lower()

    if not line:
        continue

    key, value = line.split(' ')

    if key == 'node':
        node_id = int(value)
        node = Node(node_id)
        nodes[node_id] = node
    elif key == 'string':
        node.string = value
    elif key == 'value':
        node.value = float(value)
    elif key == 'visits':
        node.visits = int(value)
    elif key == 'child':
        node.children.append(nodes[int(value)])
    else:
        print('Unknown key:', key)

for node in nodes.values():
    node.children.sort(key=lambda n: n.score, reverse=True)

root = nodes[0]

print('Done! Access root node at `root`.')
