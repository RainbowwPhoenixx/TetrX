#ifndef __TYPE_BOARD_H__
#define __TYPE_BOARD_H__

#include <stdbool.h>
#include "general_library.h"
#include "type_shape.h"
#include "type_mino.h"
#include "type_tetrimino.h"
#include "type_next_queue.h"
#include "type_matrix.h"

typedef struct {
  Tmatrix matrix;
  Ttetrimino active_tetrimino;
  Tnext_queue next_queue;
  Tshape hold_piece;

  bool has_held_this_turn, has_lost, should_end_turn;
} Tboard;

// Accessors
Tmatrix *getBoardMatrix (Tboard *board);
Ttetrimino *getBoardActiveTetrimino (Tboard *board);
Tnext_queue *getBoardNextQueue (Tboard *board);

Tshape getBoardHoldPiece (Tboard *board);
void setBoardHoldPiece (Tboard *board, Tshape new_hold_piece);

bool getBoardHasHeldThisTurnStatus (Tboard *b);
void setBoardHasHeldThisTurnStatus (Tboard *b, bool new_state);

bool getBoardHasLostStatus (Tboard *b);
void setBoardHasLostStatus (Tboard *b, bool new_state);

bool getBoardShouldEndTurnStatus (Tboard *b);
void setBoardShouldEndTurnStatus (Tboard *b, bool new_state);

// Constructor
Tboard createBoard ();

// Copy
void copyBoard (Tboard *dest, Tboard *src);

#endif
