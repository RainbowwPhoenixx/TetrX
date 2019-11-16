#include "type_tetrimino.h"

Tshape getTetriminoShape (Ttetrimino *t) {
  return t->shape;
}
void setTetriminoShape (Ttetrimino *t, Tshape new_shape) {
  t->shape = new_shape;
}

Tmino *getIthMino (Ttetrimino *t, int i) {
  return &(t->minos[i]);
}
void setIthMino (Ttetrimino *t, int i, Tmino new_mino) {
  t->minos[i] = new_mino;
}

Tcoordinate getTetriminoX (Ttetrimino *t) {
  return t->x;
}
void setTetriminoX (Ttetrimino *t, Tcoordinate new_x) {
  t->x = new_x;
}

Tcoordinate getTetriminoY (Ttetrimino *t) {
  return t->y;
}
void setTetriminoY (Ttetrimino *t, Tcoordinate new_y) {
  t->y = new_y;
}
