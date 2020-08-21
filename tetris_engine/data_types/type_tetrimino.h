#ifndef __TYPE__TETRIMINO_H__
#define __TYPE__TETRIMINO_H__

#include "general_library.h"
#include "type_shape.h"
#include "type_mino.h"

#define NUMBER_OF_MINOS 4

typedef enum {R0=0, R90=1, R180=2, R270=3} Trotation_state;

typedef struct {
  Tshape shape;                   // Shape of the tetrimino
  Tmino *minos;   // Components of the tetrimino
  Tcoordinate x, y;               // Position of the tetrimino on the matrix
  Trotation_state rotation_state; // Stores the current rotation state
} Ttetrimino;

// Accessor prototypes
Tshape getTetriminoShape (Ttetrimino *t);
void setTetriminoShape (Ttetrimino *t, Tshape new_shape);

Tmino *getIthMino (Ttetrimino *t, int i);
// You can only set the minos as a predefined list defined at the
// top of type_tetrimino.c
void setMinos (Ttetrimino *t, Tmino *minos);

Tcoordinate getTetriminoX (Ttetrimino *t);
void setTetriminoX (Ttetrimino *t, Tcoordinate new_x);

Tcoordinate getTetriminoY (Ttetrimino *t);
void setTetriminoY (Ttetrimino *t, Tcoordinate new_y);

Trotation_state getTetriminoRotationState (Ttetrimino *t);

// Constructor
Ttetrimino createTetrimino (Tshape tetrimino_shape);

// Useful
// Translation
void moveTetriminoRight (Ttetrimino *t);
void moveTetriminoLeft (Ttetrimino *t);
void moveTetriminoUp (Ttetrimino *t);
void moveTetriminoDown (Ttetrimino *t);

// Rotation
void moveTetriminoCW (Ttetrimino *t);
void moveTetriminoCCW (Ttetrimino *t);

// Copying
void copyTetrimino (Ttetrimino *dest, Ttetrimino *src);

#endif
