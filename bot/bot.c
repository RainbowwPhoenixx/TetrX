#include "bot.h"
#include "move_queue.h"
#include <unistd.h>

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


static float computeAvrgHeight (Tbot_board *board) {
  int height_sum = 0;

  for (Tcoordinate i = 0; i < C_MATRIX_WIDTH; i++) {
    Tcoordinate j = C_MATRIX_HEIGHT-1;
    while (j > -1 && isMinoAtPosEmpty(getBotBoardMatrix (board), i, j)) {
      j--;
    }
    height_sum += j+1;
  }

  return ((float) height_sum)/C_MATRIX_WIDTH;
}
static float computeMaxHeight (Tbot_board *board) {
  int max_height = 0;

  for (Tcoordinate i = 0; i < C_MATRIX_WIDTH; i++) {
    Tcoordinate j = C_MATRIX_HEIGHT-1;
    while (j > -1 && isMinoAtPosEmpty(getBotBoardMatrix (board), i, j)) {
      j--;
    }
    max_height = (j+1>max_height)?(j+1):(max_height);
  }

  return (float) max_height;
}
static float evaluateBoard (Tbot_board *board) {
  return 1/(computeMaxHeight (board) + computeAvrgHeight (board));
}
static Tnode *expandNode (Tbot *bot, Tnode *node, Tnext_queue *next_queue) {
  // Generate the possible moves from the given node, and assign them a score
  // Return the best generated node
  // Returns NULL if children were already generated

  if (getNodeAreChildrenGenerated (node)) {
    return NULL;
  }
  setNodeAreChildrenGenerated (node, true);

  float best_score = -1.0/0.0; // Init to -infty
  Tnode *best_node;
  Tmovement moves[15];
  Tbyte nb_of_moves = 0;

  for (Tbyte rot = 0; rot < 4; rot++) {
    for (int i = -4; i < 6; i++) {
      nb_of_moves = 0;
      for (size_t j = 0; j < rot; j++) {
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
      botClearLines (&new_board);

      Tnode *new_node = createNode (new_board, nb_of_moves, moves);
      setNodeIthChild (node, getNodeNbOfChildren (node), new_node);
      setNodeNbOfChildren (node, getNodeNbOfChildren (node)+1);
      float board_score = evaluateBoard (&new_board);
      setNodeBoardValue (new_node, board_score);
      if (board_score > best_score) {
        best_score = board_score;
        best_node = new_node;
      }
    }
  }

  return best_node;
}

// Thinking function
static void *bot_TetrX (void *_bot) {
  Tbot *bot = (Tbot*) _bot;
  Tnode *search_tree = createNode (convertBoardToBotBoard (&bot->master_board), 0, NULL);
  Tnext_queue global_next_queue = createNextQueue ();
  copyNextQueue (&global_next_queue, getBoardNextQueue (&bot->master_board));
  Tnode *best_node = NULL;

  while (!getShouldEndBotFlag (bot)) {
    // Check all the flags
    if (getShouldOutputPieceFlag (bot) && best_node  && !getNewPiecesReadyFlag (bot)) {
      // Output the moves of the next piece of the best path found
      for (Tbyte i = 0; i < best_node->nb_of_moves; i++) {
        bot->next_moves[i] = best_node->moves[i];
      }
      bot->next_moves_length = best_node->nb_of_moves;
      // Signal that the next piece is ready.
      setOutputPieceReadyFlag (bot, true);
      // Reset the flag
      setShouldOutputPieceFlag (bot, false);
      // Advance to think on the next board state
      setNodeIthChild (search_tree, getNodeChildID (best_node), NULL);
      freeNode (search_tree);
      search_tree = best_node;
      best_node = NULL;
      advanceNextQueue (&global_next_queue);
      setBotBoardNextQueueOffset (getNodeBotBoard (search_tree), getBotBoardNextQueueOffset (getNodeBotBoard (search_tree))-1);
    }
    if (getShouldResetSearchFlag (bot)) {
      // Free the current search tree
      // Reset the queue
      // Regenerate the first node
      // Reset the flag
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
    Tnode *best_node_candidate = expandNode (bot, search_tree, &global_next_queue);
    if (best_node_candidate != NULL) {
      best_node = best_node_candidate;
    } else {
      usleep (1000);
    }

    // Generate the moves, compute their value, and put them in the queue
  }

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
    // if (!getShouldOutputPieceFlag (bot)) {
      // setShouldOutputPieceFlag (bot, true);
    // }

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
