#ifndef __TYPE_SHAPE_H__
#define __TYPE_SHAPE_H__

#include "general_library.h"

typedef enum {L, J, S, Z, O, I, T, EMPTY} Tshape; // Defines the possible tetrimino shapes

// Prototypes
Tshape getRandomShape ();

#endif
