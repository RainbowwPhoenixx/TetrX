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

Tmovement getNodeIthMove (Tnode *node, Tbyte i) {
  return node->moves[i];
}
void setNodeIthMove (Tnode *node, Tbyte i, Tmovement new_move) {
  node->moves[i] = new_move;
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
void setNodeNbOfChildren (Tnode *node, unsigned short new_nb_of_children) {
  node->nb_of_children = new_nb_of_children;
}

Tnode *getNodeIthChild (Tnode *node, unsigned short i) {
  return node->children[i];
}
void setNodeIthChild (Tnode *node, unsigned short i, Tnode *new_child) {
  node->children[i] = new_child;
  if (new_child != NULL) {
    setNodeChildID (node->children[i], i);
  }
}

bool getNodeAreChildrenGenerated (Tnode *node){
  return node->are_children_generated;
}
void setNodeAreChildrenGenerated (Tnode *node, bool new_val) {
  node->are_children_generated = new_val;
}

unsigned short getNodeChildID (Tnode *node) {
  return node->child_id;
}
void setNodeChildID (Tnode *node, unsigned short new_val){
  node->child_id = new_val;
}

Tnode *getNodeImmediateParent (Tnode* node) {
  return node->immediate_parent;
}
void setNodeImmediateParent (Tnode *node, Tnode *parent) {
  node->immediate_parent = parent;
}

float getNodeAccumulatedBoardValue (Tnode *node) {
  return node->accumulated_board_value;
}
void setNodeAccumulatedBoardValue (Tnode *node, float new_acc_value) {
  node->accumulated_board_value = new_acc_value;
}


Tnode *createNode (Tbot_board board, Tbyte nb_of_moves, Tmovement *moves, Tnode *parent) {
  Tnode *tree = calloc (1, sizeof(Tnode));

  setNodeBotBoard (tree, board);
  setNodeNbOfMoves (tree, nb_of_moves);
  setNodeAreChildrenGenerated (tree, false);
  setNodeNbOfChildren (tree, 0);
  setNodeImmediateParent (tree, parent);

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
