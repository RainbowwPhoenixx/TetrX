#include "tree.h"

// Accessors
Tbot_board *getNodeBotBoard (Tnode *node) {
  return &(node->board);
}
void setNodeBotBoard (Tnode *node, Tbot_board new_board) {
  node->board = new_board;
}

Tbyte getNodeNbOfMoves (Tnode *node) {
  return node->nb_of_moves;
}
void setNodeNbOfMoves (Tnode *node, Tbyte new_nb_of_moves) {
  node->nb_of_moves = new_nb_of_moves;
}

Tbot_movement getNodeIthMove (Tnode *node, Tbyte i) {
  return node->moves[i];
}
void setNodeIthMove (Tnode *node, Tbyte i, Tbot_movement new_move) {
  node->moves[i] = new_move;
}

Tline_clear getNodeLinesCleared (Tnode *node) {
  return node->lines_cleared;
}
void setNodeLinesCleared (Tnode *node, Tline_clear lines) {
  node->lines_cleared = lines;
}

float getNodeBoardValue (Tnode *node) {
  return node->board_value;
}
void setNodeBoardValue (Tnode *node, float new_val) {
  node->board_value = new_val;
}

unsigned short getNodeNbOfChildren (Tnode *node) {
  return node->nb_of_children;
}
static void setNodeNbOfChildren (Tnode *node, unsigned short new_nb_of_children) {
  node->nb_of_children = new_nb_of_children;
}

Tnode *getNodeIthChild (Tnode *node, unsigned short i) {
  return node->children[i];
}
void addChildToNode (Tnode *node, Tnode *new_child) {
  node->children[node->nb_of_children] = new_child;
  node->nb_of_children++;
}
void setNodeIthChild (Tnode *node, unsigned short i, Tnode *new_child) {
  node->children[i] = new_child;
}
void computeChildrenInitialRanks (Tnode *node) {
  // Method to call when all children of node have been generated
  Tnode **children = node->children;
  for (Tbyte i = 0; i < getNodeNbOfChildren (node); i++) {
    setNodeInitialRank (children[i], i);
  }
}

bool getNodeAreChildrenGenerated (Tnode *node){
  return node->are_children_generated;
}
void setNodeAreChildrenGenerated (Tnode *node, bool new_val) {
  node->are_children_generated = new_val;
}

float getNodeAccumulatedBoardValue (Tnode *node) {
  return node->accumulated_board_value;
}
void setNodeAccumulatedBoardValue (Tnode *node, float new_acc_value) {
  node->accumulated_board_value = new_acc_value;
}

unsigned short getNodeInitialRank (Tnode *node) {
  return node->initial_rank;
}
void setNodeInitialRank (Tnode *node, unsigned short new_val){
  node->initial_rank = new_val;
}

Tnode *getNodeImmediateParent (Tnode* node) {
  return node->immediate_parent;
}
void setNodeImmediateParent (Tnode *node, Tnode *parent) {
  node->immediate_parent = parent;
}

static int partitionNodeChildren (Tnode *children[], int low_limit, int high_limit) {
  float pivot = getNodeAccumulatedBoardValue (children[high_limit]);
  int i = low_limit - 1;
  
  for (int j = low_limit; j < high_limit; j++) {
    if (getNodeAccumulatedBoardValue (children[j]) > pivot) {
      i++;
      // Swap j and i
      Tnode *tmp = children[j];
      children[j] = children[i];
      children[i] = tmp;
    }
  }
  
  Tnode *tmp = children[high_limit];
  children[high_limit] = children[i+1];
  children[i+1] = tmp;
  return i+1;
}
static void quicksortNodeChildren (Tnode *children[], int low_limit, int high_limit) {
  if (low_limit < high_limit) {
    int pivot = partitionNodeChildren (children, low_limit, high_limit);
    
    quicksortNodeChildren (children, low_limit, pivot-1);
    quicksortNodeChildren (children, pivot+1, high_limit);
  }
}
void sortNodeChildren (Tnode* node) {
  quicksortNodeChildren (node->children, 0, getNodeNbOfChildren (node)-1);
}

Tnode *createNode (Tbot_board board, Tbyte nb_of_moves, Tbot_movement *moves, Tnode *parent) {
  Tnode *tree = calloc (1, sizeof(Tnode));

  setNodeBotBoard (tree, board);
  setNodeNbOfMoves (tree, nb_of_moves);
  setNodeAreChildrenGenerated (tree, false);
  setNodeNbOfChildren (tree, 0);
  setNodeImmediateParent (tree, parent);
  setNodeAccumulatedBoardValue (tree, -1.0/0.0);

  for (int i = 0; i < nb_of_moves; i++) {
    setNodeIthMove (tree, i,  moves[i]);
  }

  return tree;
}
void freeNode (Tnode *tree) {
  // Frees the node and all its descendants
  for (Tbyte i = 0; i < getNodeNbOfChildren (tree); i++) {
    Tnode *node = getNodeIthChild (tree, i);
    if (node != NULL) {
      freeNode (node);
    }
  }

  free (tree);
}
