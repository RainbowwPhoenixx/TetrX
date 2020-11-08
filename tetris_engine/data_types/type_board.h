#ifndef __TYPE_BOARD_H__
#define __TYPE_BOARD_H__

#include <stdbool.h>
#include "general_library.h"
#include "type_shape.h"
#include "type_mino.h"
#include "type_tetrimino.h"
#include "type_next_queue.h"
#include "type_matrix.h"
#include "type_movement.h"

typedef unsigned int Tline_counter;

// Structure to keep track of the various game stats
typedef struct {
  unsigned int clear_single;
  unsigned int clear_double;
  unsigned int clear_triple;
  unsigned int clear_tetris;
  unsigned int clear_tss;
  unsigned int clear_tsd;
  unsigned int clear_tst;
  unsigned int clear_pc;
  // For APP
  unsigned int nb_of_pieces;
  unsigned int total_attack;
} Tstatistics;

typedef struct {
  Tmatrix matrix;
  Ttetrimino active_tetrimino;
  Tnext_queue next_queue;
  Tshape hold_piece;

  Tbyte current_DAS;
  Tbyte current_ARR;
  Tbyte current_SD_ARR;
  Tmovement previous_mv;

  unsigned int lines_cleared;
  Tstatistics stats;

  bool has_held_this_turn, has_lost, should_end_turn;
} Tboard;

// Accessors
Tmatrix *getBoardMatrix (Tboard *board);
Ttetrimino *getBoardActiveTetrimino (Tboard *board);
Tnext_queue *getBoardNextQueue (Tboard *board);

Tshape getBoardHoldPiece (Tboard *board);
void setBoardHoldPiece (Tboard *board, Tshape new_hold_piece);

Tbyte getBoardCurrentDAS (Tboard *board);
void setBoardCurrentDAS (Tboard *board, Tbyte new_DAS);

Tbyte getBoardCurrentARR (Tboard *board);
void setBoardCurrentARR (Tboard *board, Tbyte new_DAS);

Tbyte getBoardCurrentSoftDropARR (Tboard *board);
void setBoardCurrentSoftDropARR (Tboard *board, Tbyte new_SD_ARR);

Tmovement *getBoardPreviousMv (Tboard *board);
void setBoardPreviousMv (Tboard *board, Tmovement new_mv);

Tline_counter getBoardLinesCleared (Tboard *b);
void setBoardLinesCleared (Tboard *b, Tline_counter lines);

Tstatistics *getBoardStats (Tboard *b);

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
