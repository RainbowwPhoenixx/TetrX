#ifndef __NODE_QUEUE_H__
#define __NODE_QUEUE_H__

#include "tree.h"

// Linked list that will act as a queue
struct _Tnode_queue_element {
  struct _Tnode_queue_element* next;
  Tnode *element;
};
typedef struct _Tnode_queue_element* Tnode_queue_element;
typedef struct {
  Tnode_queue_element head, tail;
  unsigned long size;
} Tnode_queue;

void addToNodeQueue (Tnode_queue *queue, Tnode *node); // Adds the given node to the queue
void addToNodeQueueBeginning (Tnode_queue *queue, Tnode *node);
Tnode *getFromNodeQueue (Tnode_queue *queue); // Pop the next element off the queue

Tnode_queue createNodeQueue ();
void freeNodeQueue (Tnode_queue queue);

#endif /* end of include guard: __NODE_QUEUE_H__ */
