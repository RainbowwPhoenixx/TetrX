#ifndef __TREE_H__
#define __TREE_H__

#include "../tetris_engine/tetris_engine.h"
#include "../interface/type_interface.h"
#include "bot_types.h"
#include <stdlib.h>

// Definition of the search tree used by the bot
#define MAX_MOVEMENTS 15

typedef struct _Tnode {
  // A node contains the state of the board, with the active piece
  // in its final place, as well as the sequence of movements to get
  // it there. It also contains the score for this board given
  // by the evaluation function.
  Tbot_board board;
  Tbyte nb_of_moves;
  Tmovement moves[MAX_MOVEMENTS];
  float board_value;

  unsigned short nb_of_children;
  struct _Tnode* children[100];

  bool are_children_generated;
  unsigned short child_id;
  // Pointer to parent, bool is_root
} Tnode;

// Accessors
Tbot_board *getNodeBotBoard (Tnode *node);
void setNodeBotBoard (Tnode *node, Tbot_board new_board);

Tbyte getNodeNbOfMoves (Tnode *node);
void setNodeNbOfMoves (Tnode *node, Tbyte new_nb_of_moves);

Tmovement getNodeIthMove (Tnode *node, Tbyte i);
void setNodeIthMove (Tnode *node, Tbyte i, Tmovement new_move);

float getNodeBoardValue (Tnode *node);
void setNodeBoardValue (Tnode *node, float new_val);

unsigned short getNodeNbOfChildren (Tnode *node);
void setNodeNbOfChildren (Tnode *node, unsigned short new_nb_of_children);

Tnode *getNodeIthChild (Tnode *node, unsigned short i);
void setNodeIthChild (Tnode *node, unsigned short i, Tnode *new_child);

bool getNodeAreChildrenGenerated (Tnode *node);
void setNodeAreChildrenGenerated (Tnode *node, bool new_val);

unsigned short getNodeChildID (Tnode *node);
void setNodeChildID (Tnode *node, unsigned short new_val);

// Constructor & destructor
Tnode *createNode (Tbot_board board, Tbyte nb_of_moves, Tmovement *moves);
void freeNode (Tnode *tree);

#endif /* end of include guard: __TREE_H__ */
