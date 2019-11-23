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

// Constructor
Tboard createBoard () {
  Tboard b;

  setBoardMatrix (&b, createMatrix ());
  setBoardActiveTetrimino (&b, createTetrimino (EMPTY));
  setBoardNextQueue (&b, createNextQueue ());
  setBoardHoldPiece (&b, EMPTY);

  return b;
}
