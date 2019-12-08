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
  copyNextQueue (getBoardNextQueue (dest), getBoardNextQueue (src));
  setBoardHasLostStatus (dest, getBoardHasLostStatus(src));
  setBoardShouldEndTurnStatus (dest, getBoardShouldEndTurnStatus (src));
  setBoardHasHeldThisTurnStatus (dest, getBoardHasHeldThisTurnStatus (src));
}
