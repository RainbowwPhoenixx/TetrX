#ifndef __TYPE_NEXT_QUEUE_H__
#define __TYPE_NEXT_QUEUE_H__

#include "general_library.h"
#include "type_shape.h"
#include <stdbool.h>

typedef struct {
  Tshape queue[C_QUEUE_LENGTH];
  Tbyte queue_length;
} Tnext_queue;

// Accessors
Tshape getIthNextPiece(Tnext_queue *next_queue, Tbyte i);
void setIthNextPiece(Tnext_queue *next_queue, Tbyte i, Tshape new_shape);

Tbyte getNextQueueLength (Tnext_queue *next_queue);
void setNextQueueLength (Tnext_queue *next_queue, Tbyte new_length);

// Useful functions
void advanceNextQueue (Tnext_queue *next_queue);
void addPieceToNextQueue (Tnext_queue *next_queue, Tshape piece);
void addBagToNextQueue (Tnext_queue *next_queue, bool should_signal_new_pieces);

// Constructor
Tnext_queue createNextQueue ();

// Copy
void copyNextQueue (Tnext_queue *dest, Tnext_queue *src);

#endif
