#ifndef __BOT_H__
#define __BOT_H__

#include "../tetris_engine/tetris_engine.h"
#include "../interface/type_interface.h"
#include "tree.h"
#include <pthread.h>
#include <stdbool.h>

typedef struct {
  // General purpose fields
  pthread_t thread_id;

  // Fields for the tetris thinking
  bool should_output_piece_flag,
       output_piece_ready_flag,
       should_reset_search_flag,
       should_end_bot_flag;

  // Mutexes to protect the above flags
  pthread_mutex_t should_output_piece_mutex;
  pthread_mutex_t output_piece_ready_mutex;
  pthread_mutex_t should_reset_search_mutex;
  pthread_mutex_t should_end_bot_mutex;

  Tmovement next_moves[15];
  Tbyte next_moves_length;

  Tboard master_board;
  Tbyte max_previews;
} Tbot;

// Interact with the bot
Tinterface_in getBotInterface ();

#endif /* end of include guard: __BOT_H__ */
