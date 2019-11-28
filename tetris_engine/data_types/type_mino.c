#include "type_mino.h"

Tcoordinate_diff getMinoXDiff (Tmino *mino) {
  return mino->x;
}
void setMinoXDiff (Tmino *mino, Tcoordinate_diff new_x) {
  mino->x = new_x;
}

Tcoordinate_diff getMinoYDiff (Tmino *mino) {
  return mino->y;
}
void setMinoYDiff (Tmino *mino, Tcoordinate_diff new_y) {
  mino->y = new_y;
}

Tmino createMino (Tcoordinate_diff x, Tcoordinate_diff y) {
  Tmino mino;
  setMinoXDiff (&mino, x);
  setMinoYDiff (&mino, y);
  return mino;
}
