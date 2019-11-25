#ifndef __TYPE_MOVEMENT_H__
#define __TYPE_MOVEMENT_H__

#include "general_library.h"

typedef enum {MV_LEFT  = 0x01, // 0b00000001
              MV_RIGHT = 0x02, // 0b00000010
              MV_CW    = 0x04, // 0b00000100
              MV_CCW   = 0x08, // 0b00001000
              MV_SD    = 0x10, // 0b00010000
              MV_HD    = 0x20, // 0b00100000
              MV_HOLD  = 0x40  // 0b01000000
            } Tpossible_movement;

typedef short Tmovement;
// This movement type as sort of a boolean array for which keys are pressed.
// Setting and checking for inputs will work with bit masking

Tmovement createMovementWord ();
void addMovementToWord (Tmovement *mv_word, Tpossible_movement mv);
int isMovementInWord (Tmovement *mv_word, Tpossible_movement mv);

#endif
