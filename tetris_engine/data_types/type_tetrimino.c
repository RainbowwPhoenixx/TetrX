#include "type_tetrimino.h"

// Definitions for the different tetrimino shapes & rotations
// piece_minos[shape][rotation_state][mino_index][x or y]
static Tcoordinate_diff piece_minos[7][4][4][2] = {{
  {{-1,  0}, { 0,  0}, { 1,  0}, { 1,  1}}, // Initial pos for the L
  {{ 0, -1}, { 1, -1}, { 0,  0}, { 0,  1}},
  {{-1,  0}, { 0,  0}, { 1,  0}, {-1, -1}},
  {{ 0, -1}, { 0,  0}, {-1,  1}, { 0,  1}},
}, 
{
  {{-1,  0}, { 0,  0}, { 1,  0}, {-1,  1}}, // Initial pos for the J
  {{ 0, -1}, { 0,  0}, { 0,  1}, { 1,  1}},
  {{-1,  0}, { 0,  0}, { 1,  0}, { 1, -1}},
  {{ 0, -1}, { 0,  0}, { 0,  1}, {-1, -1}},
},
{
  {{-1,  0}, { 0,  0}, { 0,  1}, { 1,  1}}, // Initial pos for the S
  {{ 0,  1}, { 0,  0}, { 1,  0}, { 1, -1}},
  {{ 1,  0}, { 0,  0}, { 0, -1}, {-1, -1}},
  {{ 0, -1}, { 0,  0}, {-1,  0}, {-1,  1}},
},
{
  {{ 0,  0}, { 1,  0}, { 0,  1}, {-1,  1}}, // Initial pos for the Z
  {{ 0, -1}, { 0,  0}, { 1,  0}, { 1,  1}},
  {{ 0,  0}, {-1,  0}, { 0, -1}, { 1, -1}},
  {{ 0,  1}, { 0,  0}, {-1,  0}, {-1, -1}},
},
{
  {{ 0,  0}, { 1,  0}, { 0,  1}, { 1,  1}}, // Initial pos for the O
  {{ 0,  0}, { 1,  0}, { 0,  1}, { 1,  1}},
  {{ 0,  0}, { 1,  0}, { 0,  1}, { 1,  1}},
  {{ 0,  0}, { 1,  0}, { 0,  1}, { 1,  1}},
},
{
  {{-1,  0}, { 0,  0}, { 1,  0}, { 2,  0}}, // Initial pos for the I
  {{ 1, -1}, { 1,  0}, { 1,  1}, { 1,  2}},
  {{-1, -1}, { 0, -1}, { 1, -1}, { 2, -1}},
  {{ 0, -1}, { 0,  0}, { 0,  1}, { 0,  2}},
},
{
  {{-1,  0}, { 0,  0}, { 1,  0}, { 0,  1}}, // Initial pos for the T
  {{ 0, -1}, { 0,  0}, { 0,  1}, { 1,  0}},
  {{-1,  0}, { 0,  0}, { 1,  0}, { 0, -1}},
  {{ 0, -1}, { 0,  0}, { 0,  1}, {-1,  0}},
}};

// Accessors
Tshape getTetriminoShape (Ttetrimino *t) {
  return t->shape;
}
void setTetriminoShape (Ttetrimino *t, Tshape new_shape) {
  t->shape = new_shape;
}

Tmino *getIthMino (Ttetrimino *t, int i) {
  return &piece_minos[t->shape][t->rotation_state][i];
}

Tcoordinate getTetriminoX (Ttetrimino *t) {
  return t->x;
}
void setTetriminoX (Ttetrimino *t, Tcoordinate new_x) {
  t->x = new_x;
}

Tcoordinate getTetriminoY (Ttetrimino *t) {
  return t->y;
}
void setTetriminoY (Ttetrimino *t, Tcoordinate new_y) {
  t->y = new_y;
}

Trotation_state getTetriminoRotationState (Ttetrimino *t) {
  return t->rotation_state;
}
static void setTetriminoRotationState (Ttetrimino *t, Trotation_state rot_state) {
  t->rotation_state = rot_state;
}

Ttspin_status getTetriminoTspinStatus (Ttetrimino *t) {
  return t->tspin_status;
}
Ttspin_status setTetriminoTspinStatus (Ttetrimino *t, Ttspin_status new_status) {
  t->tspin_status = new_status;
}

// Constructor
Ttetrimino createTetrimino (Tshape tetrimino_shape) {
  Ttetrimino t;

  setTetriminoShape (&t, tetrimino_shape);

  // If the given shape is EMPTY, set coordinates to 0 and return
  if (tetrimino_shape == EMPTY) {
    setTetriminoX (&t, 0);
    setTetriminoY (&t, 0);
    return t;
  }

  // If not, init the minos and the rotation value
  setTetriminoX (&t, TETRIMINO_SPAWN_X);
  setTetriminoY (&t, TETRIMINO_SPAWN_Y);

  setTetriminoRotationState (&t, R0);
  setTetriminoTspinStatus (&t, NONE);

  return t;
}

// Useful functions

// Translation
void moveTetriminoRight (Ttetrimino *t) {
  // Moves the tetrimino 1 to the right
  setTetriminoX (t, getTetriminoX (t) + 1);
}
void moveTetriminoLeft (Ttetrimino *t) {
  // Moves the tetrimino 1 to the left
  setTetriminoX (t, getTetriminoX (t) - 1);
}
void moveTetriminoUp (Ttetrimino *t) {
  // Moves the tetrimino 1 up
  setTetriminoY (t, getTetriminoY (t) + 1);
}
void moveTetriminoDown (Ttetrimino *t) {
  // Moves the tetrimino 1 down
  setTetriminoY (t, getTetriminoY (t) - 1);
}

// Rotation
void moveTetriminoCW (Ttetrimino *t) {
  // Rotates the tetrimino clockwise
  setTetriminoRotationState (t, (getTetriminoRotationState (t)+1) % 4);
}
void moveTetriminoCCW (Ttetrimino *t) {
  // Rotates the tetrimino counter-clockwise
  setTetriminoRotationState (t, (getTetriminoRotationState (t)-1) % 4);
}

// Copy
void copyTetrimino (Ttetrimino *dest, Ttetrimino *src) {
  // Copies the src tetrimino into dest

  *dest = *src;
}
