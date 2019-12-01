#ifndef __TYPE_MATRIX_H__
#define __TYPE_MATRIX_H__

#include <stdbool.h>

#include "general_library.h"
#include "type_shape.h"

typedef struct {
  Tshape mat[C_MATRIX_WIDTH][C_MATRIX_HEIGHT];
} Tmatrix;

// Accessor protoypes
Tshape getMatrixShapeAtPos (Tmatrix *matrix, Tcoordinate x, Tcoordinate y);
void setMatrixShapeAtPos (Tmatrix *matrix, Tcoordinate x, Tcoordinate y, Tshape new_shape);

// Constructor protoype
Tmatrix createMatrix ();

// Useful functions protoypes
bool isMinoAtPosEmpty (Tmatrix *matrix, Tcoordinate x, Tcoordinate y);

#endif
