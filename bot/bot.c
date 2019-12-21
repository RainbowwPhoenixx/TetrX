#include "bot.h"
#include <unistd.h>

// typedef struct {
//
// } Tpiece_placement_list;

// Accessors
bool getShouldOutputPieceFlagTry (Tbot *bot) {
  // Returns true if the should_output_piece_flag is set to true.
  // Returns false if the should_output_piece_flag is set to false.
  // Returns false is a lock could not be acquired for the flag
  // (So that the bot can keep thinking until the lock becomes available)
  if (pthread_mutex_lock (&(bot->should_output_piece_mutex)) == 0) {
    bool res = bot->should_output_piece_flag;
    pthread_mutex_unlock (&(bot->should_output_piece_mutex));
    return res;
  } else return false;
}
bool getShouldOutputPieceFlagWait (Tbot *bot) {
  pthread_mutex_lock (&(bot->should_output_piece_mutex));
  bool res = bot->should_output_piece_flag;
  pthread_mutex_unlock (&(bot->should_output_piece_mutex));
  return res;
}
void setShouldOutputPieceFlagWait (Tbot *bot, bool new_flag) {
  pthread_mutex_lock (&(bot->should_output_piece_mutex));
  bot->should_output_piece_flag = new_flag;
  pthread_mutex_unlock (&(bot->should_output_piece_mutex));
}

bool getOutPutPieceReadyFlagTry (Tbot *bot) {
  if (pthread_mutex_lock (&(bot->output_piece_ready_mutex)) == 0) {
    bool res = bot->output_piece_ready_flag;
    pthread_mutex_unlock (&(bot->output_piece_ready_mutex));
    return res;
  } else return false;
}
bool getOutPutPieceReadyFlagWait (Tbot *bot) {
  pthread_mutex_lock (&(bot->output_piece_ready_mutex));
  bool res = bot->output_piece_ready_flag;
  pthread_mutex_unlock (&(bot->output_piece_ready_mutex));
  return res;
}
void setOutputPieceFlagReadyWait (Tbot *bot, bool new_flag) {
  pthread_mutex_lock (&(bot->output_piece_ready_mutex));
  bot->output_piece_ready_flag = new_flag;
  pthread_mutex_unlock (&(bot->output_piece_ready_mutex));
}

bool getShouldResetSearchFlagWait (Tbot *bot) {
  pthread_mutex_lock (&(bot->should_reset_search_mutex));
  bool res = bot->should_reset_search_flag;
  pthread_mutex_unlock (&(bot->should_reset_search_mutex));
  return res;
}
void setShouldResetSearchFlagWait (Tbot *bot, bool new_flag) {
  pthread_mutex_lock (&(bot->should_reset_search_mutex));
  bot->should_reset_search_flag = new_flag;
  pthread_mutex_unlock (&(bot->should_reset_search_mutex));
}

bool getShouldEndBotFlagWait (Tbot *bot) {
  pthread_mutex_lock (&(bot->should_end_bot_mutex));
  bool res = bot->should_end_bot_flag;
  pthread_mutex_unlock (&(bot->should_end_bot_mutex));
  return res;
}
void setShouldEndBotFlagWait (Tbot *bot, bool new_flag) {
  pthread_mutex_lock (&(bot->should_end_bot_mutex));
  bot->should_end_bot_flag = new_flag;
  pthread_mutex_unlock (&(bot->should_end_bot_mutex));
}


// Thinking function
static void *bot_TetrX (void *_bot) {
  Tbot *bot = (void*) _bot;
  // Tnode *search_tree = createNode (bot->master_board, 0, NULL);
  // Tnode *queue[500];
  // unsigned short queue_length = 0;

  Tmovement hard_drop = createMovementWord();
  addMovementToWord (&hard_drop, MV_HD);

  while (!getShouldEndBotFlagWait (bot)) {
    // Check all the flags
    if (getShouldOutputPieceFlagTry (bot)) {
      // Output the moves of the next piece of the best path found
      bot->next_moves[0] = hard_drop;
      bot->next_moves_length = 1;
      // Signal that the next piece is ready.
      setOutputPieceFlagReadyWait (bot, true);
      // Reset the flag
      setShouldOutputPieceFlagWait (bot, false);
    }
    if (getShouldResetSearchFlagWait (bot)) {
      // Free the current search tree
      // Reset the queue
      // Regenerate the first node
      // Reset the flag
    }

    // Do the thinking

    // Generate the moves, compute their value, and put them in the queue
  }

  // Free the memory
  // freeNode (search_tree);

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

  // Init the mutexes
  pthread_mutex_init (&(bot->should_output_piece_mutex), NULL);
  pthread_mutex_init (&(bot->output_piece_ready_mutex), NULL);
  pthread_mutex_init (&(bot->should_reset_search_mutex), NULL);
  pthread_mutex_init (&(bot->should_end_bot_mutex), NULL);

  // Create the bot thread
  pthread_create (&(bot->thread_id), PTHREAD_CREATE_JOINABLE, bot_TetrX, (void*) bot);
}
static Tmovement getBotMovement (Tbot *bot) {
  static Tmovement next_moves[15];
  static Tbyte next_moves_length = 0;

  if (next_moves_length == 0) {
    // Ask the bot to prepare the input
    if (!getShouldOutputPieceFlagWait (bot)) {
      setShouldOutputPieceFlagWait (bot, true);
    }

    // When input is prepared, get it & reset flag
    if (getOutPutPieceReadyFlagTry(bot)) {
      for (Tbyte i = 0; i < bot->next_moves_length; i++) {
        next_moves[i] = bot->next_moves[i];
      }
      next_moves_length = bot->next_moves_length;
      bot->next_moves_length = 0;
      setOutputPieceFlagReadyWait (bot, false);
    }
  }

  // next_moves acts as a stack
  Tmovement res;
  if (next_moves_length == 0) {
    res = createMovementWord ();
  } else {
    next_moves_length--;
    res = next_moves[next_moves_length];
  }
  return res;
}
static void endBot (Tbot *bot) {
  void *status;
  bot->should_end_bot_flag = true;
  pthread_join (bot->thread_id, &status);
}
Tinterface_in getBotInterface () {
  Tinterface_in bot_IO;
  bot_IO.initInputFunc = startBot;
  bot_IO.getInputFunc = getBotMovement;
  bot_IO.endInputFunc = endBot;

  return bot_IO;
}

// static Tpiece_placement_list getMoveList () {
//
// }

static float computeAvrgHeight (Tboard *board) {
  int height_sum = 0;

  for (Tcoordinate i = 0; i < C_MATRIX_WIDTH; i++) {
    Tcoordinate j = C_MATRIX_HEIGHT;
    while (j > 0 || !isMinoAtPosEmpty(getBoardMatrix (board), i, j)) {
      j--;
    }
    height_sum += j;
  }

  return ((float) height_sum)/C_MATRIX_WIDTH;
}

static float evaluateBoard (Tboard *board) {
  return computeAvrgHeight (board);
}
