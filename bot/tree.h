#ifndef __TREE_H__
#define __TREE_H__

#include "../tetris_engine/tetris_engine.h"
#include "../interface/type_interface.h"
#include <stdlib.h>

// Definition of the search tree used by the bot
#define MAX_MOVEMENTS 15

typedef struct _Tnode {
  // A node contains the state of the board, with the active piece
  // in its final place, as well as the sequence of movements to get
  // it there. It also contains the score for this board given
  // by the evaluation function.
  Tboard board;
  Tbyte nb_of_moves;
  Tmovement moves[MAX_MOVEMENTS];
  float board_value;

  short nb_of_children;
  struct _Tnode** children;
} Tnode;

// Accesors
Tboard *getNodeBoard (Tnode *node);
void setNodeBoard (Tnode *node, Tboard new_board);

Tbyte getNodeNbOfMoves (Tnode *node);
void setNodeNbOfMoves (Tnode *node, Tbyte new_nb_of_moves);

Tmovement getNodeIthMove (Tnode *node, Tbyte i);
void setNodeIthMove (Tnode *node, Tbyte i, Tmovement new_move);

float getNodeBoardValue (Tnode *node);
void setNodeBoardValue (Tnode *node, float new_val);

short getNodeNbOfChildren (Tnode *node);
void setNodeNbOfChildren (Tnode *node, short new_nb_of_children);

Tnode *getNodeIthChild (Tnode *node, short i);
void setNodeIthChild (Tnode *node, short i, Tnode *new_child);

// Constructor & destructor
Tnode *createNode (Tboard board, Tbyte nb_of_moves, Tmovement *moves);
void freeNode (Tnode *tree);

#endif /* end of include guard: __TREE_H__ */
