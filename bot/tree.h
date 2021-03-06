#ifndef __TREE_H__
#define __TREE_H__

#include "../tetris_engine/tetris_engine.h"
#include "../interface/type_interface.h"
#include "bot_types.h"
#include <stdlib.h>
#include "bot_parameters.h"

// --------------------------------------------------------------------------
//                                    NODE
// --------------------------------------------------------------------------

typedef struct _Tnode {
  // A node contains the state of the board, with the active piece
  // in its final place, as well as the sequence of movements to get
  // it there. It also contains the score for this board given
  // by the evaluation function.
  Tbot_board board;
  Tbyte nb_of_moves;
  Tbot_movement moves[MAX_MOVEMENTS];
  Tline_clear lines_cleared;
  float board_value;

  // Children related variables
  unsigned short nb_of_children;
  struct _Tnode* children[MAX_CHILDREN];
  bool are_children_generated;
  float accumulated_board_value;

  // Parent related variables
  unsigned short initial_rank;
  struct _Tnode *immediate_parent;
} Tnode;

// Accessors
Tbot_board *getNodeBotBoard (Tnode *node);
void setNodeBotBoard (Tnode *node, Tbot_board new_board);

Tbyte getNodeNbOfMoves (Tnode *node);
void setNodeNbOfMoves (Tnode *node, Tbyte new_nb_of_moves);

Tbot_movement getNodeIthMove (Tnode *node, Tbyte i);
void setNodeIthMove (Tnode *node, Tbyte i, Tbot_movement new_move);

Tline_clear getNodeLinesCleared (Tnode *node);
void setNodeLinesCleared (Tnode *node, Tline_clear lines);

float getNodeBoardValue (Tnode *node);
void setNodeBoardValue (Tnode *node, float new_val);

unsigned short getNodeNbOfChildren (Tnode *node);

Tnode *getNodeIthChild (Tnode *node, unsigned short i);
void addChildToNode (Tnode *node, Tnode *new_child);
void setNodeIthChild (Tnode *node, unsigned short i, Tnode *new_child);
void computeChildrenInitialRanks (Tnode *node);

bool getNodeAreChildrenGenerated (Tnode *node);
void setNodeAreChildrenGenerated (Tnode *node, bool new_val);

float getNodeAccumulatedBoardValue (Tnode *node);
void setNodeAccumulatedBoardValue (Tnode *node, float new_acc_value);

unsigned int getNodeNbOfAccumulations (Tnode *node);
void increaseNodeNbOfAccumulations (Tnode *node);

unsigned short getNodeInitialRank (Tnode *node);
void setNodeInitialRank (Tnode *node, unsigned short new_val);

Tnode *getNodeImmediateParent (Tnode* node);
void setNodeImmediateParent (Tnode *node, Tnode *parent);

void sortNodeChildren (Tnode* node);

// I define this here so that the next functions for the node are aware of this struct
#define MAX_NODES_ALLOCATED 500000
typedef struct _Tnode_memory_pool {
  uint32_t free_node_count;
  uint32_t total_node_count;
  Tnode* free_nodes[MAX_NODES_ALLOCATED];
} Tnode_memory_pool;

// Constructor & destructor
Tnode *createNode (Tbot_board board, Tbyte nb_of_moves, Tbot_movement *moves, Tnode *parent, Tnode_memory_pool *pool);
void freeNode (Tnode *tree, Tnode_memory_pool *pool);

// --------------------------------------------------------------------------
//                            NODE MEMORY POOL
// --------------------------------------------------------------------------

Tnode_memory_pool createNodeMemoryPool();
bool freeNodeMemoryPool(Tnode_memory_pool pool);

Tnode* getNodeFromPool (Tnode_memory_pool* pool);
void giveNodeToPool (Tnode_memory_pool* pool, Tnode* node);



#endif /* end of include guard: __TREE_H__ */
