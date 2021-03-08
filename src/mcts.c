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


struct Node {
    struct State state;

    bool expanded;
    struct Node **children;

    unsigned int visits;
    float value;
};


void Node_init(struct Node *node, const struct State *state) {
    node->state  = *state;
    node->expanded = false;
    node->visits = 0;
    node->value = 0;
    stats.nodes++;

    node->children = malloc(sizeof(struct Node*) * node->state.actionCount);
    if (node->children == NULL) {
        fprintf(stderr, "malloc failed in Node_init\n");
        exit(6);
    }
    stats.treeBytes += sizeof(struct Node*) * node->state.actionCount;
}


void Node_expand(struct Node *node) {
    for (int i = 0; i < node->state.actionCount; i++) {
        node->children[i] = malloc(sizeof(struct Node));
        if (node->children[i] == NULL) {
            fprintf(stderr, "malloc failed in Node_expand\n");
            exit(3);
        }
        stats.treeBytes += sizeof(struct Node);
        Node_init(node->children[i], &node->state);

        State_act(&node->children[i]->state, &node->state.actions[i]);
    }
    node->expanded = true;
}


void Node_free(struct Node *node) {
    if (node->expanded) {
        for (int i = 0; i < node->state.actionCount; i++) {
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


float iterate(struct Node *root, unsigned int depth) {
    if (depth > stats.maxTreeDepth) {
        stats.maxTreeDepth = depth;
    }

    if (root->expanded == false) {
        Node_expand(root);
    }

    if (root->state.actionCount == 0) {
        // TODO we use this branch a couple times; probably should be a
        // function
        float score;
        if (root->state.score[root->state.turn] >= WIN_SCORE) {
            score = INFINITY;
        } else if (root->state.score[!root->state.turn] >= WIN_SCORE) {
            score = -INFINITY;
        } else {
            score = 0.0;
        }

        root->visits++;
        root->value += score;
        return score;
    }

    if (root->visits == 0) {
        struct State state = root->state;
        float score = simulate(&state);

        root->visits++;
        root->value += score;
        return score;
    }

    // TODO implement uct
    struct Node *child = NULL;
    for (int i = 0; i < root->state.actionCount; i++) {
        if (root->children[i]->visits == 0) {
            child = root->children[i];
        }
    }
    if (child == NULL) {
        child = root->children[rand() % root->state.actionCount];
    }

    float score = iterate(child, depth+1);
    if (child->state.turn != root->state.turn) {
        score *= -1;
    }

    root->visits++;
    root->value += score;
    return score;
}


unsigned int dumpTree(FILE *fp, const struct Node *root, unsigned int id) {
    int rootID = id++;
    int childIDs[MAX_ACTIONS];

    char stateString[STATE_STRING_SIZE];
    State_toString(&root->state, stateString);

    if (!root->expanded) {
        fprintf(fp, "node %d\n", rootID);
        fprintf(fp, "string %s\n", stateString);
        fprintf(fp, "\n");
        return id;
    }

    for (int i = 0; i < root->state.actionCount; i++) {
        childIDs[i] = id;
        id = dumpTree(fp, root->children[i], id);
    }

    fprintf(fp, "node %d\n", rootID);
    fprintf(fp, "value %f\n", root->value);
    fprintf(fp, "visits %d\n", root->visits);
    for (int i = 0; i < root->state.actionCount; i++) {
        char actionString[ACTION_STRING_SIZE];
        Action_toString(&root->state.actions[i], actionString);
        fprintf(fp, "child %s %d\n", actionString, childIDs[i]);
    }
    fprintf(fp, "\n");

    return id;
}


int mcts(const struct State *state) {
    memset(&stats, 0, sizeof(struct Stats));

    struct Node *root = malloc(sizeof(struct Node));
    if (root == NULL) {
        fprintf(stderr, "root malloc failed in mcts\n");
        exit(4);
    }
    stats.treeBytes += sizeof(struct Node);
    Node_init(root, state);

    struct timeval start;
    gettimeofday(&start, NULL);

    // TODO Repeat iteratons on best children until it is not our turn
    // anymore, to reuse to data in the tree
    for (int i = 0; i < ITERATIONS; i++) {
        iterate(root, 0);

        if (i % (ITERATIONS/10) == 0) {
            fprintf(stderr, ".");
        }
    }
    fprintf(stderr, "\n");

    struct timeval end;
    gettimeofday(&end, NULL);
    int duration = (end.tv_sec - start.tv_sec)*1000 + (end.tv_usec - start.tv_usec)/1000;

    struct Action bestAction;
    float bestScore = -INFINITY;
    struct Node *bestChild = NULL;
    for (int i = 0; i < root->state.actionCount; i++) {
        float score = root->children[i]->value / root->children[i]->visits;
        if (score > bestScore) {
            bestAction = root->state.actions[i];
            bestScore = score;
            bestChild = root->children[i];
        }
    }

    char actionString[ACTION_STRING_SIZE];
    Action_toString(&bestAction, actionString);
    printf("%s\n", actionString);

    State_printDetail(state);
    fprintf(stderr, "action:\t\t%s\n", actionString);
    fprintf(stderr, "value:\t\t%f\n", bestScore);
    fprintf(stderr, "visits:\t\t%d\n", bestChild->visits);
    fprintf(stderr, "iterations:\t%d\n", ITERATIONS);
    fprintf(stderr, "time:\t\t%d ms\n", duration);
    fprintf(stderr, "iter/s:\t\t%f\n", (float)ITERATIONS/duration*1000);
    fprintf(stderr, "actions:\t%d\n", root->state.actionCount);
    fprintf(stderr, "nodes:\t\t%d\n", stats.nodes);
    fprintf(stderr, "tree size:\t%lld MiB\n", stats.treeBytes / 1024 / 1024);
    fprintf(stderr, "max tree depth:\t%d\n", stats.maxTreeDepth);
    fprintf(stderr, "simulations:\t%d\n", stats.simulations);
    fprintf(stderr, "depth outs:\t%d\n", stats.depthOuts);

    #ifdef KELV_LOGACTIONS
    fprintf(stderr, "action value dump:\n");
    for (int i = 0; i < root->state.actionCount; i++) {
        Action_toString(&root->state.actions[i], actionString);
        fprintf(stderr, "%s\t%f\n", actionString,
            root->children[i]->value / root->children[i]->visits);
    }
    #endif

    FILE* fp = fopen("tree.txt", "w");
    if (!fp) {
        fprintf(stderr, "WARNING: Couldn't dump tree to tree.txt\n");
    } else {
        fprintf(stderr, "Dumping tree to tree.txt...\n");
        dumpTree(fp, root, 0);
        fclose(fp);
        fprintf(stderr, "Done\n");
    }

    Node_free(root);
    return 0;
}
