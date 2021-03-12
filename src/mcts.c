#include "mcts.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>


#define DEFAULT_ITERATIONS 150000
#define DEFAULT_MAX_SIM_DEPTH 500
#define DEFAULT_UCTC 2
#define DEFAULT_MULTIACTION true
#define DEFAULT_SAVE_TREE false


struct MCTSOptions options;
struct MCTSResults *results;


struct Node {
    bool expanded;
    unsigned int visits;
    float value;

    struct Node **children;
    uint_fast8_t childrenCount;

    uint8_t depth;
};


/**
 * mallocs, checks for null, and increases results.stats.treeBytes
 */
void *mctsmalloc(size_t size)
{
    void *ptr = malloc(size);
    if (ptr == NULL) {
        fprintf(stderr, "ERROR: failure to malloc in MCTS\n");
        exit(1);
    }
    results->stats.treeBytes += size;
    return ptr;
}


void MCTSOptions_default(struct MCTSOptions *o)
{
        o->iterations = DEFAULT_ITERATIONS;
        o->uctc = DEFAULT_UCTC;
        o->maxSimDepth = DEFAULT_MAX_SIM_DEPTH;
        o->multiaction = DEFAULT_MULTIACTION;
        o->saveTree = DEFAULT_SAVE_TREE;
}


void Node_init(struct Node *node, uint8_t depth)
{
    node->expanded = false;
    node->visits = 0;
    node->value = 0;
    // TODO we probably could pass this around mcts() and iterate()
    // instead of storing it here
    node->depth = depth;

    results->stats.nodes++;
    if (depth > results->stats.treeDepth) {
        results->stats.treeDepth = depth;
    }
}


/**
 * allocates space for children pointers and the child nodes themselves,
 * and calls Node_init on each child
 */
void Node_expand(struct Node *node, const struct State *state)
{
    node->childrenCount = state->actionCount;
    node->children = mctsmalloc(sizeof(struct Node*) * node->childrenCount);

    for (int i = 0; i < state->actionCount; i++) {
        node->children[i] = mctsmalloc(sizeof(struct Node));
        Node_init(node->children[i], node->depth+1);
    }

    node->expanded = true;
}


/**
 * frees the node's children along with the node
 */
void Node_free(struct Node *node)
{
    if (node->expanded) {
        for (int i = 0; i < node->childrenCount; i++) {
            Node_free(node->children[i]);
        }
        free(node->children);
    }

    free(node);
}


unsigned int dumpTree(FILE *fp, const struct Node *root,
        const struct State *state, unsigned int id)
{
    int rootID = id++;
    int childIDs[MAX_ACTIONS];

    char stateString[STATE_STRING_SIZE];
    State_toString(state, stateString);

    if (!root->expanded) {
        fprintf(fp, "node %d\n", rootID);
        fprintf(fp, "string %s\n", stateString);
        fprintf(fp, "\n");
        return id;
    }

    for (int i = 0; i < state->actionCount; i++) {
        childIDs[i] = id;
        struct State s = *state;
        State_act(&s, &state->actions[i]);
        id = dumpTree(fp, root->children[i], &s, id);
    }

    fprintf(fp, "node %d\n", rootID);
    fprintf(fp, "value %f\n", root->value);
    fprintf(fp, "visits %d\n", root->visits);
    for (int i = 0; i < state->actionCount; i++) {
        char actionString[ACTION_STRING_SIZE];
        Action_toString(&state->actions[i], actionString);
        fprintf(fp, "child %s %d\n", actionString, childIDs[i]);
    }
    fprintf(fp, "\n");

    return id;
}


/**
 * simulates play (in place) on a state, stopping at game end or
 * MAX_SIM_DEPTH, and returns 1.0 if the initial state won, -1.0 if it
 * lost, and 0.0 on a draw or depth out
 */
float simulate(struct State *state)
{
    results->stats.simulations++;
    enum Player turn = state->turn;

    int depth = 0;
    while (state->actionCount) {
        // TODO We might be able to avoid this if we keep track of
        // stuck states, such as where every square with a node is
        // exhausted
        if (depth++ > options.maxSimDepth) {
            results->stats.depthOuts++;
            return 0.0;
        }

        struct Action action = state->actions[rand() % state->actionCount];
        State_act(state, &action);
    }

    if (state->score[turn] >= WIN_SCORE) {
        return 1.0;
    } else if (state->score[!turn] >= WIN_SCORE) {
        return -1.0;
    } else {
        return 0.0;
    }
}


/**
 * single MCTS iteration: recursively walk down tree with state
 * (choosing promising children), simulate when we get to the end of the
 * tree, and update visited nodes with the results
 */
float iterate(struct Node *root, struct State *state)
{
    // game-terminal node
    if (state->actionCount == 0) {
        float score;
        // a winner can only be the current player
        if (State_currentPlayerWon(state)) {
            score = 1.0;
        } else {
            score = 0.0;
        }

        root->visits++;
        root->value += score;
        return score;
    }

    if (!root->expanded) {
        Node_expand(root, state);
    }

    if (root->visits == 0) {
        float score = simulate(state);

        root->visits++;
        root->value += score;
        return score;
    }

    int childIndex = 0;
    float bestUCT = -INFINITY;
    for (int i = 0; i < state->actionCount; i++) {
        if (root->children[i]->visits == 0) {
            childIndex = i;
            break;
        }

        int scoreSign = Action_changesTurn(&state->actions[i], state) ? -1 : 1;
        float uct =
            scoreSign*root->children[i]->value/root->children[i]->visits
            + options.uctc*sqrtf(logf(root->visits)/root->children[i]->visits);

        if (uct >= bestUCT) {
            bestUCT = uct;
            childIndex = i;
        }
    }

    enum Player turn = state->turn;

    struct Node *child = root->children[childIndex];
    State_act(state, &state->actions[childIndex]);

    float scoreSign = 1;
    if (state->turn != turn) {
        scoreSign = -1;
    }
    float score = scoreSign * iterate(child, state);

    root->visits++;
    root->value += score;
    return score;
}


/**
 * selects an action to play from state, using MCTS and the provided
 * tree; returns the index of the action
 */
int mctsAction(const struct State *state, struct Node *root)
{
    struct timeval start;
    gettimeofday(&start, NULL);

    for (int i = 0; i < options.iterations; i++) {
        struct State s = *state;
        iterate(root, &s);

        results->stats.iterations++;
        results->actionStats[results->actionCount].iterations++;
    }

    float bestScore = -INFINITY;
    int bestActionIndex = 0;
    for (int i = 0; i < state->actionCount; i++) {
        int scoreSign = Action_changesTurn(&state->actions[i], state) ? -1 : 1;
        float score = scoreSign * root->children[i]->value / root->children[i]->visits;

        // The second conditional clause is there so we don't always
        // choose END if we're about to win and all actions are valued
        // the same
        if (score >= bestScore
                && (score != bestScore || state->actions[i].type != END)) {
            bestScore = score;
            bestActionIndex = i;
        }
    }

    struct timeval end;
    gettimeofday(&end, NULL);
    int duration = (end.tv_sec - start.tv_sec)*1000 + (end.tv_usec - start.tv_usec)/1000;

    results->actionStats[results->actionCount].duration = duration;

    return bestActionIndex;
}


struct Node* mctsAct(struct State *state, const struct Node *node,
        int actionIndex)
{
    struct MCTSActionStats *stats = &results->actionStats[results->actionCount];
    struct Node *child = node->children[actionIndex];
    int valueSign = Action_changesTurn(&state->actions[actionIndex], state) ? -1 : 1;
    stats->actionCount = state->actionCount;
    stats->value = valueSign*child->value;
    stats->visits = child->visits;

    results->actions[results->actionCount++] = state->actions[actionIndex];
    State_act(state, &state->actions[actionIndex]);

    return node->children[actionIndex];
}


void mcts(const struct State *s, struct MCTSResults *r, const struct MCTSOptions *o)
{
    struct State state = *s;

    results = r;
    memset(results, 0, sizeof(struct MCTSResults));

    if (o == NULL) {
        MCTSOptions_default(&options);
    } else {
        options = *o;
    }

    if (state.actionCount == 0) {
        return;
    }

    struct Node *root = mctsmalloc(sizeof(struct Node));
    Node_init(root, 0);
    Node_expand(root, &state);

    struct timeval start;
    gettimeofday(&start, NULL);

    enum Player turn = state.turn;
    struct Node *node = root;
    do {
        if (state.actionCount == 1) {
            node = mctsAct(&state, node, 0);
            continue;
        }

        int actionIndex = mctsAction(&state, node);
        node = mctsAct(&state, node, actionIndex);
    } while (options.multiaction
        && state.turn == turn && state.actionCount > 0);

    struct timeval end;
    gettimeofday(&end, NULL);
    results->stats.duration = (end.tv_sec - start.tv_sec)*1000 + (end.tv_usec - start.tv_usec)/1000;

    results->state = state;

    if (options.saveTree) {
        results->tree = root;
    } else {
        Node_free(root);
        results->tree = NULL;
    }
}
