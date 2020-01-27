#include "bot.h"
#include <unistd.h>
#include <stdio.h>

#define BOT_MAX_PREVIEWS 5 // Do not set lower than 1
#define LOG_BOT_THINKING
#ifdef LOG_BOT_THINKING
#define LOGFILE "bot_debug.log"
#endif

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
  for (Tcoordinate i = 0; i < C_MATRIX_WIDTH; i++) {
    Tcoordinate j = C_MATRIX_HEIGHT-1;
    while (j > -1 && isMinoAtPosEmpty(getBotBoardMatrix (board), i, j)) {
      j--;
    }
    column_heights [i] = j+1;
  }
}
static float computeBumpiness (Tcoordinate *column_heights) {

  int bumpiness = 0;
  for (Tcoordinate i = 0; i < C_MATRIX_WIDTH-1; i++) {
    int diff = column_heights [i] - column_heights[i+1];
    bumpiness += (diff>0)?(diff):(-diff);
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
      if (isMinoAtPosEmpty (getBotBoardMatrix (board), i, j)) {
        nb_of_holes++;
      }
    }
  }
  return (float) nb_of_holes;
}
static float evaluateBoard (Tbot_board *board, Tline_clear lines) {
  Tcoordinate heights[C_MATRIX_WIDTH];
  computeHeights (board, heights);

  float score = 0.0;
  score += 10*lines*lines;
  score += -1*computeMaxHeight (heights);
  score += -3*computeAvrgHeight (heights);
  score += -.1*computeBumpiness (heights);
  score += -1000*computeNumberOfHoles (board, heights);

  #ifdef LOG_BOT_THINKING
  explored_nodes++;
  #endif

  return score;
}
static void accumulateScoreIntoParents (Tnode *highest_parent, Tnode *node, float score) {
  Tnode *tmp_node = node;
  Tbyte link_level = 0;
  setNodeAccumulatedBoardValue (tmp_node, getNodeAccumulatedBoardValue (tmp_node) + score * accumulation_weights[0]);

  while (tmp_node != highest_parent) {
    tmp_node = getNodeImmediateParent (tmp_node);
    setNodeAccumulatedBoardValue (tmp_node, getNodeAccumulatedBoardValue (tmp_node) + score * accumulation_weights[link_level]);
    link_level++;
  }
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

  float best_score = -1.0/0.0; // Init to -infty
  Tnode *best_node;
  Tmovement moves[15];
  Tbyte nb_of_moves = 0;

  for (Tbyte hold = 0; hold < 2; hold++) {
    for (Tbyte rot = 0; rot < 4; rot++) {
      for (int i = -4; i < 6; i++) {
        nb_of_moves = 0;
        if (hold) {
          moves[nb_of_moves] = MV_HOLD;
          nb_of_moves++;
        }
        for (Tbyte j = 0; j < rot; j++) {
          moves[nb_of_moves] = MV_CW;
          nb_of_moves++;
        }
        if (i<0) {
          for (Tbyte j = 0; j < -i; j++) {
            moves[nb_of_moves] = MV_LEFT;
            nb_of_moves++;
          }
        } else if (i>0) {
          for (Tbyte j = 0; j < i; j++) {
            moves[nb_of_moves] = MV_RIGHT;
            nb_of_moves++;
          }
        }
        moves[nb_of_moves] = MV_HD;
        nb_of_moves++;

        Tbot_board new_board;
        copyBotBoard (&new_board, getNodeBotBoard (node));
        botPopTetriminoFromQueue (&new_board, next_queue);
        for (Tbyte j = 0; j < nb_of_moves; j++) {
          botApplyInput (&new_board, next_queue, moves[j]);
        }

        botLockActiveTetrimino (&new_board);
        Tline_clear lines = botClearLines (&new_board);

        Tnode *new_node = createNode (new_board, nb_of_moves, moves, node);
        setNodeIthChild (node, getNodeNbOfChildren (node), new_node);
        setNodeNbOfChildren (node, getNodeNbOfChildren (node)+1);
        float board_score = evaluateBoard (&new_board, lines);
        setNodeBoardValue (new_node, board_score);
        accumulateScoreIntoParents (search_tree_root, new_node, board_score);
        addToNodeQueue (processing_queue, new_node);
        if (board_score > best_score) {
          best_score = board_score;
          best_node = new_node;
        }
      }
    }
  }

  return best_node;
}

// Various processing functions
static void translate_moves (Tmovement *moves, Tbyte *nb_of_moves) {
  // Translate moves from the bot's thinking to the game's thinking

  // Copy the moves in a temporary list
  Tmovement tmp_moves[MAX_MOVES];
  Tbyte tmp_nb_of_moves = *nb_of_moves;
  for (Tbyte i = 0; i < tmp_nb_of_moves; i++) {
    tmp_moves[i] = moves [i];
  }

  // Space out side moves (hypertap, not DAS)
  *nb_of_moves = 0;
  for (Tbyte i = 0; i < tmp_nb_of_moves; i++) {
    moves[*nb_of_moves] = tmp_moves [i];
    (*nb_of_moves)++;
    if (isMovementInWord (tmp_moves+i, MV_LEFT) || isMovementInWord (tmp_moves+i, MV_RIGHT) || isMovementInWord (tmp_moves+i, MV_CW) || isMovementInWord (tmp_moves+i, MV_CCW)) {
      moves [*nb_of_moves] = createMovementWord ();
      (*nb_of_moves)++;
    }
  }
}
static Tnode *getRootDirectChildLink (Tnode *root, Tnode *node) {
  // Goes up the parent chain of 'node' until it gets to 'root'
  // Allows to get the immediate next movement from a best child
  // several levels down.

  Tnode *tmp_node = node;

  while (getNodeImmediateParent (tmp_node) != root) {
    tmp_node = getNodeImmediateParent (tmp_node);
  }

  return tmp_node;
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
        if (getNodeAccumulatedBoardValue (best_child_candidate) > best_score) {
          best_score = getNodeAccumulatedBoardValue (best_child_candidate);
          best_child = best_child_candidate;
        }
      }

      #ifdef LOG_BOT_THINKING
      logBestNode (logfile, best_child);
      logVarious (logfile);
      explored_nodes = 0;
      #endif

      // Output the moves of the next piece of the best path found
      for (Tbyte i = 0; i < best_child->nb_of_moves; i++) {
        bot->next_moves[i] = best_child->moves[i];
      }
      bot->next_moves_length = best_child->nb_of_moves;
      translate_moves (bot->next_moves, &bot->next_moves_length);
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
