#include "type_next_queue.h"

// Accessors
Tshape getIthNextPiece(Tnext_queue *next_queue, Tbyte i) {
  return next_queue->queue[i];
}
void setIthNextPiece(Tnext_queue *next_queue, Tbyte i, Tshape new_shape) {
  next_queue->queue[i] = new_shape;
}

Tbyte getNextQueueLength (Tnext_queue *next_queue) {
  return next_queue->queue_length;
}
void setNextQueueLength (Tnext_queue *next_queue, Tbyte new_length) {
  next_queue->queue_length = new_length;
}

// Useful functions
void advanceNextQueue (Tnext_queue *next_queue) {
  // Advances the next queue by one step
  // !! Erases the first element of the queue !!

  for (Tbyte i = 0; i < getNextQueueLength (next_queue)-1; i++) {
    setIthNextPiece (next_queue, i, getIthNextPiece (next_queue, i+1));
  }

  setNextQueueLength (next_queue, getNextQueueLength (next_queue)-1);
}

static void addPieceToNextQueue (Tnext_queue *next_queue, Tshape piece) {
  // Set the piece
  setIthNextPiece (next_queue, getNextQueueLength(next_queue), piece);
  // Up the queue length
  setNextQueueLength (next_queue, getNextQueueLength (next_queue)+1);
}
void addBagToNextQueue (Tnext_queue *next_queue) {
  // Adds a 7 bag to the end of the queue
  Tshape bag[] = {L, J, S, Z, O, I, T};

  for (Tbyte i = 7; i > 0; i--) {
    // Get a random piece form the list
    Tbyte choice = getRandomInteger (i);
    // Put it into the queue
    addPieceToNextQueue (next_queue, bag[choice]);
    // Take it out of the array
    for (Tbyte j = choice; j < i-1; j++) {
      bag[j] = bag[j+1];
    }
  }
}

// Constructor
Tnext_queue createNextQueue () {
  Tnext_queue next_queue;

  // init the queue length
  setNextQueueLength (&next_queue, 0);

  // Add two bags to the queue
  addBagToNextQueue (&next_queue);
  addBagToNextQueue (&next_queue);

  return next_queue;
}

// Copy
void copyNextQueue (Tnext_queue *dest, Tnext_queue *src) {
  setNextQueueLength (dest, getNextQueueLength (src));

  for (Tbyte i = 0; i < getNextQueueLength (dest); i++) {
    setIthNextPiece (dest, i, getIthNextPiece (src, i));
  }
}
