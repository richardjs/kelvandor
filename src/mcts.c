#include "mcts.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>


struct Stats {
    unsigned int nodes;
    unsigned long long treeBytes;
    unsigned int maxTreeDepth;
    unsigned int simulations;
    unsigned int depthOuts;
};
struct Stats stats;


void Node_init(struct Node *node) {
    node->expanded = false;
    node->visits = 0;
    node->value = 0;

    stats.nodes++;
}


void Node_expand(struct Node *node, const struct State *state) {
    node->childrenCount = state->actionCount;
    node->children = malloc(sizeof(struct Node*) * node->childrenCount);
    if (node->children == NULL) {
        fprintf(stderr, "malloc failed in Node_expand\n");
        exit(6);
    }
    stats.treeBytes += sizeof(struct Node*) * node->childrenCount;

    for (int i = 0; i < state->actionCount; i++) {
        node->children[i] = malloc(sizeof(struct Node));
        if (node->children[i] == NULL) {
            fprintf(stderr, "malloc failed in Node_expand\n");
            exit(3);
        }
        stats.treeBytes += sizeof(struct Node);
        Node_init(node->children[i]);
    }
    node->expanded = true;
}


void Node_free(struct Node *node) {
    if (node->expanded) {
        for (int i = 0; i < node->childrenCount; i++) {
            Node_free(node->children[i]);
        }
    }
    free(node->children);
    free(node);
}


float simulate(struct State *state) {
    stats.simulations++;
    enum Player turn = state->turn;

    int depth = 0;
    while (state->actionCount) {
        // TODO We might be able to avoid this if we keep track of
        // stuck states, such as where every square with a node is
        // exhausted
        if (depth++ > MAX_SIM_DEPTH) {
            stats.depthOuts++;
            return 0.0;
        }

        struct Action action = state->actions[rand() % state->actionCount];
        State_act(state, &action);
    }

    if (state->score[turn] >= 10) {
        return 1.0;
    } else if (state->score[!turn] >= 10) {
        return -1.0;
    } else {
        return 0.0;
    }
}


float iterate(struct Node *root, struct State *state, unsigned int depth) {
    if (depth > stats.maxTreeDepth) {
        stats.maxTreeDepth = depth;
    }

    if (!root->expanded) {
        Node_expand(root, state);
    }

    if (state->actionCount == 0) {
        // TODO we use this branch a couple times; probably should be a function
        float score;
        if (state->score[state->turn] >= WIN_SCORE) {
            score = INFINITY;
        } else if (state->score[!state->turn] >= WIN_SCORE) {
            score = -INFINITY;
        } else {
            score = 0.0;
        }

        root->visits++;
        root->value += score;
        return score;
    }

    if (root->visits == 0) {
        float score = simulate(state);

        root->visits++;
        root->value += score;
        return score;
    }

    // TODO implement uct
    struct Node *child = NULL;
    int childIndex = 0;
    for (int i = 0; i < state->actionCount; i++) {
        if (root->children[i]->visits == 0) {
            child = root->children[i];
            childIndex = i;
            break;
        }
    }
    if (child == NULL) {
        childIndex = rand() % state->actionCount;
        child = root->children[childIndex];
    }

    enum Player turn = state->turn;
    State_act(state, &state->actions[childIndex]);
    float scoreSign = 1;
    if (state->turn != turn) {
        scoreSign = -1;
    }
    float score = scoreSign * iterate(child, state, depth+1);

    root->visits++;
    root->value += score;
    return score;
}


unsigned int dumpTree(FILE *fp, const struct Node *root, const struct State *state, unsigned int id) {
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


void mcts(const struct State *state, struct Node *root) {
    fprintf(stderr, "Monte Carlo tree search\n");

    bool ownRoot = root == NULL;
    if (root == NULL) {
        fprintf(stderr, "Creating new tree\n");

        memset(&stats, 0, sizeof(struct Stats));

        root = malloc(sizeof(struct Node));
        if (root == NULL) {
            fprintf(stderr, "root malloc failed in mcts\n");
            exit(4);
        }
        stats.treeBytes += sizeof(struct Node);
        Node_init(root);
    } else {
        fprintf(stderr, "Using existing tree\n");
    }

    struct timeval start;
    gettimeofday(&start, NULL);

    for (int i = 0; i < ITERATIONS; i++) {
        struct State s = *state;
        iterate(root, &s, 0);

        if (i % (ITERATIONS/10) == 0) {
            fprintf(stderr, ".");
        }
    }
    fprintf(stderr, "\n");

    struct timeval end;
    gettimeofday(&end, NULL);
    int duration = (end.tv_sec - start.tv_sec)*1000 + (end.tv_usec - start.tv_usec)/1000;

    float bestScore = -INFINITY;
    const struct Action *bestAction = NULL;
    struct Node *bestChild = NULL;
    int bestIndex = 0;
    for (int i = 0; i < state->actionCount; i++) {
        const struct Action *action = &state->actions[i];

        float scoreSign = 1;
        struct State s = *state;
        State_act(&s, action);
        if (state->turn != s.turn) {
            scoreSign = -1;
        }
        float score = scoreSign * root->children[i]->value / root->children[i]->visits;

        // The second conditional clause is there so we don't always
        // choose END if we're about to win and all actions are valued
        // the same
        if (score >= bestScore && (bestAction == NULL || action->type != END)) {
            bestScore = score;
            bestAction = action;
            bestChild = root->children[i];
            bestIndex = i;
        }
    }

    char actionString[ACTION_STRING_SIZE];
    Action_toString(bestAction, actionString);
    printf("%s\n", actionString);

    fprintf(stderr, "Input state:\n");
    State_print(state);
    char stateString[STATE_STRING_SIZE];
    State_toString(state, stateString);
    fprintf(stderr, "%s\n", stateString);

    fprintf(stderr, "\n");
    fprintf(stderr, "action:\t\t%s\n", actionString);
    fprintf(stderr, "value:\t\t%f\n", bestScore);
    fprintf(stderr, "visits:\t\t%d\n", bestChild->visits);
    fprintf(stderr, "iterations:\t%d\n", ITERATIONS);
    fprintf(stderr, "time:\t\t%d ms\n", duration);
    fprintf(stderr, "iter/s:\t\t%f\n", (float)ITERATIONS/duration*1000);
    fprintf(stderr, "actions:\t%d\n", state->actionCount);
    fprintf(stderr, "nodes:\t\t%d\n", stats.nodes);
    fprintf(stderr, "tree size:\t%lld MiB\n", stats.treeBytes / 1024 / 1024);
    fprintf(stderr, "max tree depth:\t%d\n", stats.maxTreeDepth);
    fprintf(stderr, "simulations:\t%d\n", stats.simulations);
    fprintf(stderr, "depth out pct:\t%f%%\n", (float)stats.depthOuts/stats.simulations);
    fprintf(stderr, "\n");

    struct State afterState = *state;
    State_act(&afterState, bestAction);

    fprintf(stderr, "Output state:\n");
    State_print(&afterState);
    State_toString(&afterState, stateString);
    fprintf(stderr, "%s\n", stateString);

    #ifdef KELV_LOGACTIONVALUES
    fprintf(stderr, "action values:\n");
    for (int i = 0; i < root->state.actionCount; i++) {
        Action_toString(&root->state.actions[i], actionString);
        fprintf(stderr, "%s\t%f\n", actionString,
            root->children[i]->value / root->children[i]->visits);
    }
    #endif

    #ifdef KELV_DUMPTREE
    FILE* fp = fopen("tree.txt", "w");
    if (!fp) {
        fprintf(stderr, "WARNING: Couldn't dump tree to tree.txt\n");
    } else {
        fprintf(stderr, "Dumping tree to tree.txt...\n");
        dumpTree(fp, root, state, 0);
        fclose(fp);
        fprintf(stderr, "Done\n");
    }
    #endif

    if (state->turn == afterState.turn && MULTIACTION) {
        struct Node* next = root->children[bestIndex];
        // TODO Free up parts of the tree that we won't use after recursing
        mcts(&afterState, next);
    }

    if (ownRoot) {
        Node_free(root);
    }
}
