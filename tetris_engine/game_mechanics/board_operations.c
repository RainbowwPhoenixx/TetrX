#include "board_operations.h"

#define MINIMUM_QUEUE_LENGTH 8

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

bool isBoardStateValid (Tboard *b) {
  // Checks that the position of the active tetrimino is valid in relation to the matrix.
  Ttetrimino *t = getBoardActiveTetrimino (b);

  int i = 0;
  bool res = true;
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
      Tmatrix *tmp_m = getBoardMatrix(b);
      if (!isMinoAtPosEmpty (tmp_m, tmp_x, tmp_y)) {
        res = false;
      }
    }

    i++;
  } while ((i < NUMBER_OF_MINOS) && res);

  return res;
}

// Game mechanics implementation

void lockActiveTetrimino (Tboard *b) {
  Ttetrimino *t = getBoardActiveTetrimino (b);
  Tmatrix *m = getBoardMatrix (b);

  for (Tbyte i = 0; i < NUMBER_OF_MINOS; i++) {
    Tmino *tmp_mino = getIthMino (t, i);
    setMatrixShapeAtPos (m, getTetriminoX (t) + getMinoXDiff (tmp_mino), getTetriminoY (t) + getMinoYDiff (tmp_mino), getTetriminoShape (t));
  }
}
void popTetriminoFromQueue (Tboard *b) {
  Ttetrimino *t = getBoardActiveTetrimino (b);
  Tnext_queue *next_queue = getBoardNextQueue (b);
  *t = createTetrimino (getIthNextPiece (next_queue, 0));

  advanceNextQueue (next_queue);

  if (getNextQueueLength (next_queue) < MINIMUM_QUEUE_LENGTH) {
    addBagToNextQueue (next_queue);
  }
}

static void performMoveLeft (Tboard *b) {
  Tboard tmp_board;
  copyBoard (&tmp_board, b);

  moveTetriminoLeft (getBoardActiveTetrimino (b));

  // If the new state is not valid, revert to previous state
  if (!isBoardStateValid (b)) {
    copyBoard (b, &tmp_board);
  }
}
static void performMoveRight (Tboard *b) {
  Tboard tmp_board;
  copyBoard (&tmp_board, b);

  moveTetriminoRight (getBoardActiveTetrimino (b));

  // If the new state is not valid, revert to previous state
  if (!isBoardStateValid (b)) {
    copyBoard (b, &tmp_board);
  }
}
static void performSoftDrop (Tboard *b) {
  Tboard tmp_board;
  copyBoard (&tmp_board, b);

  moveTetriminoDown (getBoardActiveTetrimino (b));

  // If the new state is not valid, revert to previous state
  if (!isBoardStateValid (b)) {
    copyBoard (b, &tmp_board);
  }
}
static void performHardDrop  (Tboard *b) {
  Tboard tmp_board;
  copyBoard (&tmp_board, b);

  do {
    copyBoard (b, &tmp_board);
    moveTetriminoDown (getBoardActiveTetrimino (&tmp_board));
  } while(isBoardStateValid (&tmp_board));

  setBoardShouldEndTurnStatus (b, true);
}
static void performRotateCW (Tboard *b) {
  Ttetrimino *t = getBoardActiveTetrimino (b);

  if (getTetriminoShape (t) != O) { // The O doesn't rotate
    Tboard tmp_board;
    Tcoordinate_diff (*kicks)[2];
    copyBoard (&tmp_board, b);

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
    } while(!isBoardStateValid (b) && i < 5);

    // If the new state is not valid, revert to previous state
    if (!isBoardStateValid (b)) {
      copyBoard (b, &tmp_board);
    }
  }
}
static void performRotateCCW (Tboard *b) {
  Ttetrimino *t = getBoardActiveTetrimino (b);

  if (getTetriminoShape (t) != O) { // The O doesn't rotate
    Tboard tmp_board;
    Tcoordinate_diff (*kicks)[2];
    copyBoard (&tmp_board, b);

    // Rotate the piece
    moveTetriminoCCW (t);
    Ttetrimino piece_no_kicks = *t;

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
    } while(!isBoardStateValid (b) && i < 5);

    // If the new state is not valid, revert to previous state
    if (!isBoardStateValid (b)) {
      copyBoard (b, &tmp_board);
    }
  }
}
static void performHold (Tboard *b) {
  if (getBoardHasHeldThisTurnStatus (b)) {
    return;
  }

  Ttetrimino *t = getBoardActiveTetrimino (b);
  Tshape tmp_shape = getBoardHoldPiece (b);

  setBoardHoldPiece (b, getTetriminoShape (t));

  if (tmp_shape == EMPTY) {
    popTetriminoFromQueue (b);
  } else {
    *t = createTetrimino (tmp_shape);
  }

  setBoardHasHeldThisTurnStatus (b, true);
}

void applyInput (Tboard *b, Tmovement mv) {
  if (isMovementInWord (&mv, MV_LEFT )) performMoveLeft (b);
  if (isMovementInWord (&mv, MV_RIGHT)) performMoveRight (b);
  if (isMovementInWord (&mv, MV_CW   )) performRotateCW (b);
  if (isMovementInWord (&mv, MV_CCW  )) performRotateCCW (b);
  if (isMovementInWord (&mv, MV_SD   )) performSoftDrop (b);
  if (isMovementInWord (&mv, MV_HD   )) performHardDrop (b);
  if (isMovementInWord (&mv, MV_HOLD )) performHold (b);
}
