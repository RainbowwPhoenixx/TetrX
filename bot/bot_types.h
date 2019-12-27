#ifndef __BOT_TYPES_H__
#define __BOT_TYPES_H__

#include "../tetris_engine/tetris_engine.h"

typedef struct {
  Tmatrix matrix;
  Ttetrimino active_tetrimino;
  Tshape hold_piece;
  Tbyte next_queue_offset;
} Tbot_board;

// Accessors
Tmatrix *getBotBoardMatrix (Tbot_board *board);
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
void botClearLines (Tbot_board *b);
void botLockActiveTetrimino (Tbot_board *b);

bool isBotBoardStateValid (Tbot_board *b);
void botApplyInput (Tbot_board *b, Tnext_queue *next_queue, Tmovement mv);

#endif /* end of include guard: __BOT_TYPES_H__ */
