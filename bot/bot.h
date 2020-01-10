#ifndef __BOT_H__
#define __BOT_H__

#include "../tetris_engine/tetris_engine.h"
#include "../interface/type_interface.h"
#include "move_queue.h"
#include "tree.h"
#include <pthread.h>
#include <stdbool.h>

typedef struct {
  // General purpose fields
  pthread_t thread_id;

  // Fields for the tetris thinking
  volatile bool should_output_piece_flag;
  volatile bool output_piece_ready_flag;
  volatile bool should_reset_search_flag;
  volatile bool should_end_bot_flag;
  volatile bool new_pieces_ready_flag;

  // Mutexes to protect the above flags
  // pthread_mutex_t should_output_piece_mutex;
  // pthread_mutex_t output_piece_ready_mutex;
  // pthread_mutex_t should_reset_search_mutex;
  // pthread_mutex_t should_end_bot_mutex;

  Tmovement next_moves[MAX_MOVES];
  Tbyte next_moves_length;

  Tnext_queue new_pieces;

  Tboard master_board;
  Tbyte max_previews;

  // Best path found
} Tbot;

// Interact with the bot
Tinterface_in getBotInterface ();

#endif /* end of include guard: __BOT_H__ */
