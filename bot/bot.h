#ifndef __BOT_H__
#define __BOT_H__

#include "../tetris_engine/tetris_engine.h"
#include "../interface/type_interface.h"
#include "move_queue.h"
#include "tree.h"
#include <pthread.h>
#include <stdbool.h>
#include "bot_parameters.h"

typedef struct {
  // General purpose fields
  pthread_t thread_id;

  // Fields for the tetris thinking
  volatile bool should_output_piece_flag;
  volatile bool output_piece_ready_flag;
  volatile bool should_reset_search_flag;
  volatile bool should_end_bot_flag;
  volatile bool new_pieces_ready_flag;

  Tmovement next_moves[MAX_MOVES];
  Tbyte next_moves_length;

  Tnext_queue new_pieces;

  Tboard master_board;
  Tbyte max_previews;
  
  float weights[WEIGHT_COUNT];
} Tbot;

// Interact with the bot
Tinterface_in getBotInterface ();

#endif /* end of include guard: __BOT_H__ */
