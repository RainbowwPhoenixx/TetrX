#include "bot.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "bot_parameters.h"
#include "MC_weights.h"

#ifdef LOG_BOT_THINKING
#define LOGFILE "bot_debug.log"
#endif

#define MAX(a, b) (((a)>(b))?(a):(b))
#define MIN(a, b) (((a)<(b))?(a):(b))
#define ABS(val) (((val)<0)?(-(val)):(val))

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
void logBestNode (FILE *logfile, Tnode *node) {
  fprintf(logfile, "Chosen node had a score of : Normal: %6.2f\tAccumulated: %6.2f\n", getNodeBoardValue(node), getNodeAccumulatedBoardValue (node));
  fprintf(logfile, "Initial rank : %d/%d\n", getNodeInitialRank (node), getNodeNbOfChildren (getNodeImmediateParent (node)));
  Ttetrimino *tet = getBotBoardActiveTetrimino (getNodeBotBoard (node));
  fprintf(logfile, "Piece : %d, %d, %d, %d\n", getTetriminoShape (tet), getTetriminoX (tet), getTetriminoY (tet), getTetriminoRotationState (tet));
  Tline_clear lines = getNodeLinesCleared (node);
  fprintf(logfile, "Attack : lines: %d \t type: %d\n", lines.nb_of_lines, lines.attack_kind);
  fprintf(logfile, "Moves : ");
  for (int i = 0; i < node->nb_of_moves; i++) {
    fprintf(logfile, "%d ", node->moves[i]);
  }
  fprintf(logfile, "\n");
  fprintf(logfile, "Thinking log_depth : %d\n", log_depth);
  fprintf(logfile, "Explored nodes : %d\n", explored_nodes);
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
static void evaluateNode (Tnode *node) {
  // Look at the state of node, calculate its score and set its score field.
  Tbot_board *board = getNodeBotBoard (node);
  Tcoordinate heights[BOT_MATRIX_WIDTH];
  computeHeights (board, heights);

  // Potential future criteria : time, swag, damage, damage/line, death

  float landing_height = (float) getTetriminoY (getBotBoardActiveTetrimino(board));
  int max_height = computeMaxHeight (heights);
  float max_height_score = max_height - 5;
  float avrg_height_score = computeAvrgHeight (heights) - 6;
  float lines_score, spin_score = 0;
  Tline_clear lines_cleared = getNodeLinesCleared(node);

  // Line clear bonus
  switch (lines_cleared.nb_of_lines) {
    case 0: lines_score = 0;  break;
    case 1: lines_score = -25;  break;
    case 2: lines_score = -10;  break;
    case 3: lines_score =  -5;  break;
    case 4: lines_score = 50; break;
    default: lines_score = 0; break;
  }

  // Tspin bonus
  if (lines_cleared.attack_kind == TSPIN) {
    spin_score = 500*lines_cleared.nb_of_lines;
  }
  
  // Well detection
  int well = 0;
  for (int i = 0; i < C_MATRIX_WIDTH; i++) {
    if (heights[well] > heights[i]) {
      well = i;
    }
  }

  // The score needs to be on average positive for the best nodes for MCTS
  // so it is initialized with a non 0 value.
  float score = 50.0;
  score += lines_score;
  score += spin_score;
  score += -5*landing_height;
  score += -1*ABS(max_height_score);
  score += -5*ABS(avrg_height_score);
  score += -.8*computeBumpiness (heights, well);
  score += -.05*computeBumpinessSquared (heights, well);
  score += -100*computeNumberOfHoles (board, heights);
  
  // PC bonus
  if (max_height == 0) {
    score += 1000;
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
static Tnode *addNode(Tnode *parent, Tbot_movement *moves, Tbyte nb_of_moves, Tnext_queue *next_queue) {
  // Computes the board state of the given seqeunce of movements, computes its score, and adds it as a node
  // Returns a pointer to the created node

  // Create node with the moves applied
  Tbot_board new_board;
  copyBotBoard (&new_board, getNodeBotBoard (parent));
  botPopTetriminoFromQueue (&new_board, next_queue);
  for (Tbyte j = 0; j < nb_of_moves-1; j++) {
    botApplyInput (&new_board, next_queue, moves[j]);
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
  Tnode *new_node = createNode (new_board, nb_of_moves, moves, parent);
  setNodeLinesCleared (new_node, lines);

  // Add new node to the search tree, compute score, and update parent data
  evaluateNode (new_node);
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
static void generateMoves (Tnode *parent, Tbot_board* board_state, Tnext_queue *next_queue, bool should_insert_hold_move) {
  // Generates all the possible moves form the given board state and adds them to the queue
  // Current implementation : Dijkstra

  // Possible optimizations :
  // - Reduce matrix height because you can't actually pathfind up

  // Move set that the pathfinder can use & durantion estimates for them
  Tbyte move_set_size = 5;
  Tbot_movement move_set[] = {BOT_MV_CW, BOT_MV_CCW, BOT_MV_LEFT, BOT_MV_RIGHT, BOT_MV_SONICD};
  Tbyte move_distances[] = {0, 0, 0, 0, 1}; // Sonic drop cost is calculated at the end

  // Node currently being examined
  TMoveNode* current;
  // Array used to keep track of which spots have been visited
  // visited [tetrimino_x][tetrimino_y][Rotation state]
  #define NB_OF_ROTATIONS 4
  #define VISITED_WIDTH BOT_MATRIX_WIDTH+1
  bool visited [VISITED_WIDTH][BOT_MATRIX_HEIGHT][NB_OF_ROTATIONS];
  // Array used to know which node corresponds to which spot
  TMoveNode* known_nodes[VISITED_WIDTH][C_MATRIX_HEIGHT][NB_OF_ROTATIONS];
  // List of unvisited nodes
  TMoveNodeList unvisited_move_nodes = {.size = 0};
  // List of final nodes ?

  // Set all visited to false and all known to NULL (is it necessary ?)
  for (Tcoordinate x = 0; x < VISITED_WIDTH; x++) {
    for (Tcoordinate y = 0; y < C_MATRIX_HEIGHT; y++) {
      for (Tbyte r = 0; r < NB_OF_ROTATIONS; r++) {
        visited[x][y][r] = false;
        known_nodes[x][y][r] = NULL;
      }
    }
  }

  // Convert start node and set dist to 0
  Ttetrimino tmp_tet = createTetrimino (getTetriminoShape(getBotBoardActiveTetrimino(board_state)));
  TMoveNode *start_node = createMoveNode (0, &tmp_tet, 0, NULL);
  addMoveNodeToList (start_node, &unvisited_move_nodes);
  known_nodes[getTetriminoX (&tmp_tet)+1][getTetriminoY (&tmp_tet)][ getTetriminoRotationState (&tmp_tet)] = start_node;

  while ((current = popMinMoveNodeFromList (&unvisited_move_nodes))) {
    // Consider/generate all unvisited neighbours and set their dist
    for (Tbyte i = 0; i < move_set_size; i++) {
      float new_distance = current->dist + move_distances[i];
      if (current->best_parent && (current->best_parent->move == BOT_MV_SONICD)) {
        new_distance += 2 * (getTetriminoY (&current->best_parent->best_parent->tetrimino) - getTetriminoY (&current->best_parent->tetrimino));
      }
      TMoveNode* potential_new_neighbour = createMoveNode (move_set[i], &(current->tetrimino), new_distance, current);
      Ttetrimino* new_tetrimino = &(potential_new_neighbour->tetrimino);
      applyBotMoveToTetrimino (move_set[i], new_tetrimino, board_state);
      
      // If node is not visited & is not an obstacle
      if (isNotObstacle (board_state, new_tetrimino)
          && !visited[getTetriminoX (new_tetrimino)+1][getTetriminoY (new_tetrimino)][ getTetriminoRotationState (new_tetrimino)]) {
        // If node is not known, create it, else compare distances and set the shortest
        TMoveNode* neighbour = known_nodes[getTetriminoX (new_tetrimino)+1][getTetriminoY (new_tetrimino)][getTetriminoRotationState (new_tetrimino)];
        if (!neighbour) {
          neighbour = potential_new_neighbour;
          known_nodes[getTetriminoX (new_tetrimino)+1][getTetriminoY (new_tetrimino)][ getTetriminoRotationState (new_tetrimino)] = neighbour;
          addMoveNodeToList (neighbour, &unvisited_move_nodes);
        } else {
          if ((potential_new_neighbour->dist < neighbour->dist) && (current != neighbour)) {
            neighbour->dist = potential_new_neighbour->dist;
            neighbour->best_parent = current;
          }
          destroyMoveNode (potential_new_neighbour);
        }
      } else {destroyMoveNode (potential_new_neighbour);}
    }
    // Mark the current node as visited
    visited[getTetriminoX (&(current->tetrimino))+1][getTetriminoY (&(current->tetrimino))][getTetriminoRotationState (&(current->tetrimino))] = true;
    // Add as a tree node if position is final
    if (isRestingOnBlock (board_state, &(current->tetrimino))) {
      // Get path taken to get there
      Tbyte nb_of_moves = 0, final_nb_of_moves = 0;
      Tbot_movement reverse_moves[BOT_MAX_MOVES];
      TMoveNode* current_path_backtrack = current;
      Ttetrimino spin_check_tet = createTetrimino (getTetriminoShape(getBotBoardActiveTetrimino(board_state)));

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
      // Also handle special cases for sonic drop.
      // Apply the moves to a tetrimino as we go along to know how much to soft drop
      Tbot_movement moves[BOT_MAX_MOVES];
      for (Tbyte j = 0; j < nb_of_moves; j++) {
        Tmovement move_to_add = reverse_moves[nb_of_moves -j-1];
        if (move_to_add == BOT_MV_SONICD) {
          // If sonic drop is last move, just ignore it
          if (j < nb_of_moves-1) {
            // Replace sonic drops with a bunch of soft drops
            do  {
              moves[final_nb_of_moves++] = BOT_MV_SD;
              moveTetriminoDown (&spin_check_tet);
            } while(isNotObstacle (board_state, &spin_check_tet));
            moveTetriminoUp(&spin_check_tet);
            final_nb_of_moves--;
          }
        } else {
          moves[final_nb_of_moves++] = move_to_add;
          applyBotMoveToTetrimino (move_to_add, &spin_check_tet, board_state);
        }
      }
      moves[final_nb_of_moves++] = BOT_MV_HD;

      addNode (parent, moves, final_nb_of_moves, next_queue);
    }
    // Stop if necessary (queue is empty)
    // Repeat with a new node
  }

  // Free all the move nodes
  for (Tcoordinate x = 0; x < VISITED_WIDTH; x++) {
    for (Tcoordinate y = 0; y < C_MATRIX_HEIGHT; y++) {
      for (Tbyte r = 0; r < NB_OF_ROTATIONS; r++) {
        if (known_nodes[x][y][r]) {
          destroyMoveNode (known_nodes[x][y][r]);
        }
      }
    }
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
  generateMoves (node, &tmp_board, next_queue, false);
  botApplyInput (&tmp_board, next_queue, BOT_MV_HOLD);
  generateMoves (node, &tmp_board, next_queue, true);
  
  // Backpropagate the score of the best node (at index 0 since nodes are sorted)
  sortNodeChildren (node);
  backpropagateScore (search_tree_root, getNodeIthChild (node, 0));
  computeChildrenInitialRanks (node);

}

// Various processing functions
static void translate_moves (Tbot_movement *src_moves, Tbyte src_nb_of_moves,Tmovement *dest_moves, Tbyte *dest_nb_of_moves) {
  // Translate moves from the bot's thinking to the game's thinking

  // Space out side moves (hypertap, not DAS)
  *dest_nb_of_moves = 0;
  for (Tbyte i = 0; i < src_nb_of_moves; i++) {
    Tmovement new_move;
    switch (src_moves[i]) {
      case BOT_MV_LEFT   : new_move = MV_LEFT   ; break;
      case BOT_MV_RIGHT  : new_move = MV_RIGHT  ; break;
      case BOT_MV_CW     : new_move = MV_CW     ; break;
      case BOT_MV_CCW    : new_move = MV_CCW    ; break;
      case BOT_MV_SD     : new_move = MV_SD     ; break;
      case BOT_MV_SONICD : new_move = 0         ; break;
      case BOT_MV_HD     : new_move = MV_HD     ; break;
      case BOT_MV_HOLD   : new_move = MV_HOLD   ; break;
      default            : new_move = 0;
    }

    dest_moves[*dest_nb_of_moves] = new_move;
    (*dest_nb_of_moves)++;
    if (isBotMovementInWord (src_moves+i, BOT_MV_LEFT) || isBotMovementInWord (src_moves+i, BOT_MV_RIGHT) || isBotMovementInWord (src_moves+i, BOT_MV_CW) || isBotMovementInWord (src_moves+i, BOT_MV_CCW)) {
      dest_moves [*dest_nb_of_moves] = createMovementWord ();
      (*dest_nb_of_moves)++;
    }
  }
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
  Tnode *search_tree = createNode (convertBoardToBotBoard (&bot->master_board), 0, NULL, NULL);
  Tnext_queue global_next_queue = createNextQueue ();
  copyNextQueue (&global_next_queue, getBoardNextQueue (&bot->master_board));
  // Init the PRNG for random node selection
  struct drand48_data RNG_data;
  srand48_r(time(0), &RNG_data);

  #ifdef LOG_BOT_THINKING
  // Setting up the log file for debugging
  FILE *logfile = fopen (LOGFILE, "w");
  #endif

  while (!getShouldEndBotFlag (bot)) {
    // Check all the flags
    if (getShouldOutputPieceFlag (bot) && getNodeAreChildrenGenerated (search_tree)  && !getNewPiecesReadyFlag (bot)) {
      // Because nodes are sorted
      Tnode *best_child = getNodeIthChild (search_tree, 0);
      
      #ifdef LOG_BOT_THINKING
      logBestNode (logfile, best_child);
      explored_nodes = 0;
      log_depth = 0;
      #endif

      // Output the moves of the next piece of the best path found
      bot->next_moves_length = best_child->nb_of_moves;
      translate_moves ((Tbot_movement*) &(best_child->moves), best_child->nb_of_moves, bot->next_moves, &bot->next_moves_length);
      // Signal that the next piece is ready.
      setOutputPieceReadyFlag (bot, true);
      // Reset the flag
      setShouldOutputPieceFlag (bot, false);
      // Free all the search tree except for the best node
      setNodeIthChild (search_tree, 0, NULL);
      freeNode (search_tree);
      // Advance to think on the next board state
      search_tree = best_child;
      advanceNextQueue (&global_next_queue);
      reduceNextPieceOffset (search_tree);
    }
    if (getShouldResetSearchFlag (bot)) {
      // Free the current search tree
      freeNode (search_tree);
      // Get the new values
      search_tree = createNode (convertBoardToBotBoard (&bot->master_board), 0, NULL, NULL);
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
  freeNode (search_tree);

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

  // Init the mutexes
  // pthread_mutexattr_t attr;
  // pthread_mutexattr_init (&attr);
  // pthread_mutexattr_settype (&attr, PTHREAD_MUTEX_RECURSIVE);
  // pthread_mutex_init (&(bot->should_output_piece_mutex), &attr);
  // pthread_mutex_init (&(bot->output_piece_ready_mutex), &attr);
  // pthread_mutex_init (&(bot->should_reset_search_mutex), &attr);
  // pthread_mutex_init (&(bot->should_end_bot_mutex), &attr);

  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  // Create the bot thread
  pthread_create (&(bot->thread_id), &attr, bot_TetrX, (void*) bot);
}
static Tmovement getBotMovement (Tbot *bot) {
  static Tmove_queue next_moves; // Initialized to 0 because it is static

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
  Tmovement res;
  if (getQueueSize (&next_moves) == 0) {
    res = createMovementWord ();
  } else {
    res = popMoveFromQueue (&next_moves);
  }
  return res;
}
static void endBot (Tbot *bot) {
  void *status;
  bot->should_end_bot_flag = true;
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
