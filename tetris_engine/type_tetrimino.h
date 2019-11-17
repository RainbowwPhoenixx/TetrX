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

// Accessor prototypes
Tshape getTetriminoShape (Ttetrimino *t);
void setTetriminoShape (Ttetrimino *t, Tshape new_shape);
Tmino *getIthMino (Ttetrimino *t, int i);
void setIthMino (Ttetrimino *t, int i, Tmino new_mino);
Tcoordinate getTetriminoX (Ttetrimino *t);
void setTetriminoX (Ttetrimino *t, Tcoordinate new_x);
Tcoordinate getTetriminoY (Ttetrimino *t);
void setTetriminoY (Ttetrimino *t, Tcoordinate new_y);

#endif
