#include "move_queue.h"

Tmove_queue createMoveQueue () {
  Tmove_queue queue;
  queue.head = 0;
  queue.tail = 0;
  queue.size = 0;

  return queue;
}
Tbyte getQueueSize (Tmove_queue *queue) {
  return queue->size;
}
Tmovement popMoveFromQueue (Tmove_queue *queue) {
  // Returns an empty move if the queue is empty
  if (queue->size == 0) {
    return 0;
  } else {
    queue->size--;
    Tmovement res = queue->moves[queue->head];
    queue->head = (queue->head + 1) % MAX_MOVES;
    return res;
  }
}
void addMovementToQueue (Tmove_queue *queue, Tmovement move) {
  queue->size++;
  queue->tail = (queue->tail + 1) % MAX_MOVES;
  queue->moves[queue->tail] = move;
}
