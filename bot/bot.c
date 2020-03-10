#include "bot.h"
#include <unistd.h>
#include <stdio.h>

#define LOG_BOT_THINKING
#ifdef LOG_BOT_THINKING
#define LOGFILE "bot_debug.log"
#endif

#define BOT_MAX_MOVES 50

#define ABS(val) (((val)<0)?(-(val)):(val))
#define BOT_MAX_PREVIEWS 3 // Do not set lower than 1
// float accumulation_weights[20] = {1, 1/2, 1/3, 1/4, 1/5, 1/6, 1/7, 1/8, 1/9, 1/10, 1/11, 1/12, 1/13, 1/14, 1/15, 1/16, 1/17, 1/17, 1/19, 1/20};
// float accumulation_weights[20] = {1, 1/2, 1/4, 1/8, 1/16, 1/32, 1/64, 1/128, 1/256, 1/256, 1/256, 1/256, 1/256, 1/256, 1/256, 1/256, 1/256, 1/256, 1/256, 1/256};
float accumulation_weights[20] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

// Accessors
bool getShouldOutputPieceFlag (Tbot *bot) {
  // Returns true if the should_output_piece_flag is set to true.
  // Returns false if the should_output_piece_flag is set to false.
  return bot->should_output_piece_flag;
}
void setShouldOutputPieceFlag (Tbot *bot, bool new_flag) {
  bot->should_output_piece_flag = new_flag;
}
// bool getShouldOutputPieceFlagTry (Tbot *bot) {
//   // Returns true if the should_output_piece_flag is set to true.
//   // Returns false if the should_output_piece_flag is set to false.
//   // Returns false is a lock could not be acquired for the flag
//   // (So that the bot can keep thinking until the lock becomes available)
//   if (pthread_mutex_trylock (&(bot->should_output_piece_mutex)) == 0) {
//     bool res = bot->should_output_piece_flag;
//     pthread_mutex_unlock (&(bot->should_output_piece_mutex));
//     return res;
//   } else return false;
// }
// bool getShouldOutputPieceFlagWait (Tbot *bot) {
//   pthread_mutex_lock (&(bot->should_output_piece_mutex));
//   bool res = bot->should_output_piece_flag;
//   pthread_mutex_unlock (&(bot->should_output_piece_mutex));
//   return res;
// }
// void setShouldOutputPieceFlagWait (Tbot *bot, bool new_flag) {
//   pthread_mutex_lock (&(bot->should_output_piece_mutex));
//   bot->should_output_piece_flag = new_flag;
//   pthread_mutex_unlock (&(bot->should_output_piece_mutex));
// }

bool getOutputPieceReadyFlag (Tbot *bot) {
  return bot->output_piece_ready_flag;
}
void setOutputPieceReadyFlag (Tbot *bot, bool new_flag) {
  bot->output_piece_ready_flag = new_flag;
}
// bool getOutputPieceReadyFlagTry (Tbot *bot) {
//   if (pthread_mutex_trylock (&(bot->output_piece_ready_mutex)) == 0) {
//     bool res = bot->output_piece_ready_flag;
//     pthread_mutex_unlock (&(bot->output_piece_ready_mutex));
//     return res;
//   } else return false;
// }
// bool getOutputPieceReadyFlagWait (Tbot *bot) {
//   pthread_mutex_lock (&(bot->output_piece_ready_mutex));
//   bool res = bot->output_piece_ready_flag;
//   pthread_mutex_unlock (&(bot->output_piece_ready_mutex));
//   return res;
// }
// void setOutputPieceReadyFlagWait (Tbot *bot, bool new_flag) {
//   pthread_mutex_lock (&(bot->output_piece_ready_mutex));
//   bot->output_piece_ready_flag = new_flag;
//   pthread_mutex_unlock (&(bot->output_piece_ready_mutex));
// }

bool getShouldResetSearchFlag (Tbot *bot) {
  return bot->should_reset_search_flag;
}
void setShouldResetSearchFlag (Tbot *bot, bool new_flag) {
  bot->should_reset_search_flag = new_flag;
}
// bool getShouldResetSearchFlagWait (Tbot *bot) {
//   pthread_mutex_lock (&(bot->should_reset_search_mutex));
//   bool res = bot->should_reset_search_flag;
//   pthread_mutex_unlock (&(bot->should_reset_search_mutex));
//   return res;
// }
// void setShouldResetSearchFlagWait (Tbot *bot, bool new_flag) {
//   pthread_mutex_lock (&(bot->should_reset_search_mutex));
//   bot->should_reset_search_flag = new_flag;
//   pthread_mutex_unlock (&(bot->should_reset_search_mutex));
// }

bool getShouldEndBotFlag (Tbot *bot) {
  return bot->should_end_bot_flag;
}
void setShouldEndBotFlag (Tbot *bot, bool new_flag) {
  bot->should_end_bot_flag = new_flag;
}
// bool getShouldEndBotFlagWait (Tbot *bot) {
//   pthread_mutex_lock (&(bot->should_end_bot_mutex));
//   bool res = bot->should_end_bot_flag;
//   pthread_mutex_unlock (&(bot->should_end_bot_mutex));
//   return res;
// }
// void setShouldEndBotFlagWait (Tbot *bot, bool new_flag) {
//   pthread_mutex_lock (&(bot->should_end_bot_mutex));
//   bot->should_end_bot_flag = new_flag;
//   pthread_mutex_unlock (&(bot->should_end_bot_mutex));
// }

bool getNewPiecesReadyFlag (Tbot *bot) {
  return bot->new_pieces_ready_flag;
}
void setNewPiecesReadyFlag (Tbot *bot, bool new_flag) {
  bot->new_pieces_ready_flag = new_flag;
}

Tbyte depth = 0;
#ifdef LOG_BOT_THINKING
// Debug functions
int explored_nodes = 0;
void logBestNode (FILE *logfile, Tnode *node) {
  fprintf(logfile, "Chosen node had a score of : %12.2f\n", getNodeAccumulatedBoardValue (node));
  fflush (logfile);
}
void logVarious (FILE *logfile) {
  fprintf(logfile, "Thinking depth : %d\n", depth);
  fprintf(logfile, "Explored nodes : %d\n", explored_nodes);
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
static float computeBumpiness (Tcoordinate *column_heights) {

  int bumpiness = 0;
  int diff;
  for (Tcoordinate i = 1; i < BOT_MATRIX_WIDTH-2; i++) {
    diff = column_heights [i] - column_heights[i+1];
    bumpiness += ABS(diff);
  }
  // Special cases for the edges of the board
  diff = column_heights [0] - column_heights[1];
  bumpiness += 2*ABS(diff);
  diff = column_heights [BOT_MATRIX_WIDTH-2] - column_heights[BOT_MATRIX_WIDTH-1];
  bumpiness += 2*ABS(diff);

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

  float max_height_score = computeMaxHeight (heights) - 6;
  float avrg_height_score = computeAvrgHeight (heights) - 6;
  float lines_score;
  Tline_clear lines_cleared = getNodeLinesCleared(node);

  switch (lines_cleared.nb_of_lines) {
    case 0: lines_score = 0; break;
    case 1: lines_score = 0;   break;
    case 2: lines_score = 0;  break;
    case 3: lines_score = 0;  break;
    case 4: lines_score = 500; break;
    default: lines_score = 0.0; break;
  }

  float score = 0.0;
  score += lines_score;
  score += -1*ABS(max_height_score);
  score += -7*ABS(avrg_height_score);
  score += -1*computeBumpiness (heights);
  score += -100*computeNumberOfHoles (board, heights);

  setNodeBoardValue (node, score);
  setNodeAccumulatedBoardValue (node, score);

  #ifdef LOG_BOT_THINKING
  explored_nodes++;
  #endif
}
static void accumulateScoreIntoParents (Tnode *highest_parent, Tnode *node, float score) {
  Tnode *tmp_node = node;
  Tbyte link_level = 0;

  while (tmp_node != highest_parent) {
    tmp_node = getNodeImmediateParent (tmp_node);
    setNodeAccumulatedBoardValue (tmp_node, getNodeAccumulatedBoardValue (tmp_node) + score * accumulation_weights[link_level]);
    link_level++;
  }
}
static Tnode *addNode(Tnode *parent, Tbot_movement *moves, Tbyte nb_of_moves, Tnext_queue *next_queue) {
  // Computes the board state of the given seqeunce of movements, computes its score, and adds it as a node
  // Returns a pointer to the created node

  // Create node with the moves applied
  Tbot_board new_board;
  copyBotBoard (&new_board, getNodeBotBoard (parent));
  botPopTetriminoFromQueue (&new_board, next_queue);
  for (Tbyte j = 0; j < nb_of_moves; j++) {
    botApplyInput (&new_board, next_queue, moves[j]);
  }
  botLockActiveTetrimino (&new_board);
  Tline_clear lines = createLineClear (botClearLines (&new_board), NORMAL);
  Tnode *new_node = createNode (new_board, nb_of_moves, moves, parent);
  setNodeLinesCleared (new_node, lines);

  // Add new node to the search tree, compute score, and update parent data
  setNodeIthChild (parent, getNodeNbOfChildren (parent), new_node);
  setNodeNbOfChildren (parent, getNodeNbOfChildren (parent)+1);
  evaluateNode (new_node);

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
static Tnode *generateMoves (Tnode *parent, Tbot_board* board_state, Tnext_queue *next_queue, Tnode_queue *processing_queue, bool should_insert_hold_move) {
  // Generates all the possible moves form the given board state and adds them to the queue
  // Returns the best node found
  // Current implementation : Dijkstra

  // Possible optimizations :
  // - Reduce matrix height because you can't actually pathfind up

  float best_score = -1.0/0.0; // Init to -infty
  Tnode *best_final_node;

  // Move set that the pathfinder can use & durantion estimates for them
  Tbyte move_set_size = 5;
  Tbot_movement move_set[] = {BOT_MV_LEFT, BOT_MV_RIGHT, BOT_MV_CW, BOT_MV_CCW, BOT_MV_SD};
  Tbyte move_distances[] = {2, 2, 2, 2, 2};

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
      TMoveNode* potential_new_neighbour = createMoveNode (move_set[i], &(current->tetrimino), current->dist + move_distances[i], current);
      Ttetrimino* new_tetrimino = &(potential_new_neighbour->tetrimino);
      switch (move_set[i]) {
        case BOT_MV_LEFT:
          moveTetriminoLeft (new_tetrimino);
        break;
        case BOT_MV_RIGHT:
          moveTetriminoRight (new_tetrimino);
        break;
        case BOT_MV_CW:
          moveTetriminoCW (new_tetrimino);
        break;
        case BOT_MV_CCW:
          moveTetriminoCCW (new_tetrimino);
        break;
        case BOT_MV_SD:
          moveTetriminoDown (new_tetrimino);
        break;
      }
      // If node is not visited & is not an obstacle
      if (isNotObstacle (board_state, new_tetrimino)
          && !visited[getTetriminoX (new_tetrimino)+1][getTetriminoY (new_tetrimino)][ getTetriminoRotationState (new_tetrimino)]) {
        // If node is not known, create it, else compare distances and set the shortest
        TMoveNode* neighbour = known_nodes[getTetriminoX (new_tetrimino)+1][getTetriminoY (new_tetrimino)][ getTetriminoRotationState (new_tetrimino)];
        if (!neighbour) {
          neighbour = potential_new_neighbour;
          known_nodes[getTetriminoX (new_tetrimino)+1][getTetriminoY (new_tetrimino)][ getTetriminoRotationState (new_tetrimino)] = neighbour;
          addMoveNodeToList (neighbour, &unvisited_move_nodes);
        } else {
          if (potential_new_neighbour->dist < neighbour->dist) {
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
      Tbyte nb_of_moves = 0;
      Tbot_movement reverse_moves[BOT_MAX_MOVES];
      TMoveNode* current_path_backtrack = current;

      do {
        reverse_moves[nb_of_moves] = current_path_backtrack->move;
        nb_of_moves++;
      } while((current_path_backtrack = current_path_backtrack->best_parent));

      if (should_insert_hold_move) {
        reverse_moves[nb_of_moves] = BOT_MV_HOLD;
        nb_of_moves++;
      }

      Tbot_movement moves[BOT_MAX_MOVES];
      for (Tbyte j = 0; j < nb_of_moves; j++) {
        moves[j] = reverse_moves[nb_of_moves -j-1];
      }
      moves[nb_of_moves] = BOT_MV_HD;
      nb_of_moves++;

      Tnode *new_node = addNode (parent, moves, nb_of_moves, next_queue);
      addToNodeQueue (processing_queue, new_node);
      if (getNodeBoardValue (new_node) > best_score) {
        best_score = getNodeBoardValue (new_node);
        best_final_node = new_node;
      }
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

  return best_final_node;
}
static Tnode *expandNode (Tbot *bot, Tnode *search_tree_root, Tnext_queue *next_queue, Tnode_queue *processing_queue) {
  // Generate the possible moves from the given node, and assign them a score
  // Return the best generated node
  // Returns NULL if children were already generated

  Tnode *node = getFromNodeQueue (processing_queue);
  // If max previews is reached, don't compute
  if (node == NULL ||
      (depth = getBotBoardNextQueueOffset (getNodeBotBoard (node)) - getBotBoardNextQueueOffset (getNodeBotBoard (search_tree_root))) >= bot->max_previews) {
    // addToNodeQueue (processing_queue, node);
    return NULL;
  }

  if (getNodeAreChildrenGenerated (node)) {
    for (unsigned short i = 0; i < getNodeNbOfChildren (node); i++) {
      addToNodeQueue (processing_queue, getNodeIthChild (node, i));
    }
    return NULL;
  }
  setNodeAreChildrenGenerated (node, true);

  Tbot_board tmp_board;
  copyBotBoard (&tmp_board, getNodeBotBoard (node));
  botPopTetriminoFromQueue (&tmp_board, next_queue);
  Tnode *best_node = generateMoves (node, &tmp_board, next_queue, processing_queue, false);
  botApplyInput (&tmp_board, next_queue, BOT_MV_HOLD);
  Tnode *best_node2 =  generateMoves (node, &tmp_board, next_queue, processing_queue, true);
  best_node = (best_node>best_node2)?(best_node):(best_node2);
  accumulateScoreIntoParents (search_tree_root, best_node, getNodeBoardValue (best_node));

  return best_node;
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
  Tnode_queue processing_queue = createNodeQueue ();
  addToNodeQueue (&processing_queue, search_tree);

  #ifdef LOG_BOT_THINKING
  // Setting up the log file for debugging
  FILE *logfile = fopen (LOGFILE, "w");
  #endif

  while (!getShouldEndBotFlag (bot)) {
    // Check all the flags
    if (getShouldOutputPieceFlag (bot) && getNodeAreChildrenGenerated (search_tree)  && !getNewPiecesReadyFlag (bot)) {
      // Get the best immediate child
      Tnode *best_child;
      float best_score = -1.0/0.0;
      for (Tbyte i = 0; i < getNodeNbOfChildren (search_tree); i++) {
        Tnode *best_child_candidate = getNodeIthChild (search_tree, i);
        float score_candidate = getNodeAccumulatedBoardValue (best_child_candidate)/getNodeNbOfAccumulations (best_child_candidate);
        if (score_candidate > best_score) {
          best_score = score_candidate;
          best_child = best_child_candidate;
        }
      }

      #ifdef LOG_BOT_THINKING
      logBestNode (logfile, best_child);
      logVarious (logfile);
      explored_nodes = 0;
      #endif

      // Output the moves of the next piece of the best path found
      bot->next_moves_length = best_child->nb_of_moves;
      translate_moves ((Tbot_movement*) &(best_child->moves), best_child->nb_of_moves, bot->next_moves, &bot->next_moves_length);
      // Signal that the next piece is ready.
      setOutputPieceReadyFlag (bot, true);
      // Reset the flag
      setShouldOutputPieceFlag (bot, false);
      // Advance to think on the next board state
      setNodeIthChild (search_tree, getNodeChildID (best_child), NULL);
      freeNode (search_tree);
      search_tree = best_child;
      advanceNextQueue (&global_next_queue);
      reduceNextPieceOffset (search_tree);
      // Resetting the queue
      freeNodeQueue (&processing_queue);
      processing_queue = createNodeQueue ();
      addToNodeQueue (&processing_queue, search_tree);
    }
    if (getShouldResetSearchFlag (bot)) {
      // Free the current search tree
      freeNode (search_tree);
      // Get the new values
      search_tree = createNode (convertBoardToBotBoard (&bot->master_board), 0, NULL, NULL);
      copyNextQueue (&global_next_queue, getBoardNextQueue (&bot->master_board));
      // Reset the processing queue
      freeNodeQueue (&processing_queue);
      processing_queue = createNodeQueue ();
      addToNodeQueue (&processing_queue, search_tree);
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
    if (expandNode (bot, search_tree, &global_next_queue, &processing_queue) == NULL) {
      usleep (1000);
    }
    // Generate the moves, compute their value, and put them in the queue
  }

  #ifdef LOG_BOT_THINKING
  // Setting up the log file for debugging
  fclose (logfile);
  #endif

  // Free the memory
  freeNodeQueue (&processing_queue);
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
