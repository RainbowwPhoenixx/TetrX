#ifndef __BOT_TYPES_H__
#define __BOT_TYPES_H__
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "bot_parameters.h"

#include "../tetris_engine/tetris_engine.h"

typedef uint16_t Trow;

// --------------------------------------------------------------------------
//                               BOT MOVEMENT
// --------------------------------------------------------------------------

typedef enum {BOT_MV_LEFT   = 0x0001, // 0b00000001
              BOT_MV_RIGHT  = 0x0002, // 0b00000010
              BOT_MV_CW     = 0x0004, // 0b00000100
              BOT_MV_CCW    = 0x0008, // 0b00001000
              BOT_MV_SD     = 0x0010, // 0b00010000
              BOT_MV_SONICD = 0x0020, // 0b00100000
              BOT_MV_HD     = 0x0040, // 0b01000000
              BOT_MV_HOLD   = 0x0080  // 0b10000000
            } Tbot_possible_movement;

typedef uint16_t Tbot_movement;

Tbot_movement createBotMovementWord ();
void addBotMovementToWord (Tbot_movement *mv_word, Tbot_possible_movement mv);
bool isBotMovementInWord (Tbot_movement *mv_word, Tbot_possible_movement mv);
void removeBotMovementFromWord (Tbot_movement *mv_word, Tbot_possible_movement mv);

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
void botApplyInput (Tbot_board *b, Tnext_queue *next_queue, Tbot_movement mv);


// --------------------------------------------------------------------------
//                        PATHFINDING STUFF
// --------------------------------------------------------------------------

typedef struct _TMoveNode {
  Tbot_movement move;
  Ttetrimino tetrimino;
  Tbyte dist; // = time taken to get there
  bool visited;
  struct _TMoveNode* best_parent;
} TMoveNode;

// Queue implementation for 0 and 1, list for else
// Change to heap implementation later for optimization
// at the moment, only 2 distance values are possible, let's take advantage of this
// We're still gonna have a third list just in case tho
typedef struct {
  unsigned int head_0, tail_0;
  unsigned int head_1, tail_1;
  unsigned int size_else;
  TMoveNode* items_0[MOVE_NODE_LIST_MAX_ITEMS];
  TMoveNode* items_1[MOVE_NODE_LIST_MAX_ITEMS];
  TMoveNode* items_else[MOVE_NODE_LIST_MAX_ITEMS];
} TMoveNodeList;

void setMoveNode (TMoveNode* node, Tbot_movement move, Ttetrimino* tetrimino, Tbyte distance, TMoveNode* parent);

void addMoveNodeToList (TMoveNode* mvnode, TMoveNodeList* list);
TMoveNode* popMinMoveNodeFromList (TMoveNodeList *list);

bool isNotObstacle (Tbot_board* b, Ttetrimino* t);
void applyBotMoveToTetrimino (Tmovement mv, Ttetrimino* t, Tbot_board* board);

// --------------------------------------------------------------------------
//                        PATTERN MATCHING STUFF
// --------------------------------------------------------------------------

typedef struct {
  // Contains the size of the pattern
  int h, w;
  // Contains the bitmasks of the rows of the pattern
  Trow filled[6], empty[6];
} Tpattern;

int patternMatch (Tbot_board *b, Tpattern *pattern, Tcoordinate *heights);

#endif /* end of include guard: __BOT_TYPES_H__ */
