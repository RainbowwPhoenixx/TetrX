#include "bot_types.h"


// --------------------------------------------------------------------------
//                               BOT MOVEMENT
// --------------------------------------------------------------------------

Tbot_movement createBotMovementWord () {
  return 0;
}

void addBotMovementToWord (Tbot_movement *mv_word, Tbot_possible_movement mv) {
  (*mv_word) = (*mv_word) | mv;
}
bool isBotMovementInWord (Tbot_movement *mv_word, Tbot_possible_movement mv) {
  return (*mv_word & mv);
}
void removeBotMovementFromWord (Tbot_movement *mv_word, Tbot_possible_movement mv) {
  if (isBotMovementInWord (mv_word, mv)) {
    (*mv_word) = (*mv_word) ^ mv;
  }
}

// --------------------------------------------------------------------------
//                                MATRIX TYPE
// --------------------------------------------------------------------------

// Constructor
Tbot_matrix createBotMatrix () {
  Tbot_matrix res;
  for (Tcoordinate i = 0; i < BOT_MATRIX_HEIGHT; i++) {
    res.rows[i] = 0;
  }
  return res;
}

// Useful functions
bool getBotMatrixCellFilledState (Tbot_matrix *matrix, Tcoordinate x, Tcoordinate y) {
  if (x < 0 || x >= BOT_MATRIX_WIDTH) {
    return 1;
  } else
  return (matrix->rows[y] & (1 << x));
}
void setBotMatrixCellFilledState (Tbot_matrix *matrix, Tcoordinate x, Tcoordinate y, bool new_state) {
  matrix->rows[y] = (matrix->rows[y] & ~(1 << x)) | (new_state << x);
}

bool isBotMatrixRowEmpty (Tbot_matrix *matrix, Tcoordinate row) {
  return (matrix->rows[row] == 0);
}
bool isBotMatrixRowFull (Tbot_matrix *matrix, Tcoordinate row) {
  return (matrix->rows[row] >= 0x03FF);
}

Trow getBotMatrixRow (Tbot_matrix *matrix, Tcoordinate row_num) {
  return matrix->rows[row_num];
}
void setBotMatrixRow (Tbot_matrix *matrix, Tcoordinate row_num, Trow new_row) {
  matrix->rows[row_num] = new_row;
}

void copyBotMatrix (Tbot_matrix *dest, Tbot_matrix *src) {
  for (Tcoordinate i = 0; i < BOT_MATRIX_HEIGHT; i++) {
    dest->rows[i] = src->rows[i];
  }
}

// --------------------------------------------------------------------------
//                                BOARD TYPE
// --------------------------------------------------------------------------

// Accessors
Tbot_matrix *getBotBoardMatrix (Tbot_board *board) {
  return &(board->matrix);
}
static void setBotBoardMatrix (Tbot_board *board, Tbot_matrix new_matrix) {
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
  setBotBoardMatrix (&b, createBotMatrix ());
  setBotBoardActiveTetrimino (&b, createTetrimino (EMPTY));
  setBotBoardNextQueueOffset (&b, 0);
  setBotBoardHoldPiece (&b, EMPTY);

  return b;
}

// Copy & convert
void copyBotBoard (Tbot_board *dest, Tbot_board *src) {

  // Copy each field of the tetrimino
  copyBotMatrix (getBotBoardMatrix (dest), getBotBoardMatrix (src));
  copyTetrimino (getBotBoardActiveTetrimino (dest), getBotBoardActiveTetrimino (src));
  setBotBoardHoldPiece (dest, getBotBoardHoldPiece (src));
  setBotBoardNextQueueOffset (dest, getBotBoardNextQueueOffset (src));
}
Tbot_board convertBoardToBotBoard (Tboard *b) {
  Tbot_board bb = createBotBoard();
  Tbot_matrix* tmp_botmat = getBotBoardMatrix (&bb);
  Tmatrix* tmp_mat = getBoardMatrix (b);
  // Convert matrix
  for (Tcoordinate y = 0; y < BOT_MATRIX_HEIGHT; y++) {
    for (Tcoordinate x = 0; x < BOT_MATRIX_WIDTH; x++) {
      setBotMatrixCellFilledState (tmp_botmat, x, y, !isMinoAtPosEmpty (tmp_mat, x, y));
    }
  }
  copyTetrimino (getBotBoardActiveTetrimino (&bb), getBoardActiveTetrimino (b));
  setBotBoardHoldPiece (&bb, getBoardHoldPiece (b));
  setBotBoardNextQueueOffset (&bb, 0);
  return bb;
}

// Operations
void botPopTetriminoFromQueue (Tbot_board *bb, Tnext_queue *next_queue) {
  Ttetrimino *t = getBotBoardActiveTetrimino (bb); // Useless line ???
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
      Tbot_matrix *tmp_m = getBotBoardMatrix(b);
      if (getBotMatrixCellFilledState (tmp_m, tmp_x, tmp_y)) {
        res = false;
      }
    }

    i++;
  } while ((i < NUMBER_OF_MINOS) && res);

  return res;
}
Tbyte botClearLines (Tbot_board *b) {
  // Clears lines and returns the number of lines cleared.
  Tbot_matrix *tmp_matrix = getBotBoardMatrix (b);
  Tcoordinate lines_to_clear[4];
  Tbyte number_of_lines_to_clear = 0;

  // Find full lines
  for (Tcoordinate line = 0; line < C_MATRIX_HEIGHT; line++) {
    if (isBotMatrixRowFull (tmp_matrix, line)) {
      lines_to_clear[number_of_lines_to_clear] = line;
      number_of_lines_to_clear++;
    }
  }

  // If no lines to clear, stop here
  if (number_of_lines_to_clear == 0) return 0;

  // Delete the full lines and pull down the ones above
  for (Tcoordinate line_number = number_of_lines_to_clear-1; line_number >= 0; line_number--) {
    for (Tcoordinate line = lines_to_clear[line_number]; line < C_MATRIX_HEIGHT-1; line++) {
      setBotMatrixRow (tmp_matrix, line, getBotMatrixRow (tmp_matrix, line+1));
    }
  }

  return number_of_lines_to_clear;
}
void botLockActiveTetrimino (Tbot_board *b) {
  Ttetrimino *t = getBotBoardActiveTetrimino (b);
  Tbot_matrix *m = getBotBoardMatrix (b);

  for (Tbyte i = 0; i < NUMBER_OF_MINOS; i++) {
    Tmino *tmp_mino = getIthMino (t, i);
    setBotMatrixCellFilledState (m, getTetriminoX (t) + getMinoXDiff (tmp_mino), getTetriminoY (t) + getMinoYDiff (tmp_mino), true);
  }
}

// Input handlers
static void botPerformMoveLeft (Tbot_board *b) {
  // Attempt to move left
  moveTetriminoLeft (getBotBoardActiveTetrimino (b));

  // If the new state is not valid, revert to previous state
  if (!isBotBoardStateValid (b)) {
    moveTetriminoRight (getBotBoardActiveTetrimino (b));
  }
}
static void botPerformMoveRight (Tbot_board *b) {
  // Attempt to move left
  moveTetriminoRight (getBotBoardActiveTetrimino (b));

  // If the new state is not valid, revert to previous state
  if (!isBotBoardStateValid (b)) {
    moveTetriminoLeft (getBotBoardActiveTetrimino (b));
  }
}
static void botPerformSoftDrop (Tbot_board *b) {
  // Attempt to move left
  moveTetriminoDown (getBotBoardActiveTetrimino (b));

  // If the new state is not valid, revert to previous state
  if (!isBotBoardStateValid (b)) {
    moveTetriminoUp (getBotBoardActiveTetrimino (b));
  }
}
static void botPerformHardDrop  (Tbot_board *b) {
  // Move down until tetrimino hits the ground
  do {
    moveTetriminoDown (getBotBoardActiveTetrimino (b));
  } while(isBotBoardStateValid (b));
  
  moveTetriminoUp (getBotBoardActiveTetrimino (b));
}
static void botPerformRotateCW (Tbot_board *b) {
  Ttetrimino *t = getBotBoardActiveTetrimino (b);
  
  // If the piece is not an O (the O does not rotate)
  if (getTetriminoShape (t) == O) {
    return;
  }

  Tcoordinate_diff (*kicks)[2];

  // Get the kicks for the avtive piece and its rotation state
  if (getTetriminoShape (t) == I) {
    kicks = I_kicks[getTetriminoRotationState (t)];
  } else {
    kicks = not_I_kicks[getTetriminoRotationState (t)];
  }

  // Rotate the piece
  moveTetriminoCW (t);
  Tcoordinate init_tet_x = getTetriminoX (t);
  Tcoordinate init_tet_y = getTetriminoY (t);

  // Attempt to kick the piece
  int i = 0;
  do {
    setTetriminoX (t, init_tet_x + kicks[i][0]);
    setTetriminoY (t, init_tet_y + kicks[i][1]);

    i++;
  } while(!isBotBoardStateValid (b) && i < 5);
}
static void botPerformRotateCCW (Tbot_board *b) {
  Ttetrimino *t = getBotBoardActiveTetrimino (b);
  
  // If the piece is not an O (the O does not rotate)
  if (getTetriminoShape (t) == O) {
    return;
  }
  
  Tcoordinate_diff (*kicks)[2];
  
  // Rotate the piece
  moveTetriminoCCW (t);
  Tcoordinate init_tet_x = getTetriminoX (t);
  Tcoordinate init_tet_y = getTetriminoY (t);
  
  // Get the kicks for the avtive piece and its rotation state
  if (getTetriminoShape (t) == I) {
    kicks = I_kicks[getTetriminoRotationState (t)];
  } else {
    kicks = not_I_kicks[getTetriminoRotationState (t)];
  }
  
  // Attempt to kick the piece
  int i = 0;
  do {
    setTetriminoX (t, init_tet_x - kicks[i][0]);
    setTetriminoY (t, init_tet_y - kicks[i][1]);
    
    i++;
  } while(!isBotBoardStateValid (b) && i < 5);
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
void botApplyInput (Tbot_board *b, Tnext_queue *next_queue, Tbot_movement mv) {
  // For each movement in the input, apply it to the board
  if (isBotMovementInWord (&mv, BOT_MV_LEFT )) botPerformMoveLeft (b);
  if (isBotMovementInWord (&mv, BOT_MV_RIGHT)) botPerformMoveRight (b);
  if (isBotMovementInWord (&mv, BOT_MV_CW   )) botPerformRotateCW (b);
  if (isBotMovementInWord (&mv, BOT_MV_CCW  )) botPerformRotateCCW (b);
  if (isBotMovementInWord (&mv, BOT_MV_SD   )) botPerformSoftDrop (b);
  if (isBotMovementInWord (&mv, BOT_MV_HD   )) botPerformHardDrop (b);
  if (isBotMovementInWord (&mv, BOT_MV_HOLD )) botPerformHold (b, next_queue);
}

// --------------------------------------------------------------------------
//                        PATHFINDING STUFF
// --------------------------------------------------------------------------

void setMoveNode (TMoveNode* node, Tbot_movement move, Ttetrimino* tetrimino, Tbyte distance, TMoveNode* parent) {
  node->move = move;
  copyTetrimino (&(node->tetrimino), tetrimino);
  node->dist = distance;
  node->best_parent = parent;
}

void addMoveNodeToList (TMoveNode* mvnode, TMoveNodeList* list) {
  switch (mvnode->dist) {
  case 0:
    list->items_0[list->tail_0] = mvnode;
    list->tail_0 = (list->tail_0 + 1) % MOVE_NODE_LIST_MAX_ITEMS;
    break;
  
  case 1:
    list->items_1[list->tail_1] = mvnode;
    list->tail_1 = (list->tail_1 + 1) % MOVE_NODE_LIST_MAX_ITEMS;
    break;

  default:
    list->items_else[list->size_else] = mvnode;
    list->size_else++;
    break;
  }
}
TMoveNode* popMinMoveNodeFromList (TMoveNodeList *list) {
  // Removes and returns the node with smallest distance from start
  // Returns NULL is queue is empty

  // First look in the zero and 1 distances
  if (list->head_0 != list->tail_0) {
    TMoveNode* res = list->items_0[list->head_0];
    list->head_0 = (list->head_0 + 1) % MOVE_NODE_LIST_MAX_ITEMS;
    return res;
  }
  
  if (list->head_1 != list->tail_1) {
    TMoveNode* res = list->items_1[list->head_1];
    list->head_1 = (list->head_1 + 1) % MOVE_NODE_LIST_MAX_ITEMS;
    return res;
  }

  if (list->size_else == 0) {
    return NULL;
  }

  // Then look in the other distances. Reaching this code should
  // be extremely rare, so we do not care about efficiency
  float min = 1.0/0.0; // Distances start from 0
  TMoveNode* min_node;
  unsigned int min_i;

  // We find the node with the least distance
  for (unsigned int i = 0; i < list->size_else; i++) {
    TMoveNode* min_candidate = list->items_else[i];
    if (min_candidate->dist < min) {
      min = min_candidate->dist;
      min_node = min_candidate;
      min_i = i;
    }
  }

  // Delete the min node from the list by shifting the others
  for (unsigned int i = min_i; i < list->size_else; i++) {
    list->items_else[i] = list->items_else[i+1];
  }
  list->size_else--;

  return min_node;
}

bool isNotObstacle (Tbot_board* b, Ttetrimino* t) {
  // Checks that the position of the active tetrimino is valid in relation to the matrix.
  // Returns true is the board is valid, false if not. (Variation of isBotBoardStateValid for pathfinding)

  int i = 0;
  bool res = true;
  // For each mino
  do {
    Tmino *tmp_mino = getIthMino (t, i);
    Tcoordinate tmp_x = getTetriminoX (t) + getMinoXDiff (tmp_mino);
    Tcoordinate tmp_y = getTetriminoY (t) + getMinoYDiff (tmp_mino);

    // Check that the mino is inside of the matrix
    if (   tmp_x >= BOT_MATRIX_WIDTH
        || tmp_x < 0
        || tmp_y < 0) {

      res = false;
    }

    // Check that the mino does not intersect with minos on the board
    if (res) {
      Tbot_matrix *tmp_m = getBotBoardMatrix(b);
      if (getBotMatrixCellFilledState (tmp_m, tmp_x, tmp_y)) {
        res = false;
      }
    }

    i++;
  } while ((i < NUMBER_OF_MINOS) && res);

  return res;
}
static void pathfinderPerformMoveLeft (Tbot_board *b, Ttetrimino *t) {
  moveTetriminoLeft (t);
}
static void pathfinderPerformMoveRight (Tbot_board *b, Ttetrimino *t) {
  moveTetriminoRight (t);
}
static void pathfinderPerformRotateCW (Tbot_board *b, Ttetrimino *t) {
  // If the piece is not an O (the O does not rotate)
  if (getTetriminoShape (t) == O) {
    return;
  }

  Tcoordinate_diff (*kicks)[2];

  // Get the kicks for the avtive piece and its rotation state
  if (getTetriminoShape (t) == I) {
    kicks = I_kicks[getTetriminoRotationState (t)];
  } else {
    kicks = not_I_kicks[getTetriminoRotationState (t)];
  }

  // Rotate the piece
  moveTetriminoCW (t);
  Tcoordinate init_tet_x = getTetriminoX (t);
  Tcoordinate init_tet_y = getTetriminoY (t);

  // Attempt to kick the piece
  int i = 0;
  do {
    setTetriminoX (t, init_tet_x + kicks[i][0]);
    setTetriminoY (t, init_tet_y + kicks[i][1]);

    i++;
  } while(!isNotObstacle (b, t) && i < 5);
}
static void pathfinderPerformRotateCCW (Tbot_board *b, Ttetrimino *t) {

  // If the piece is not an O (the O does not rotate)
  if (getTetriminoShape (t) == O) {
    return;
  }

  Tcoordinate_diff (*kicks)[2];
  
  // Rotate the piece
  moveTetriminoCCW (t);
  Tcoordinate init_tet_x = getTetriminoX (t);
  Tcoordinate init_tet_y = getTetriminoY (t);

  // Get the kicks for the avtive piece and its rotation state
  if (getTetriminoShape (t) == I) {
    kicks = I_kicks[getTetriminoRotationState (t)];
  } else {
    kicks = not_I_kicks[getTetriminoRotationState (t)];
  }

  // Attempt to kick the piece
  int i = 0;
  do {
    setTetriminoX (t, init_tet_x - kicks[i][0]);
    setTetriminoY (t, init_tet_y - kicks[i][1]);

    i++;
  } while(!isNotObstacle (b, t) && i < 5);
}
static void pathfinderPerformSoftDrop (Tbot_board *b, Ttetrimino *t) {
  moveTetriminoDown (t);
}
void applyBotMoveToTetrimino (Tmovement mv, Ttetrimino* t, Tbot_board* board) {
  // Applies the given move to the tetrimino
  switch  (mv) {
    case BOT_MV_LEFT:   pathfinderPerformMoveLeft  (board, t); break;
    case BOT_MV_RIGHT:  pathfinderPerformMoveRight (board, t); break;
    case BOT_MV_CW:     pathfinderPerformRotateCW  (board, t); break;
    case BOT_MV_CCW:    pathfinderPerformRotateCCW (board, t); break;
    case BOT_MV_SD:     pathfinderPerformSoftDrop  (board, t); break;
    case BOT_MV_SONICD:
      do {
        moveTetriminoDown (t);
      } while(isNotObstacle (board, t));
      moveTetriminoUp(t);
    break;
  }
}


// --------------------------------------------------------------------------
//                        PATTERN MATCHING STUFF
// --------------------------------------------------------------------------

int patternMatch (Tbot_board *b, Tpattern *pattern, Tcoordinate *heights) {
  // Returns the number of matching places in the board
  Tbot_matrix *m = getBotBoardMatrix (b);
  Trow *rows = m->rows;
  int pattern_count = 0;
  
  // For each column TODO : recheck bounds
  for (int column = 0; column < C_MATRIX_WIDTH-pattern->w+1; column++) {
    // For each row
    for (int row = 0; row < heights[column]+pattern->h; row++) {
      int i = 0;
      bool match = true;
      do {
        match = ((rows[row+i]<<column & pattern->filled[i]) == pattern->filled[i]) && (((~rows[row+i])<<column & pattern->empty[i]) == pattern->empty[i]);
        i++;
      } while(i < pattern->h && match);
      
      if (match) {
        pattern_count++;
      }
    }
  }
  
  return pattern_count;
}
