#ifndef __MOVE_QUEUE_H__
#define __MOVE_QUEUE_H__

#include "../tetris_engine/tetris_engine.h"
#define MAX_MOVES 30

typedef struct {
  Tbyte head, tail, size;
  Tmovement moves[MAX_MOVES];
} Tmove_queue;

Tmove_queue createMoveQueue ();
Tbyte getQueueSize (Tmove_queue *queue);
Tmovement popMoveFromQueue (Tmove_queue *queue);
void addMovementToQueue (Tmove_queue *queue, Tmovement move);

#endif /* end of include guard: __MOVE_QUEUE_H__ */
