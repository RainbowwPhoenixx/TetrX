#ifndef __BOT_TYPES_H__
#define __BOT_TYPES_H__
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "../tetris_engine/tetris_engine.h"

#define BOT_MATRIX_HEIGHT 45
#define BOT_MATRIX_WIDTH 10

typedef enum {L0=0, L1=1, L2=2, L3=3, L4=4, TSS, TSD, TST} Tline_clear;
typedef uint16_t Trow;

// --------------------------------------------------------------------------
//                                MATRIX TYPE
// --------------------------------------------------------------------------

typedef struct {
  Trow rows [BOT_MATRIX_HEIGHT];
} Tbot_matrix;

// Constructor protoype
Tbot_matrix createBotMatrix ();

// Useful functions protoypes
bool getBotMatrixCellFilledState (Tbot_matrix *matrix, Tcoordinate x, Tcoordinate y);
void setBotMatrixCellFilledState (Tbot_matrix *matrix, Tcoordinate x, Tcoordinate y, bool new_state);

bool isBotMatrixRowEmpty (Tbot_matrix *matrix, Tcoordinate row);
bool isBotMatrixRowFull (Tbot_matrix *matrix, Tcoordinate row);

Trow getBotMatrixRow (Tbot_matrix *matrix, Tcoordinate row_num);
void setBotMatrixRow (Tbot_matrix *matrix, Tcoordinate row_num, Trow new_row);

void copyBotMatrix (Tbot_matrix *dest, Tbot_matrix *src);

// --------------------------------------------------------------------------
//                                BOARD TYPE
// --------------------------------------------------------------------------

typedef struct {
  Tbot_matrix matrix;
  Ttetrimino active_tetrimino;
  Tshape hold_piece;
  Tbyte next_queue_offset;
} Tbot_board;

// Accessors
Tbot_matrix *getBotBoardMatrix (Tbot_board *board);
Ttetrimino *getBotBoardActiveTetrimino (Tbot_board *board);

Tbyte getBotBoardNextQueueOffset (Tbot_board *board);
void setBotBoardNextQueueOffset (Tbot_board *board, Tbyte new_queue_offset);

Tshape getBotBoardHoldPiece (Tbot_board *board);
void setBotBoardHoldPiece (Tbot_board *board, Tshape new_hold_piece);

// Constructor
Tbot_board createBotBoard ();

// Copy & convert
void copyBotBoard (Tbot_board *dest, Tbot_board *src);
Tbot_board convertBoardToBotBoard (Tboard *b);

// Operations
void botPopTetriminoFromQueue (Tbot_board *bb, Tnext_queue *next_queue);
Tbyte botClearLines (Tbot_board *b);
void botLockActiveTetrimino (Tbot_board *b);

bool isBotBoardStateValid (Tbot_board *b);
void botApplyInput (Tbot_board *b, Tnext_queue *next_queue, Tmovement mv);


// --------------------------------------------------------------------------
//                        PATHFINDING STUFF
// --------------------------------------------------------------------------

typedef struct _TMoveNode {
  Tmovement move;
  Ttetrimino tetrimino;
  float dist; // = time taken to get there
  struct _TMoveNode* best_parent;
} TMoveNode;

typedef struct { // Change to heap implementation later for optimization
  unsigned int size;
  TMoveNode* items[1000];
} TMoveNodeList;

TMoveNode* createMoveNode (Tmovement move, Ttetrimino* tetrimino, float distance, TMoveNode* parent);
void destroyMoveNode (TMoveNode *node);

void addMoveNodeToList (TMoveNode* mvnode, TMoveNodeList* list);
TMoveNode* popMinMoveNodeFromList (TMoveNodeList *list);

bool isNotObstacle (Tbot_board* b, Ttetrimino* t);

#endif /* end of include guard: __BOT_TYPES_H__ */
