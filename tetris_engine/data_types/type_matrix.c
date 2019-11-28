#include "type_matrix.h"

// Accessors
Tshape getMatrixShapeAtPos (Tmatrix *matrix, Tcoordinate x, Tcoordinate y) {
  return matrix->mat[x][y];
}
void setMatrixShapeAtPos (Tmatrix *matrix, Tcoordinate x, Tcoordinate y, Tshape new_shape) {
  matrix->mat[x][y] = new_shape;
}

// Constructor
Tmatrix createMatrix () {
  Tmatrix matrix;

  for (Tcoordinate i = 0; i < C_MATRIX_WIDTH; i++) {
    for (Tcoordinate j = 0; j < C_MATRIX_HEIGHT; j++) {
      setMatrixShapeAtPos (&matrix, i, j, EMPTY);
    }
  }

  return matrix;
}

// Useful functions
bool isMinoAtPosEmpty (Tmatrix *matrix, Tcoordinate x, Tcoordinate y) {
  return (getMatrixShapeAtPos(matrix, x, y) == EMPTY);
}
