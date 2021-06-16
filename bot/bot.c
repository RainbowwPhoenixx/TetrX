#include "bot.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "bot_parameters.h"
#include "MC_weights.h"

#ifdef LOG_BOT_THINKING
#define LOGFILE "bot_debug.log"
#endif

#define MAX(a, b) (((a)>(b))?(a):(b))
#define MIN(a, b) (((a)<(b))?(a):(b))
#define ABS(val) (((val)<0)?(-(val)):(val))

// Patterns (bit fields are upside down)
Tpattern tsdslot_left = {
  .h = 3, .w = 3,
  .filled = {0b101,
             0b000,
             0b001},
  .empty  = {0b010,
             0b111,
             0b110}
};
Tpattern tsdslot_right = {
  .h = 3, .w = 3,
  .filled = {0b101,
             0b000,
             0b100},
  .empty  = {0b010,
             0b111,
             0b011}
};
Tpattern tstslot_left = {
  .h = 5, .w = 4,
  .filled = {0b0101,
             0b0001,
             0b0101,
             0b0000,
             0b0010},
  .empty  = {0b0010,
             0b0110,
             0b0010,
             0b1110,
             0b1100}
};
Tpattern tstslot_right = {
  .h = 5, .w = 4,
  .filled = {0b1010,
             0b1000,
             0b1010,
             0b0000,
             0b0100},
  .empty  = {0b0100,
             0b0110,
             0b0100,
             0b0111,
             0b0011}
};

// Flag accessors for communication between threads
bool getShouldOutputPieceFlag (Tbot *bot) {
  // Returns true if the should_output_piece_flag is set to true.
  // Returns false if the should_output_piece_flag is set to false.
  return bot->should_output_piece_flag;
}
void setShouldOutputPieceFlag (Tbot *bot, bool new_flag) {
  bot->should_output_piece_flag = new_flag;
}

bool getOutputPieceReadyFlag (Tbot *bot) {
  return bot->output_piece_ready_flag;
}
void setOutputPieceReadyFlag (Tbot *bot, bool new_flag) {
  bot->output_piece_ready_flag = new_flag;
}

bool getShouldResetSearchFlag (Tbot *bot) {
  return bot->should_reset_search_flag;
}
void setShouldResetSearchFlag (Tbot *bot, bool new_flag) {
  bot->should_reset_search_flag = new_flag;
}

bool getShouldEndBotFlag (Tbot *bot) {
  return bot->should_end_bot_flag;
}
void setShouldEndBotFlag (Tbot *bot, bool new_flag) {
  bot->should_end_bot_flag = new_flag;
}

bool getNewPiecesReadyFlag (Tbot *bot) {
  return bot->new_pieces_ready_flag;
}
void setNewPiecesReadyFlag (Tbot *bot, bool new_flag) {
  bot->new_pieces_ready_flag = new_flag;
}

Tbyte log_depth = 0;
#ifdef LOG_BOT_THINKING
// Debug functions
int explored_nodes = 0;
void logBestNode (FILE *logfile, Tnode *node, Tbot *bot, time_t thinking_time) {
  fprintf(logfile, "Chosen node had a score of : Normal: %6.2f\tAccumulated: %6.2f\n", getNodeBoardValue(node), getNodeAccumulatedBoardValue (node));
  fprintf(logfile, "Initial rank : %3d/ %3d\n", getNodeInitialRank (node), getNodeNbOfChildren (getNodeImmediateParent (node)));
  Ttetrimino *tet = getBotBoardActiveTetrimino (getNodeBotBoard (node));
  fprintf(logfile, "Piece : %d, %d, %d, %d\n", getTetriminoShape (tet), getTetriminoX (tet), getTetriminoY (tet), getTetriminoRotationState (tet));
  Tline_clear lines = getNodeLinesCleared (node);
  fprintf(logfile, "Attack : lines: %d \t type: %d\n", lines.nb_of_lines, lines.attack_kind);
  fprintf(logfile, "Moves : ");
  for (int i = 0; i < node->nb_of_moves; i++) {
    fprintf(logfile, "%d ", node->moves[i]);
  }
  fprintf(logfile, "\n");
  fprintf(logfile, "Thinking depth :%2d\n", log_depth);
  fprintf(logfile, "Explored nodes :%8d in %f ms/node (cpu time)\n", explored_nodes, (float) 1000*thinking_time/(CLOCKS_PER_SEC*explored_nodes));
  fprintf(logfile, "Allocated nodes :%7d\n", bot->pool.total_node_count);
  fprintf(logfile, "\n");
  fflush (logfile);
}
#endif

static void computeHeights (Tbot_board *board, Tcoordinate *column_heights) {
  // Computes the maximum heights for each column
  for (Tcoordinate i = 0; i < BOT_MATRIX_WIDTH; i++) {
    Tcoordinate j = BOT_MATRIX_HEIGHT-1;
    while ((j > -1) && !getBotMatrixCellFilledState (getBotBoardMatrix (board), i, j)) {
      j--;
    }
    column_heights [i] = j+1;
  }
}
static float computeBumpiness (Tcoordinate *column_heights, int well) {
  // Compute the bumpiness of the field while ignoring the well column
  
  int bumpiness = 0;
  int diff;

  for (Tcoordinate i = 0; i < well-2; i++) {
    diff = column_heights[i] - column_heights[i+1];
    bumpiness += ABS(diff);
  }
  
  for (Tcoordinate i = well+1; i < BOT_MATRIX_WIDTH-1; i++) {
    diff = column_heights[i] - column_heights[i+1];
    bumpiness += ABS(diff);
  }

  return (float) bumpiness;
}
static float computeBumpinessSquared (Tcoordinate *column_heights, int well) {
  // Compute the bumpiness variance of the field while ignoring the well column
  
  int bumpiness = 0;
  int diff;

  for (Tcoordinate i = 0; i < well-2; i++) {
    diff = column_heights[i] - column_heights[i+1];
    bumpiness += ABS(diff*diff);
  }
  
  for (Tcoordinate i = well+1; i < BOT_MATRIX_WIDTH-1; i++) {
    diff = column_heights[i] - column_heights[i+1];
    bumpiness += ABS(diff*diff);
  }

  return (float) bumpiness;
}
static float computeAvrgHeight (Tcoordinate *column_heights) {
  int height_sum = 0;

  for (Tcoordinate i = 0; i < C_MATRIX_WIDTH; i++) {
    height_sum += column_heights [i];
  }

  return ((float) height_sum)/C_MATRIX_WIDTH;
}
static float computeMaxHeight (Tcoordinate *column_heights) {
  int max_height = 0;

  for (Tcoordinate i = 0; i < C_MATRIX_WIDTH; i++) {
    max_height = (column_heights[i]>max_height)?(column_heights[i]):(max_height);
  }

  return (float) max_height;
}
static float computeNumberOfHoles (Tbot_board *board, Tcoordinate *column_heights) {
  int nb_of_holes = 0;
  for (Tcoordinate i = 0; i < C_MATRIX_WIDTH; i++) {
    for (Tcoordinate j = 0; j < column_heights[i]; j++) {
      if (!getBotMatrixCellFilledState (getBotBoardMatrix (board), i, j)) {
        nb_of_holes++;
      }
    }
  }
  return (float) nb_of_holes;
}
static void evaluateNode (Tnode *node, float w[WEIGHT_COUNT]) {
  float score = 50.0;
  // Look at the state of node, calculate its score and set its score field.
  Tbot_board *board = getNodeBotBoard (node);
  Tcoordinate heights[BOT_MATRIX_WIDTH];
  computeHeights (board, heights);

  // Potential future criteria : time, swag, damage, damage/line, death

  float piece_landing_height = (float) getTetriminoY (getBotBoardActiveTetrimino(board));
  int max_height = computeMaxHeight (heights);
  int avrg_height = computeAvrgHeight (heights);
  float max_height_score = max_height - w[target_maximum_height];
  float avrg_height_score = avrg_height - w[target_average_height];
  float lines_score, spin_score = 0;
  Tline_clear lines_cleared = getNodeLinesCleared(node);

  // Line clear bonus
  if (lines_cleared.attack_kind == NORMAL) {
    switch (lines_cleared.nb_of_lines) {
      case 1: lines_score = w[clear1];  break;
      case 2: lines_score = w[clear2];  break;
      case 3: lines_score = w[clear3];  break;
      case 4: lines_score = w[clear4]; break;
      default: lines_score = 0; break;
    }
  }

  // Tspin bonus
  if (lines_cleared.attack_kind == TSPIN) {
    switch (lines_cleared.nb_of_lines) {
      case 1: lines_score = w[tss];  break;
      case 2: lines_score = w[tsd];  break;
      case 3: lines_score = w[tst];  break;
      default: lines_score = 0; break;
    }
  }
  
  // Well detection
  int well = 0;
  for (int i = 0; i < C_MATRIX_WIDTH; i++) {
    if (heights[well] > heights[i]) {
      well = i;
    }
  }
  
  // Favorize boards where both sides of the well are the same height
  if ((well != 0) && (well != 9) && (heights[well-1] == heights[well+1])) {
    score += w[well_same_height];
  }
  
  // Well position score
  score += w[well0 + well];

  score += lines_score;
  score += spin_score;
  score += w[landing_height]*piece_landing_height;
  score += w[maximum_height]*ABS(max_height_score);
  score += w[average_height]*ABS(avrg_height_score);
  score += w[bumpiness]*computeBumpiness (heights, well);
  score += w[bumpiness_sq]*computeBumpinessSquared (heights, well);
  score += w[number_of_holes]*computeNumberOfHoles (board, heights);
  score += w[tsd_slot]*patternMatch (board, &tsdslot_left, heights);
  score += w[tsd_slot]*patternMatch (board, &tsdslot_right, heights);
  score += w[tst_slot]*patternMatch (board, &tstslot_right, heights);
  score += w[tst_slot]*patternMatch (board, &tstslot_right, heights);
  
  // PC bonus
  if (max_height == 0) {
    score += w[pc];
    lines_cleared.attack_kind = PC;
  }

  setNodeBoardValue (node, score);
  setNodeAccumulatedBoardValue (node, score);

  #ifdef LOG_BOT_THINKING
  explored_nodes++;
  #endif
}
static void backpropagateScore (Tnode *tree_root, Tnode *best_node) {
  Tnode *current_node = best_node;
  float acc_score = getNodeBoardValue (best_node);
  
  acc_score += getNodeBoardValue (current_node);
  while ((current_node != tree_root) && (getNodeAccumulatedBoardValue(current_node) < acc_score)) {
    // Update parent score
    setNodeAccumulatedBoardValue (current_node, acc_score);
    current_node = getNodeImmediateParent (current_node);
    acc_score = getNodeBoardValue (current_node) + acc_score;
    
    // Re-sort the node according to accumulated score
    // Could be changed to a more naive approach that should be faster
    // Cause we could use the fact that we could know the index of the
    // node that is out of place (the one we just updated)
    sortNodeChildren (current_node);
  }
}
static Tnode *addNode(Tbot *bot, Tnode *parent, Tbot_movement *moves, Tbyte nb_of_moves, Tnext_queue *next_queue) {
  // Computes the board state of the given seqeunce of movements, computes its score, and adds it as a node
  // Returns a pointer to the created node

  // Create node with the moves applied
  Tbot_board new_board;
  copyBotBoard (&new_board, getNodeBotBoard (parent));
  botPopTetriminoFromQueue (&new_board, next_queue);
  for (Tbyte j = 0; j < nb_of_moves-1; j++) {
    if (moves[j] == BOT_MV_SONICD) {
      // If sonic drop is last move, just ignore it
      if (j < nb_of_moves-1) {
        // Replace sonic drops with a bunch of soft drops
        botApplyInput (&new_board, next_queue, BOT_MV_HD);
      }
    } else {
      botApplyInput (&new_board, next_queue, moves[j]);
    }
  }
  Tattack_kind attack = NORMAL;
  // Tspin detection
  // If piece is a T and last move was a rotation
  Tbot_movement rotations = BOT_MV_CW | BOT_MV_CCW;
  if ((getTetriminoShape (getBotBoardActiveTetrimino (&new_board)) == T) && isBotMovementInWord (&rotations, moves[nb_of_moves-2])) {
    Tbot_matrix *tmp_mat = getBotBoardMatrix (&new_board);
    Ttetrimino *tmp_tet = getBotBoardActiveTetrimino (&new_board);
    Tcoordinate tet_x = getTetriminoX (tmp_tet);
    Tcoordinate tet_y = getTetriminoY (tmp_tet);
    // If three corners are filled
    Tbyte corners = 0;
    corners += getBotMatrixCellFilledState (tmp_mat, tet_x+1, tet_y+1);
    corners += getBotMatrixCellFilledState (tmp_mat, tet_x+1, tet_y-1);
    corners += getBotMatrixCellFilledState (tmp_mat, tet_x-1, tet_y-1);
    corners += getBotMatrixCellFilledState (tmp_mat, tet_x-1, tet_y+1);
    if (corners >= 3) {
      // Then it's a tspin
      attack = TSPIN;
    }
  }
  botApplyInput (&new_board, next_queue, moves[nb_of_moves-1]); // Change moves[nb_of_moves-1] to BOT_MV_HD since it should always be that ?
  botLockActiveTetrimino (&new_board);

  Tline_clear lines = createLineClear (botClearLines (&new_board), attack);
  Tnode *new_node = createNode (new_board, nb_of_moves, moves, parent, &bot->pool);
  setNodeLinesCleared (new_node, lines);

  // Add new node to the search tree, compute score, and update parent data
  evaluateNode (new_node, bot->weights);
  addChildToNode (parent, new_node);

  return new_node;
}
static bool isRestingOnBlock (Tbot_board *board, Ttetrimino* t) {
  // Returns true if the position can be a final position
  // (aka if the tetrimino is resting on a block)

  bool res = false;
  Tbyte i = 0;

  while (i < 4 && !res) {
    Tmino *tmp_mino = getIthMino (t, i);
    Tcoordinate tmp_x = getTetriminoX (t) + getMinoXDiff (tmp_mino);
    Tcoordinate tmp_y = getTetriminoY (t) + getMinoYDiff (tmp_mino);
    // Mino touches the ground, or there is a mino below it on the matrix
    res = (tmp_y == 0) || getBotMatrixCellFilledState (getBotBoardMatrix (board), tmp_x, tmp_y - 1);
    i++;
  }

  return res;
}
static bool checkDuplicate (Ttetrimino* t, TMoveNode move_nodes[11][45][4]) {
  // Check if the space that the tetrimino occupies is already accouted for
  Tshape shape = getTetriminoShape(t);
  if ((shape != I) && (shape != S) && (shape != Z)) {
    return false;
  }
  
  Tcoordinate x = getTetriminoX (t);
  Tcoordinate y = getTetriminoY (t);
  Trotation_state rot = getTetriminoRotationState(t);
  
  switch (rot) {
    case R0  : return move_nodes[(x+1)  ][y+1][R180].visited;
    case R90 : return move_nodes[(x+1)+1][y  ][R270].visited;
    case R180: return move_nodes[(x+1)  ][y-1][R0  ].visited;
    case R270: return move_nodes[(x+1)-1][y  ][R90 ].visited;
    default : return false;
  }
}
static void generateMoves (Tbot *bot, Tnode *parent, Tbot_board* board_state, Tnext_queue *next_queue, bool should_insert_hold_move) {
  // Generates all the possible moves form the given board state and adds them to the queue
  // Current implementation : Dijkstra

  // Possible optimizations :
  // - Reduce matrix height because you can't actually pathfind up

  // Move set that the pathfinder can use & duration estimates for them
  Tshape tet_shape = getTetriminoShape(getBotBoardActiveTetrimino(board_state));
  Tbyte move_set_beginning = 0;
  if (tet_shape == O) {
    move_set_beginning = 2; // Ignore rotations if piece is an O
  }
  Tbyte move_set_size = 5;
  Tbot_movement move_set[] = {BOT_MV_CW, BOT_MV_CCW, BOT_MV_LEFT, BOT_MV_RIGHT, BOT_MV_SONICD};
  Tbyte move_distances[] = {0, 0, 0, 0, 1}; // Sonic drop cost is calculated at the end

  // Node currently being examined
  TMoveNode* current;
  // Array used to keep track of which spots have been visited
  // visited [tetrimino_x][tetrimino_y][Rotation state]
  #define NB_OF_ROTATIONS 4
  #define MOVE_NODE_WIDTH BOT_MATRIX_WIDTH+1
  // Array used to know which node corresponds to which spot
  TMoveNode move_nodes[MOVE_NODE_WIDTH][C_MATRIX_HEIGHT][NB_OF_ROTATIONS];
  // List of unvisited nodes
  TMoveNodeList unvisited_move_nodes = {.head_0 = 0, .head_1 = 0, .size_else = 0, .tail_0 = 0, .tail_1 = 0};

  // Set all visited to false and all known to NULL (is it necessary ?)
  for (Tcoordinate x = 0; x < MOVE_NODE_WIDTH; x++) {
    for (Tcoordinate y = 0; y < C_MATRIX_HEIGHT; y++) {
      for (Tbyte r = 0; r < NB_OF_ROTATIONS; r++) {
        move_nodes[x][y][r].visited = false;
        move_nodes[x][y][r].dist = -1;
      }
    }
  }

  // Convert start node and set dist to 0
  Ttetrimino tmp_tet = createTetrimino (tet_shape);
  TMoveNode *start_node = &move_nodes[getTetriminoX (&tmp_tet)+1][getTetriminoY (&tmp_tet)][getTetriminoRotationState (&tmp_tet)];
  setMoveNode(start_node, 0, &tmp_tet, 0, NULL);
  addMoveNodeToList (start_node, &unvisited_move_nodes);

  while ((current = popMinMoveNodeFromList(&unvisited_move_nodes))) {
    current->visited = true;
    // Generate moves from this node, for each possible move
    for (Tbyte move_id = move_set_beginning; move_id < move_set_size; move_id++) {
      // For this considered move, we will:
      // - Compute the distance after the move
      // - Set the new neighbour values

      // Compute the distance
      Tbyte new_distance = current->dist + move_distances[move_id];
      if (current->best_parent && (current->best_parent->move == BOT_MV_SONICD)) {
        new_distance += 2 * (getTetriminoY (&current->best_parent->best_parent->tetrimino) - getTetriminoY (&current->best_parent->tetrimino));
      }

      // Here we could define methods on move nodes to get the neighbours efficiently
      Ttetrimino new_tet;
      copyTetrimino(&new_tet, &current->tetrimino);
      applyBotMoveToTetrimino(move_set[move_id], &new_tet, board_state);
      TMoveNode* neighbour = &move_nodes[getTetriminoX(&new_tet)+1][getTetriminoY(&new_tet)][getTetriminoRotationState(&new_tet)];

      if (!neighbour->visited && (new_distance < neighbour->dist) && isNotObstacle(board_state, &new_tet)) {
        setMoveNode(neighbour, move_set[move_id], &new_tet, new_distance, current);
        addMoveNodeToList(neighbour, &unvisited_move_nodes);
      }
    }

    // Add as a game tree node if the position is valid
    if (isRestingOnBlock (board_state, &(current->tetrimino))
        && !checkDuplicate(&(current->tetrimino), move_nodes)) {
      // Get path taken to get there
      Tbyte nb_of_moves = 0, final_nb_of_moves = 0;
      Tbot_movement reverse_moves[BOT_MAX_MOVES];
      TMoveNode* current_path_backtrack = current;

      do {
        reverse_moves[nb_of_moves] = current_path_backtrack->move;
        nb_of_moves++;
      } while((current_path_backtrack = current_path_backtrack->best_parent));
      nb_of_moves--; // To account for the initial node having no moves

      if (should_insert_hold_move) {
        reverse_moves[nb_of_moves] = BOT_MV_HOLD;
        nb_of_moves++;
      }

      // Take the backward moves and put them in order.
      Tbot_movement moves[BOT_MAX_MOVES];
      for (Tbyte j = 0; j < nb_of_moves; j++) {
        moves[final_nb_of_moves++] = reverse_moves[nb_of_moves -j-1];
      }
      moves[final_nb_of_moves++] = BOT_MV_HD;

      addNode (bot, parent, moves, final_nb_of_moves, next_queue);
    }
    // Stop if necessary (queue is empty)
    // Repeat with a new node
  }
  

}
static void expandNode (Tbot *bot, Tnode *search_tree_root, Tnext_queue *next_queue, struct drand48_data *RNG_data) {
  // Generate the possible moves from the given node, and assigns them a score

  // Node selection
  // Repeat until a node has no generated children
  Tnode *current_node_selection = search_tree_root;
  while (getNodeAreChildrenGenerated (current_node_selection)) {
    Tbyte max_index = getNodeNbOfChildren (current_node_selection);
    // RNG
    double random_float;
    drand48_r (RNG_data, &random_float);
    // Scale the float to the maximum reachable probability
    random_float *= choice_weights[max_index-1];
    
    // Turn float into an index
    Tbyte index = 0;
    while (random_float > choice_weights[index+1]) {
      index++;
    }
    current_node_selection = getNodeIthChild(current_node_selection, index);
  }
  Tnode *node = current_node_selection;
  
  if (node == NULL) {
    return;
  }
  
  // If max previews is reached, don't compute
  Tbyte depth = getBotBoardNextQueueOffset (getNodeBotBoard (node)) - getBotBoardNextQueueOffset (getNodeBotBoard (search_tree_root));
  if (depth >= bot->max_previews) {
    return;
  }
  log_depth = MAX (log_depth, depth);
  
  setNodeAreChildrenGenerated (node, true);

  Tbot_board tmp_board;
  copyBotBoard (&tmp_board, getNodeBotBoard (node));
  botPopTetriminoFromQueue (&tmp_board, next_queue);
  generateMoves (bot, node, &tmp_board, next_queue, false);
  // Only try holding if the hold piece is different
  if (getTetriminoShape(getBotBoardActiveTetrimino(&tmp_board)) != getBotBoardHoldPiece(&tmp_board)) {
    botApplyInput (&tmp_board, next_queue, BOT_MV_HOLD);
    generateMoves (bot, node, &tmp_board, next_queue, true);
  }
  
  // Backpropagate the score of the best node (at index 0 since nodes are sorted)
  sortNodeChildren (node);
  backpropagateScore (search_tree_root, getNodeIthChild (node, 0));
  computeChildrenInitialRanks (node);

}

// Various processing functions
static Tmovement translate_move(Tbot_movement move) {
  // Translate moves from the bot's thinking to the game's thinking
  switch (move) {
    case BOT_MV_LEFT   : return MV_LEFT   ; break;
    case BOT_MV_RIGHT  : return MV_RIGHT  ; break;
    case BOT_MV_CW     : return MV_CW     ; break;
    case BOT_MV_CCW    : return MV_CCW    ; break;
    case BOT_MV_SD     : return MV_SD     ; break;
    case BOT_MV_SONICD : return MV_SD     ; break;
    case BOT_MV_HD     : return MV_HD     ; break;
    case BOT_MV_HOLD   : return MV_HOLD   ; break;
  }
  return 0;
}
static void reduceNextPieceOffset (Tnode *node) {
  // Recursive function to reduce the next piece offset by one when the thinking for the next piece starts
  if (node) {
    for (unsigned short i = 0; i < getNodeNbOfChildren (node); i++) {
      reduceNextPieceOffset (getNodeIthChild (node, i));
    }

    setBotBoardNextQueueOffset (getNodeBotBoard (node), getBotBoardNextQueueOffset (getNodeBotBoard (node))-1);
  }
}
// Bot main loop
static void *bot_TetrX (void *_bot) {
  Tbot *bot = (Tbot*) _bot;
  Tnode *search_tree = createNode (convertBoardToBotBoard (&bot->master_board), 0, NULL, NULL, &bot->pool);
  Tnext_queue global_next_queue = createNextQueue ();
  copyNextQueue (&global_next_queue, getBoardNextQueue (&bot->master_board));
  // Init the PRNG for random node selection
  struct drand48_data RNG_data;
  srand48_r(time(0), &RNG_data);

  #ifdef LOG_BOT_THINKING
  // Setting up the log file for debugging
  FILE *logfile = fopen (LOGFILE, "w");
  time_t time = clock();
  #endif

  while (!getShouldEndBotFlag (bot)) {
    // Check all the flags
    if (getShouldOutputPieceFlag (bot) && getNodeAreChildrenGenerated (search_tree)  && !getNewPiecesReadyFlag (bot)) {
      // Because nodes are sorted
      Tnode *best_child = getNodeIthChild (search_tree, 0);
      
      #ifdef LOG_BOT_THINKING
      logBestNode (logfile, best_child, bot, clock() - time);
      explored_nodes = 0;
      log_depth = 0;
      time = clock();
      #endif

      // Output the moves of the next piece of the best path found
      bot->next_moves_length = best_child->nb_of_moves;
      for (int move = 0; move < best_child->nb_of_moves; move++) {
        bot->next_moves[move] = best_child->moves[move];
      }
      
      // Signal that the next piece is ready.
      setOutputPieceReadyFlag (bot, true);
      // Reset the flag
      setShouldOutputPieceFlag (bot, false);
      // Free all the search tree except for the best node
      setNodeIthChild (search_tree, 0, NULL);
      freeNode (search_tree, &bot->pool);
      // Advance to think on the next board state
      search_tree = best_child;
      advanceNextQueue (&global_next_queue);
      reduceNextPieceOffset (search_tree);
    }
    if (getShouldResetSearchFlag (bot)) {
      // Free the current search tree
      freeNode (search_tree, &bot->pool);
      // Get the new values
      search_tree = createNode (convertBoardToBotBoard (&bot->master_board), 0, NULL, NULL, &bot->pool);
      copyNextQueue (&global_next_queue, getBoardNextQueue (&bot->master_board));
      // Reset the flag
      setShouldResetSearchFlag (bot, false);
    }
    if (getNewPiecesReadyFlag (bot)) {
      Tbyte queue_length = getNextQueueLength (&bot->new_pieces);
      for (Tbyte i = 0; i < queue_length; i++) {
        addPieceToNextQueue (&global_next_queue, getIthNextPiece (&bot->new_pieces, 0));
        advanceNextQueue (&bot->new_pieces);
      }
      setNewPiecesReadyFlag (bot, false);
    }

    // Do the thinking
    // If queue is not empty, try expanding the new node, else pause for a bit
    if (true) {
      expandNode (bot, search_tree, &global_next_queue, &RNG_data);
    } else {usleep (1000);}
    // Generate the moves, compute their value, and put them in the queue
  }

  #ifdef LOG_BOT_THINKING
  // Setting up the log file for debugging
  fclose (logfile);
  #endif

  // Free the memory
  freeNode (search_tree, &bot->pool);

  return NULL;
}

// Bot interface with the game
static void startBot (Tbot *bot, Tboard board) {
  // Init the bot variables
  bot->master_board = board;
  bot->should_output_piece_flag = false;
  bot->output_piece_ready_flag = false;
  bot->should_reset_search_flag = false;
  bot->should_end_bot_flag = false;
  bot->new_pieces_ready_flag = false;
  bot->max_previews = BOT_MAX_PREVIEWS;
  
  // If file does not exist, create it with default weights
  if (access(WEIGHTS_FILE, R_OK) == -1) {
    createDefaultWeightsFile ();
  }
  // Read the weights
  readWeights (bot->weights);
  // Allocate the memory pool
  bot->pool = createNodeMemoryPool();

  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  // Create the bot thread
  pthread_create (&(bot->thread_id), &attr, bot_TetrX, (void*) bot);
}
static Tmovement getBotMovement (Tbot *bot, Tboard *board) {
  static Tmove_queue next_moves; // Initialized to 0 because it is static
  static Tbot_movement last_move = 0;

  if (getQueueSize (&next_moves) == 0) {
    // Ask the bot to prepare the input

    // When input is prepared, get it & reset flag
    if (getOutputPieceReadyFlag(bot)) {
      for (Tbyte i = 0; i < bot->next_moves_length; i++) {
        addMovementToQueue (&next_moves, bot->next_moves[i]);
      }
      bot->next_moves_length = 0;
      setOutputPieceReadyFlag (bot, false);
    } else if (!getShouldOutputPieceFlag (bot)) {
      setShouldOutputPieceFlag (bot, true);
    }
  }

  // next_moves acts as a stack
  Tbot_movement res = 0;
  if (getQueueSize (&next_moves) != 0) {
    switch (last_move) {
      case 0: res = popMoveFromQueue(&next_moves);  break;
      case BOT_MV_RIGHT: case BOT_MV_LEFT: res = 0; break;
      case BOT_MV_SONICD:
        // We are currently soft dropping
        // If the tetrimino hit the floor, stop dropping
        // If it is not, continue
        res = isTetriminoOnFloor(board) ? 0 : BOT_MV_SONICD;
        break;
      default: break;
    }
  }

  last_move = res;
  return translate_move(res);
}
static void endBot (Tbot *bot) {
  void *status;
  bot->should_end_bot_flag = true;
  if (!freeNodeMemoryPool (bot->pool)) {
    exit(-10);
  }
  // pthread_join (bot->thread_id, &status); // If thread terminates before calling this, segfault
}
static void addBagToBot (Tbot *bot, Tshape *new_bag) {
  static Tnext_queue next_queue_buffer;
  for (Tbyte i = 0; i < 7; i++) {
    addPieceToNextQueue (&next_queue_buffer, new_bag[i]);
  }

  if (!getNewPiecesReadyFlag (bot)) {
    Tbyte buffer_length = getNextQueueLength (&next_queue_buffer);
    for (Tbyte i = 0; i < buffer_length; i++) {
      addPieceToNextQueue (&bot->new_pieces, getIthNextPiece (&next_queue_buffer, 0));
      advanceNextQueue (&next_queue_buffer);
    }
    setNewPiecesReadyFlag (bot, true);
  }
}
Tinterface_in getBotInterface () {
  Tinterface_in bot_IO;
  bot_IO.initInputFunc = startBot;
  bot_IO.getInputFunc = getBotMovement;
  bot_IO.endInputFunc = endBot;
  bot_IO.addNewBagFunc = addBagToBot;

  return bot_IO;
}
