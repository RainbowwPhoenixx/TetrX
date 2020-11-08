#include "board_operations.h"
#include "../../interface/type_interface.h"

#define MINIMUM_QUEUE_LENGTH 11
#define MAXIMUM_AUTHORIZED_HEIGHT 20

#define DAS_VALUE 10
#define ARR_VALUE 1
#define SD_ARR_VALUE 0

// Kick for the different pieces [rotation_state][kick_index][x or y]
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

// Stores the front and back corners of the T piece for tspin checks for each rotation
// [rotation][corner id][x or y]
static Tcoordinate_diff tspin_front_corners[4][2][2] = {
  {{ 1,  1},{-1,  1}},
  {{ 1,  1},{ 1, -1}},
  {{ 1, -1},{-1, -1}},
  {{-1,  1},{-1, -1}},
};
static Tcoordinate_diff tspin_back_corners[4][2][2]  = {
  {{ 1, -1},{-1, -1}},
  {{-1,  1},{-1, -1}},
  {{ 1,  1},{-1,  1}},
  {{ 1,  1},{ 1, -1}},
};

extern Tinterface_out IO_out;

bool isBoardStateValid (Tboard *b) {
  // Checks that the position of the active tetrimino is valid in relation to the matrix.
  // Returns true is the board is valid, false if not.
  Ttetrimino *t = getBoardActiveTetrimino (b);

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

void checkLoss (Tboard *b) {
  // Sets the board loss status if the player has lost

  for (int i = 0; i < NUMBER_OF_MINOS; i++) {
    Ttetrimino *tmp_tet = getBoardActiveTetrimino (b);
    Tmino *tmp_mino = getIthMino (tmp_tet, i);
    if (getTetriminoY (tmp_tet) + getMinoYDiff (tmp_mino) > MAXIMUM_AUTHORIZED_HEIGHT
        || !isMinoAtPosEmpty (getBoardMatrix (b), getTetriminoX (tmp_tet) + getMinoXDiff (tmp_mino), getTetriminoY (tmp_tet) + getMinoYDiff (tmp_mino))) {
      setBoardHasLostStatus (b, true);
    }
  }
}
Tline_clear clearLines (Tboard *b) {
  Tmatrix *tmp_matrix = getBoardMatrix (b);
  Tcoordinate lines_to_clear[4];
  Tbyte number_of_lines_to_clear = 0;
  Tline_clear cleared = createLineClear (0, NONE);

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
  if (number_of_lines_to_clear == 0) return cleared;

  // Delete the full lines and pull down the ones above
  for (Tcoordinate line_number = number_of_lines_to_clear-1; line_number >= 0; line_number--) {
    for (Tcoordinate line = lines_to_clear[line_number]; line < C_MATRIX_HEIGHT-1; line++) {
      for (Tbyte i = 0; i < C_MATRIX_WIDTH; i++) {
        setMatrixShapeAtPos (tmp_matrix, i, line, getMatrixShapeAtPos (tmp_matrix, i, line+1));
      }
    }
  }
  
  // To check for a PC, you only need to check the bottomest line
  Tcoordinate x = 0;
  bool pc_check;
  do {
    pc_check = isMinoAtPosEmpty (tmp_matrix, x, 0);
    x++;
  } while(pc_check && x < C_MATRIX_WIDTH);

  // Add the cleared lines to the line counter
  setBoardLinesCleared (b, getBoardLinesCleared (b) + number_of_lines_to_clear);
  IO_out.lineClearAnimationFunc (lines_to_clear, number_of_lines_to_clear);
  
  if (pc_check) {
    cleared.attack_kind = PC;
  } else if (getTetriminoTspinStatus (getBoardActiveTetrimino (b)) == FULL) {
    cleared.attack_kind = TSPIN;
  } else {
    cleared.attack_kind = NORMAL;
  }
  cleared.nb_of_lines = number_of_lines_to_clear;
  
  return cleared;
}
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
    addBagToNextQueue (next_queue, true);
  }
}

// Other
static void checkTspin (Ttetrimino *t, Tboard *b, int kick_index) {
  // Checks if the t was spun and changes the tetrimino state accordingly
  
  if (kick_index == 4) {
    setTetriminoTspinStatus (t, FULL);
  } else {
    Tmatrix *tmp_matrix = getBoardMatrix (b);
    Tcoordinate tet_x = getTetriminoX (t);
    Tcoordinate tet_y = getTetriminoY (t);
    // Count the front corners
    Tbyte front_corners = 0;
    front_corners += !isMinoAtPosEmpty (tmp_matrix, tet_x + tspin_front_corners[getTetriminoRotationState (t)][0][0], tet_y + tspin_front_corners[getTetriminoRotationState (t)][0][1]);
    front_corners += !isMinoAtPosEmpty (tmp_matrix, tet_x + tspin_front_corners[getTetriminoRotationState (t)][1][0], tet_y + tspin_front_corners[getTetriminoRotationState (t)][1][1]);
    // Count the back corners
    Tbyte back_corners = 0;
    back_corners += !isMinoAtPosEmpty (tmp_matrix, tet_x + tspin_back_corners[getTetriminoRotationState (t)][0][0], tet_y + tspin_back_corners[getTetriminoRotationState (t)][0][1]);
    back_corners += !isMinoAtPosEmpty (tmp_matrix, tet_x + tspin_back_corners[getTetriminoRotationState (t)][1][0], tet_y + tspin_back_corners[getTetriminoRotationState (t)][1][1]);
    
    if (front_corners == 2 && back_corners > 0) {
      setTetriminoTspinStatus (t, FULL);
    } else if (back_corners == 2 && front_corners > 0) {
      setTetriminoTspinStatus (t, MINI);
    }
  }
}

// Input handlers
static void performMoveLeft (Tboard *b) {
  // Save the current board state
  Tboard tmp_board;
  copyBoard (&tmp_board, b);

  // Attempt to move left
  Ttetrimino *t = getBoardActiveTetrimino (b);
  moveTetriminoLeft (t);
  setTetriminoTspinStatus (t, NOSPIN);

  // If the new state is not valid, revert to previous state
  if (!isBoardStateValid (b)) {
    copyBoard (b, &tmp_board);
  }
}
static void performMoveRight (Tboard *b) {
  // Save the current board state
  Tboard tmp_board;
  copyBoard (&tmp_board, b);

  // Attempt to move right
  Ttetrimino *t = getBoardActiveTetrimino (b);
  moveTetriminoRight (t);
  setTetriminoTspinStatus (t, NOSPIN);

  // If the new state is not valid, revert to previous state
  if (!isBoardStateValid (b)) {
    copyBoard (b, &tmp_board);
  }
}
static void performSoftDrop (Tboard *b) {
  // Save the current board state
  Tboard tmp_board;
  copyBoard (&tmp_board, b);

  // Attempt to move down
  Ttetrimino *t = getBoardActiveTetrimino (b);
  moveTetriminoDown (t);
  setTetriminoTspinStatus (t, NOSPIN);

  // If the new state is not valid, revert to previous state
  if (!isBoardStateValid (b)) {
    copyBoard (b, &tmp_board);
  }
}
static void performHardDrop  (Tboard *b) {
  // Save the current board state
  Tboard tmp_board;
  copyBoard (&tmp_board, b);

  // Move down until tetrimino hots the ground
  do {
    copyBoard (b, &tmp_board);
    moveTetriminoDown (getBoardActiveTetrimino (&tmp_board));
  } while(isBoardStateValid (&tmp_board));

  setBoardShouldEndTurnStatus (b, true);
}
static void performRotateCW (Tboard *b) {
  Ttetrimino *t = getBoardActiveTetrimino (b);

  // If the piece is not an O (the O does not rotate)
  if (getTetriminoShape (t) != O) {
    Tboard tmp_board;
    Tcoordinate_diff (*kicks)[2];
    copyBoard (&tmp_board, b);

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
    } while(!isBoardStateValid (b) && i < 5);

    // If the new state is not valid, revert to previous state
    if (!isBoardStateValid (b)) {
      copyBoard (b, &tmp_board);
      return;
    }
    
    // Check for tspin
    if (getTetriminoShape (t) == T) {
      checkTspin (t, b, i);
    }
  }
}
static void performRotateCCW (Tboard *b) {
  Ttetrimino *t = getBoardActiveTetrimino (b);

  // If the piece is not an O (the O does not rotate)
  if (getTetriminoShape (t) != O) {
    Tboard tmp_board;
    Tcoordinate_diff (*kicks)[2];
    copyBoard (&tmp_board, b);

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
    } while(!isBoardStateValid (b) && i < 5);

    // If the new state is not valid, revert to previous state
    if (!isBoardStateValid (b)) {
      copyBoard (b, &tmp_board);
      return;
    }
    
    // Check for tspin
    if (getTetriminoShape (t) == T) {
      checkTspin (t, b, i);
    }
  }
}
static void performHold (Tboard *b) {
  // If player has used hold, don't hold again
  if (getBoardHasHeldThisTurnStatus (b)) {
    return;
  }

  Ttetrimino *t = getBoardActiveTetrimino (b);

  // Swap the piece in the hold box with the active piece
  Tshape tmp_shape = getBoardHoldPiece (b);
  setBoardHoldPiece (b, getTetriminoShape (t));

  // If the active piece is now empty, it means that the hold box was empty so you should get another tetrimino from queue
  // otherwise turn the piece shape that was in the hold into a tetrimino
  if (tmp_shape == EMPTY) {
    popTetriminoFromQueue (b);
  } else {
    *t = createTetrimino (tmp_shape);
  }

  // Signal that the hold has been used this turn
  setBoardHasHeldThisTurnStatus (b, true);
}

// General input handler
void applyInput (Tboard *b, Tmovement mv) {
  // For each movement in the input, apply it to the board
  // Processing order : rotation, translation, SD,  hold, HD.

  // Rotation processing
  if (isMovementInWord (&mv, MV_CCW) && isMovementInWord (&mv, MV_CW)) {
    // If both rotation buttons are pressed, don't do anything
  } else if (isMovementInWord (&mv, MV_CW) && !isMovementInWord (getBoardPreviousMv (b), MV_CW)) {
    performRotateCW (b);
  } else if (isMovementInWord (&mv, MV_CCW) && !isMovementInWord (getBoardPreviousMv (b), MV_CCW)) {
    performRotateCCW (b);
  }

  // Translation processing
  if (isMovementInWord (&mv, MV_LEFT) && isMovementInWord (&mv, MV_RIGHT)) {
    // If both rotation buttons are pressed, reset DAS
    setBoardCurrentARR (b, 0);
  } else if (isMovementInWord (&mv, MV_LEFT)) {
    if (!isMovementInWord (getBoardPreviousMv (b), MV_LEFT)) {
      performMoveLeft (b);
    } else if (getBoardCurrentDAS (b) < DAS_VALUE) {
      setBoardCurrentDAS (b, getBoardCurrentDAS (b)+1);
    } else if (getBoardCurrentARR (b) < ARR_VALUE) {
      setBoardCurrentARR (b, getBoardCurrentARR (b)+1);
    } else {
      setBoardCurrentARR (b, 0);
      performMoveLeft (b);
    }

  } else if (isMovementInWord (&mv, MV_RIGHT)) {
    if (!isMovementInWord (getBoardPreviousMv (b), MV_RIGHT)) {
      performMoveRight (b);
    } else if (getBoardCurrentDAS (b) < DAS_VALUE) {
      setBoardCurrentDAS (b, getBoardCurrentDAS (b)+1);
    } else if (getBoardCurrentARR (b) < ARR_VALUE) {
      setBoardCurrentARR (b, getBoardCurrentARR (b)+1);
    } else {
      setBoardCurrentARR (b, 0);
      performMoveRight (b);
    }

  } else {
    setBoardCurrentDAS (b, 0);
  }

  // SD processing
  if (isMovementInWord (&mv, MV_SD)) {
    if (getBoardCurrentSoftDropARR (b) < SD_ARR_VALUE) {
      setBoardCurrentSoftDropARR (b, getBoardCurrentSoftDropARR (b)+1);
    } else {
      setBoardCurrentSoftDropARR (b, 0);
      performSoftDrop (b);
    }
  }

  // Hold processing
  if (isMovementInWord (&mv, MV_HOLD)) {
    performHold (b);
  }

  // Hard drop processing
  if (isMovementInWord (&mv, MV_HD) && !isMovementInWord (getBoardPreviousMv (b), MV_HD)) {
    performHardDrop (b);
  }

  setBoardPreviousMv (b, mv);
}
