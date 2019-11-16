#ifndef __TYPE_MINO_H__
#define __TYPE_MINO_H__

#include "type_shape.h"
#include "general_library.h"

// Type creation
typedef struct {
  Tcoordinate_diff x, y; // Stores the difference to the center of the tetrimino
} Tmino;

// Prototypes
Tmino createMino (Tcoordinate_diff x, Tcoordinate_diff y);

#endif
