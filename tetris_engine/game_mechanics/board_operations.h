#ifndef __BOARD_OPERATIONS_H__
#define __BOARD_OPERATIONS_H__

#include <stdbool.h>
#include "../data_types/tetris_data_types.h"

bool isBoardStateValid (Tboard *b);
void applyInput (Tboard *b, Tmovement mv);
void popTetriminoFromQueue (Tboard *b);

#endif
