#include "type_shape.h"

Tshape getRandomShape () {
  // Returns a random tetrimino shape
  return (Tshape) getRandomInteger(7);
}
