#include "type_movement.h"

Tmovement createMovementWord () {
  return 0;
}

void addMovementToWord (Tmovement *mv_word, Tpossible_movement mv) {
  (*mv_word) = (*mv_word) | mv;
}
bool isMovementInWord (Tmovement *mv_word, Tpossible_movement mv) {
  return (*mv_word & mv);
}
