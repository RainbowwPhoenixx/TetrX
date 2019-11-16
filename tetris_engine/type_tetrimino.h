#ifndef __TYPE__TETRIMINO_H__
#define __TYPE__TETRIMINO_H__

#include "general_library.h"
#include "type_shape.h"
#include "type_mino.h"

#define NUMBER_OF_MINOS 4

typedef struct {
  Tshape shape;                   // Shape of the tetrimino
  Tmino minos[NUMBER_OF_MINOS];   // Components of the tetrimino
  Tcoordinate x, y;               // Position of the tetrimino on the matrix
} Ttetrimino;

#endif
