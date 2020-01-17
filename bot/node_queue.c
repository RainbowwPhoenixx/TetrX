#include "node_queue.h"


void addToNodeQueue (Tnode_queue *queue, Tnode *node) {
  // Adds the given node to the queue
  Tnode_queue_element new_element = malloc (sizeof(struct _Tnode_queue_element));
  new_element->element = node;
  new_element->next = NULL;

  if (queue->size == 0) {
    queue->head = new_element;
    queue->tail = new_element;
  } else {
    (queue->tail)->next = new_element;
    queue->tail = new_element;
  }

  queue->size++;
}
void addToNodeQueueBeginning (Tnode_queue *queue, Tnode *node) {
  // Adds the given node to the beginning of the queue
  Tnode_queue_element new_element = malloc (sizeof(struct _Tnode_queue_element));
  new_element->element = node;
  new_element->next = NULL;

  if (queue->size == 0) {
    queue->head = new_element;
    queue->tail = new_element;
  } else {
    new_element->next = queue->head;
    queue->head = new_element;
  }

  queue->size++;
}
Tnode *getFromNodeQueue (Tnode_queue *queue) {
  // Pop the next element off the queue
  // Returns NULL if the queue is empty

  if (queue->size == 0) {
    return NULL;
  }

  Tnode *res = queue->head->element;
  Tnode_queue_element old_head = queue->head;
  queue->head = queue->head->next;
  free (old_head);
  queue->size--;

  return res;
}

Tnode_queue createNodeQueue () {
  Tnode_queue queue;
  queue.head = NULL;
  queue.tail = NULL;
  queue.size = 0;
  return queue;
}
static void freeNodeQueueElements (Tnode_queue_element e) {
  // Recursively frees the queue elements
  if (e->next != NULL) {
    freeNodeQueueElements (e->next);
  }
  free (e);
}
void freeNodeQueue (Tnode_queue *queue) {
  if (queue->size) {
    freeNodeQueueElements (queue->head);
  }
}
