#include "type_board.h"

// Accessors
Tmatrix *getBoardMatrix (Tboard *board) {
  return &(board->matrix);
}
static void setBoardMatrix (Tboard *board, Tmatrix new_matrix) {
  board->matrix = new_matrix;
}

Ttetrimino *getBoardActiveTetrimino (Tboard *board) {
  return &(board->active_tetrimino);
}
static void setBoardActiveTetrimino (Tboard *board, Ttetrimino new_tetrimino) {
  board->active_tetrimino = new_tetrimino;
}

Tnext_queue *getBoardNextQueue (Tboard *board) {
  return &(board->next_queue);
}
static void setBoardNextQueue (Tboard *board, Tnext_queue new_queue) {
  board->next_queue = new_queue;
}

Tshape getBoardHoldPiece (Tboard *board) {
  return board->hold_piece;
}
void setBoardHoldPiece (Tboard *board, Tshape new_hold_piece) {
  board->hold_piece = new_hold_piece;
}

Tbyte getBoardCurrentDAS (Tboard *board) {
  return board->current_DAS;
}
void setBoardCurrentDAS (Tboard *board, Tbyte new_DAS) {
  board->current_DAS = new_DAS;
}

Tbyte getBoardCurrentARR (Tboard *board) {
  return board->current_ARR;
}
void setBoardCurrentARR (Tboard *board, Tbyte new_ARR) {
  board->current_ARR = new_ARR;
}

Tmovement *getBoardPreviousMv (Tboard *board) {
  return &board->previous_mv;
}
void setBoardPreviousMv (Tboard *board, Tmovement new_mv) {
  board->previous_mv = new_mv;
}

Tline_counter getBoardLinesCleared (Tboard *b) {
  return b->lines_cleared;
}
void setBoardLinesCleared (Tboard *b, Tline_counter lines) {
  b->lines_cleared = lines;
}

bool getBoardHasHeldThisTurnStatus (Tboard *b) {
  return b->has_held_this_turn;
}
void setBoardHasHeldThisTurnStatus (Tboard *b, bool new_state) {
  b->has_held_this_turn = new_state;
}

bool getBoardHasLostStatus (Tboard *b) {
  return b->has_lost;
}
void setBoardHasLostStatus (Tboard *b, bool new_state) {
  b->has_lost = new_state;
}

bool getBoardShouldEndTurnStatus (Tboard *b) {
  return b->should_end_turn;
}
void setBoardShouldEndTurnStatus (Tboard *b, bool new_state) {
  b->should_end_turn = new_state;
}

// Constructor
Tboard createBoard () {
  Tboard b;

  // Initialize each field
  setBoardMatrix (&b, createMatrix ());
  setBoardActiveTetrimino (&b, createTetrimino (EMPTY));
  setBoardNextQueue (&b, createNextQueue ());
  setBoardHoldPiece (&b, EMPTY);
  setBoardCurrentDAS (&b, 0);
  setBoardCurrentARR (&b, 0);
  setBoardPreviousMv (&b, createMovementWord ());
  setBoardLinesCleared (&b, 0);
  setBoardHasLostStatus (&b, false);
  setBoardShouldEndTurnStatus (&b, false);
  setBoardHasHeldThisTurnStatus (&b, false);

  return b;
}

// Copy
void copyBoard (Tboard *dest, Tboard *src) {

  // Copy each field of the tetrimino
  copyMatrix (getBoardMatrix (dest), getBoardMatrix (src));
  copyTetrimino (getBoardActiveTetrimino (dest), getBoardActiveTetrimino (src));
  setBoardHoldPiece (dest, getBoardHoldPiece (src));
  setBoardCurrentDAS (dest, getBoardCurrentDAS (src));
  setBoardCurrentARR (dest, getBoardCurrentARR (src));
  setBoardPreviousMv (dest, *getBoardPreviousMv (src));
  setBoardLinesCleared (dest, getBoardLinesCleared (src));
  copyNextQueue (getBoardNextQueue (dest), getBoardNextQueue (src));
  setBoardHasLostStatus (dest, getBoardHasLostStatus(src));
  setBoardShouldEndTurnStatus (dest, getBoardShouldEndTurnStatus (src));
  setBoardHasHeldThisTurnStatus (dest, getBoardHasHeldThisTurnStatus (src));
}
