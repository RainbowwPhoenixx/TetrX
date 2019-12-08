#ifndef __BOARD_OPERATIONS_H__
#define __BOARD_OPERATIONS_H__

#include <stdbool.h>
#include "../data_types/tetris_data_types.h"

// Board functions
bool isBoardStateValid (Tboard *b);
void popTetriminoFromQueue (Tboard *b);
void lockActiveTetrimino (Tboard *b);
void clearLines (Tboard *b);
void checkLoss (Tboard *b);

// Input handling
void applyInput (Tboard *b, Tmovement mv);

#endif
