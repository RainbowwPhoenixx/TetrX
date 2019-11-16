#include "type_mino.h"

Tmino createMino (Tcoordinate_diff x, Tcoordinate_diff y) {
  Tmino mino = {.x=x, .y=y};
  return mino;
}
