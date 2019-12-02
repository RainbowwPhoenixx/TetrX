#include "board_operations.h"

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

}
static void performRotateCW (Tboard *b) {
  Tboard tmp_board;
  copyBoard (&tmp_board, b);

  moveTetriminoCW (getBoardActiveTetrimino (b));

  // If the new state is not valid, revert to previous state
  if (!isBoardStateValid (b)) {
    copyBoard (b, &tmp_board);
  }
}
static void performRotateCCW (Tboard *b) {
  Tboard tmp_board;
  copyBoard (&tmp_board, b);

  moveTetriminoCCW (getBoardActiveTetrimino (b));

  // If the new state is not valid, revert to previous state
  if (!isBoardStateValid (b)) {
    copyBoard (b, &tmp_board);
  }
}
static void performHold (Tboard *b) {

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
