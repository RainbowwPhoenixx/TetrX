#ifndef __MOVE_QUEUE_H__
#define __MOVE_QUEUE_H__

#include "../tetris_engine/tetris_engine.h"
#include "bot_parameters.h"
#include "bot_types.h"

typedef struct {
  Tbyte head, tail, size;
  Tbot_movement moves[MAX_MOVES];
} Tmove_queue;

Tmove_queue createMoveQueue ();
Tbyte getQueueSize (Tmove_queue *queue);
Tbot_movement popMoveFromQueue (Tmove_queue *queue);
void addMovementToQueue (Tmove_queue *queue, Tbot_movement move);

#endif /* end of include guard: __MOVE_QUEUE_H__ */
