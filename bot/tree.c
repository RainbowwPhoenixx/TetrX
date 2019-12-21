#include "tree.h"

// Accessors
Tboard *getNodeBoard (Tnode *node) {
  return &(node->board);
}
void setNodeBoard (Tnode *node, Tboard new_board) {
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

short getNodeNbOfChildren (Tnode *node) {
  return node->nb_of_children;
}
void setNodeNbOfChildren (Tnode *node, short new_nb_of_children) {
  node->nb_of_children = new_nb_of_children;
}

Tnode *getNodeIthChild (Tnode *node, short i) {
  return node->children[i];
}
void setNodeIthChild (Tnode *node, short i, Tnode *new_child) {
  node->children[i] = new_child;
}

Tnode *createNode (Tboard board, Tbyte nb_of_moves, Tmovement *moves) {
  Tnode *tree = calloc (1, sizeof(Tnode));

  setNodeBoard (tree, board);
  setNodeNbOfMoves (tree, nb_of_moves);

  for (int i = 0; i < nb_of_moves; i++) {
    setNodeIthMove (tree, i,  moves[i]);
  }

  return tree;
}
void freeNode (Tnode *tree) {
  // Frees the node and all its descendants
  for (Tbyte i = 0; i < getNodeNbOfChildren (tree); i++) {
    freeNode (getNodeIthChild (tree, i));
  }

  free (tree);
}
