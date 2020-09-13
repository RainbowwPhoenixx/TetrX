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

  // Init the matrix with empty minos
  for (Tcoordinate i = 0; i < C_MATRIX_WIDTH; i++) {
    for (Tcoordinate j = 0; j < C_MATRIX_HEIGHT; j++) {
      setMatrixShapeAtPos (&matrix, i, j, EMPTY);
    }
  }

  return matrix;
}

// Useful functions
bool isMinoAtPosEmpty (Tmatrix *matrix, Tcoordinate x, Tcoordinate y) {
  // Returns true if the matrix does not contain a tetrimino at the specified coordinates
  return ((x >= 0) && (x < C_MATRIX_WIDTH) && (y >= 0) && (getMatrixShapeAtPos(matrix, x, y) == EMPTY));
}

void copyMatrix (Tmatrix *dest, Tmatrix *src) {
  // Copies the src matrix into dest
  for (Tcoordinate i = 0; i < C_MATRIX_WIDTH; i++) {
    for (Tcoordinate j = 0; j < C_MATRIX_HEIGHT; j++) {
      setMatrixShapeAtPos (dest, i, j, getMatrixShapeAtPos (src, i, j));
    }
  }
}
