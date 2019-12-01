#ifndef __TYPE_MINO_H__
#define __TYPE_MINO_H__

#include "type_shape.h"
#include "general_library.h"

// Type creation
typedef struct {
  Tcoordinate_diff x, y; // Stores the difference to the center of the tetrimino
} Tmino;

// Prototypes
Tcoordinate_diff getMinoXDiff (Tmino *mino);
void setMinoXDiff (Tmino *mino, Tcoordinate_diff new_x);
Tcoordinate_diff getMinoYDiff (Tmino *mino);
void setMinoYDiff (Tmino *mino, Tcoordinate_diff new_y);

// Useful
Tmino createMino (Tcoordinate_diff x, Tcoordinate_diff y);
void copyMino (Tmino *dest, Tmino *src);

#endif
