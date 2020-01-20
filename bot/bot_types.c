#include "bot_types.h"

// Accessors
Tmatrix *getBotBoardMatrix (Tbot_board *board) {
  return &(board->matrix);
}
static void setBotBoardMatrix (Tbot_board *board, Tmatrix new_matrix) {
  board->matrix = new_matrix;
}

Ttetrimino *getBotBoardActiveTetrimino (Tbot_board *board) {
  return &(board->active_tetrimino);
}
static void setBotBoardActiveTetrimino (Tbot_board *board, Ttetrimino new_tetrimino) {
  board->active_tetrimino = new_tetrimino;
}

Tbyte getBotBoardNextQueueOffset (Tbot_board *board) {
  return board->next_queue_offset;
}
void setBotBoardNextQueueOffset (Tbot_board *board, Tbyte new_queue_offset) {
  board->next_queue_offset = new_queue_offset;
}

Tshape getBotBoardHoldPiece (Tbot_board *board) {
  return board->hold_piece;
}
void setBotBoardHoldPiece (Tbot_board *board, Tshape new_hold_piece) {
  board->hold_piece = new_hold_piece;
}

// Constructor
Tbot_board createBotBoard () {
  Tbot_board b;

  // Initialize each field
  setBotBoardMatrix (&b, createMatrix ());
  setBotBoardActiveTetrimino (&b, createTetrimino (EMPTY));
  setBotBoardNextQueueOffset (&b, 0);
  setBotBoardHoldPiece (&b, EMPTY);

  return b;
}

// Copy & convert
void copyBotBoard (Tbot_board *dest, Tbot_board *src) {

  // Copy each field of the tetrimino
  copyMatrix (getBotBoardMatrix (dest), getBotBoardMatrix (src));
  copyTetrimino (getBotBoardActiveTetrimino (dest), getBotBoardActiveTetrimino (src));
  setBotBoardHoldPiece (dest, getBotBoardHoldPiece (src));
  setBotBoardNextQueueOffset (dest, getBotBoardNextQueueOffset (src));
}
Tbot_board convertBoardToBotBoard (Tboard *b) {
  Tbot_board bb;
  copyMatrix (getBotBoardMatrix (&bb), getBoardMatrix (b));
  copyTetrimino (getBotBoardActiveTetrimino (&bb), getBoardActiveTetrimino (b));
  setBotBoardHoldPiece (&bb, getBoardHoldPiece (b));
  setBotBoardNextQueueOffset (&bb, 0);
  return bb;
}

// Operations
void botPopTetriminoFromQueue (Tbot_board *bb, Tnext_queue *next_queue) {
  Ttetrimino *t = getBotBoardActiveTetrimino (bb);
  *t = createTetrimino (getIthNextPiece (next_queue, getBotBoardNextQueueOffset (bb)));

  setBotBoardNextQueueOffset (bb, getBotBoardNextQueueOffset (bb)+1);
}

// Board functions
static Tcoordinate_diff not_I_kicks[4][5][2] = {
  {{ 0,  0}, { -1, 0}, {-1,  1}, { 0, -2}, {-1, -2}},
  {{ 0,  0}, { 1,  0}, { 1, -1}, { 0,  2}, { 1,  2}},
  {{ 0,  0}, { 1,  0}, { 1,  1}, { 0, -2}, { 1, -2}},
  {{ 0,  0}, {-1,  0}, {-1, -1}, { 0,  2}, {-1,  2}}
};
static Tcoordinate_diff I_kicks[4][5][2] = {
  {{ 0,  0}, {-2,  0}, { 1,  0}, {-2, -1}, { 1,  2}},
  {{ 0,  0}, {-1,  0}, { 2,  0}, {-1,  2}, { 2, -1}},
  {{ 0,  0}, { 2,  0}, {-1,  0}, { 2,  1}, {-1, -2}},
  {{ 0,  0}, { 1,  0}, {-2,  0}, { 1, -2}, {-2,  1}}
};

bool isBotBoardStateValid (Tbot_board *b) {
  // Checks that the position of the active tetrimino is valid in relation to the matrix.
  // Returns true is the board is valid, false if not.
  Ttetrimino *t = getBotBoardActiveTetrimino (b);

  int i = 0;
  bool res = true;
  // For each mino
  do {
    Tmino *tmp_mino = getIthMino (t, i);
    Tcoordinate tmp_x = getTetriminoX (t) + getMinoXDiff (tmp_mino);
    Tcoordinate tmp_y = getTetriminoY (t) + getMinoYDiff (tmp_mino);

    // Check that the mino is inside of the matrix
    if (   tmp_x >= C_MATRIX_WIDTH
        || tmp_x < 0
        || tmp_y < 0) {

      res = false;
    }

    // Check that the mino does not intersect with minos on the board
    if (res) {
      Tmatrix *tmp_m = getBotBoardMatrix(b);
      if (!isMinoAtPosEmpty (tmp_m, tmp_x, tmp_y)) {
        res = false;
      }
    }

    i++;
  } while ((i < NUMBER_OF_MINOS) && res);

  return res;
}
Tbyte botClearLines (Tbot_board *b) {
  // Clears lines and returns the number of lines cleared.
  Tmatrix *tmp_matrix = getBotBoardMatrix (b);
  Tcoordinate lines_to_clear[4];
  Tbyte number_of_lines_to_clear = 0;

  // Find full lines
  for (Tcoordinate line = 0; line < C_MATRIX_HEIGHT; line++) {
    bool is_line_full = true;
    for (Tcoordinate column = 0; column < C_MATRIX_WIDTH; column++) {
      if (isMinoAtPosEmpty (tmp_matrix, column, line)) {
        is_line_full = false;
      }
    }

    if (is_line_full) {
      lines_to_clear[number_of_lines_to_clear] = line;
      number_of_lines_to_clear++;
    }
  }

  // If no lines to clear, stop here
  if (number_of_lines_to_clear == 0) return 0;

  // Delete the full lines and pull down the ones above
  for (Tcoordinate line_number = number_of_lines_to_clear-1; line_number >= 0; line_number--) {
    for (Tcoordinate line = lines_to_clear[line_number]; line < C_MATRIX_HEIGHT-1; line++) {
      for (Tbyte i = 0; i < C_MATRIX_WIDTH; i++) {
        setMatrixShapeAtPos (tmp_matrix, i, line, getMatrixShapeAtPos (tmp_matrix, i, line+1));
      }
    }
  }

  return number_of_lines_to_clear;
}
void botLockActiveTetrimino (Tbot_board *b) {
  Ttetrimino *t = getBotBoardActiveTetrimino (b);
  Tmatrix *m = getBotBoardMatrix (b);

  for (Tbyte i = 0; i < NUMBER_OF_MINOS; i++) {
    Tmino *tmp_mino = getIthMino (t, i);
    setMatrixShapeAtPos (m, getTetriminoX (t) + getMinoXDiff (tmp_mino), getTetriminoY (t) + getMinoYDiff (tmp_mino), getTetriminoShape (t));
  }
}

// Input handlers
static void botPerformMoveLeft (Tbot_board *b) {
  // Save the current board state
  Tbot_board tmp_board;
  copyBotBoard (&tmp_board, b);

  // Attempt to move left
  moveTetriminoLeft (getBotBoardActiveTetrimino (b));

  // If the new state is not valid, revert to previous state
  if (!isBotBoardStateValid (b)) {
    copyBotBoard (b, &tmp_board);
  }
}
static void botPerformMoveRight (Tbot_board *b) {
  // Save the current board state
  Tbot_board tmp_board;
  copyBotBoard (&tmp_board, b);

  // Attempt to move right
  moveTetriminoRight (getBotBoardActiveTetrimino (b));

  // If the new state is not valid, revert to previous state
  if (!isBotBoardStateValid (b)) {
    copyBotBoard (b, &tmp_board);
  }
}
static void botPerformSoftDrop (Tbot_board *b) {
  // Save the current board state
  Tbot_board tmp_board;
  copyBotBoard (&tmp_board, b);

  // Attempt to move down
  moveTetriminoDown (getBotBoardActiveTetrimino (b));

  // If the new state is not valid, revert to previous state
  if (!isBotBoardStateValid (b)) {
    copyBotBoard (b, &tmp_board);
  }
}
static void botPerformHardDrop  (Tbot_board *b) {
  // Save the current board state
  Tbot_board tmp_board;
  copyBotBoard (&tmp_board, b);

  // Move down until tetrimino hots the ground
  do {
    copyBotBoard (b, &tmp_board);
    moveTetriminoDown (getBotBoardActiveTetrimino (&tmp_board));
  } while(isBotBoardStateValid (&tmp_board));
}
static void botPerformRotateCW (Tbot_board *b) {
  Ttetrimino *t = getBotBoardActiveTetrimino (b);

  // If the piece is not an O (the O does not rotate)
  if (getTetriminoShape (t) != O) {
    Tbot_board tmp_board;
    Tcoordinate_diff (*kicks)[2];
    copyBotBoard (&tmp_board, b);

    // Get the kicks for the avtive piece and its rotation state
    if (getTetriminoShape (t) == I) {
      kicks = I_kicks[getTetriminoRotationState (t)];
    } else {
      kicks = not_I_kicks[getTetriminoRotationState (t)];
    }

    // Rotate the piece
    moveTetriminoCW (t);
    Ttetrimino piece_no_kicks = *t;

    // Attempt to kick the piece
    int i = 0;
    do {
      *t = piece_no_kicks;
      setTetriminoX (t, getTetriminoX (t) + kicks[i][0]);
      setTetriminoY (t, getTetriminoY (t) + kicks[i][1]);

      i++;
    } while(!isBotBoardStateValid (b) && i < 5);

    // If the new state is not valid, revert to previous state
    if (!isBotBoardStateValid (b)) {
      copyBotBoard (b, &tmp_board);
    }
  }
}
static void botPerformRotateCCW (Tbot_board *b) {
  Ttetrimino *t = getBotBoardActiveTetrimino (b);

  // If the piece is not an O (the O does not rotate)
  if (getTetriminoShape (t) != O) {
    Tbot_board tmp_board;
    Tcoordinate_diff (*kicks)[2];
    copyBotBoard (&tmp_board, b);

    // Rotate the piece
    moveTetriminoCCW (t);
    Ttetrimino piece_no_kicks = *t;

    // Get the kicks for the avtive piece and its rotation state
    if (getTetriminoShape (t) == I) {
      kicks = I_kicks[getTetriminoRotationState (t)];
    } else {
      kicks = not_I_kicks[getTetriminoRotationState (t)];
    }

    // Attempt to kick the piece
    int i = 0;
    do {
      *t = piece_no_kicks;
      setTetriminoX (t, getTetriminoX (t) - kicks[i][0]);
      setTetriminoY (t, getTetriminoY (t) - kicks[i][1]);

      i++;
    } while(!isBotBoardStateValid (b) && i < 5);

    // If the new state is not valid, revert to previous state
    if (!isBotBoardStateValid (b)) {
      copyBotBoard (b, &tmp_board);
    }
  }
}
static void botPerformHold (Tbot_board *b, Tnext_queue *next_queue) {
  // If player has used hold, don't hold again

  Ttetrimino *t = getBotBoardActiveTetrimino (b);

  // Swap the piece in the hold box with the active piece
  Tshape tmp_shape = getBotBoardHoldPiece (b);
  setBotBoardHoldPiece (b, getTetriminoShape (t));

  // If the active piece is now empty, it means that the hold box was empty so you should get another tetrimino from queue
  // otherwise turn the piece shape that was in the hold into a tetrimino
  if (tmp_shape == EMPTY) {
    botPopTetriminoFromQueue (b, next_queue);
  } else {
    *t = createTetrimino (tmp_shape);
  }
}

// General input handler
void botApplyInput (Tbot_board *b, Tnext_queue *next_queue, Tmovement mv) {
  // For each movement in the input, apply it to the board
  if (isMovementInWord (&mv, MV_LEFT )) botPerformMoveLeft (b);
  if (isMovementInWord (&mv, MV_RIGHT)) botPerformMoveRight (b);
  if (isMovementInWord (&mv, MV_CW   )) botPerformRotateCW (b);
  if (isMovementInWord (&mv, MV_CCW  )) botPerformRotateCCW (b);
  if (isMovementInWord (&mv, MV_SD   )) botPerformSoftDrop (b);
  if (isMovementInWord (&mv, MV_HD   )) botPerformHardDrop (b);
  if (isMovementInWord (&mv, MV_HOLD )) botPerformHold (b, next_queue);
}
