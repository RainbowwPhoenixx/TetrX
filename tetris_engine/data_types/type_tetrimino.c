#include "type_tetrimino.h"

#define TETRIMINO_SPAWN_X 4
#define TETRIMINO_SPAWN_Y 20

// Definitions for the different tetrimino shapes & rotations
static Tcoordinate_diff L_initial_mino_positions[4][4][2] = { // [rotation_state][mino_index][x or y]
  {{-1,  0}, { 0,  0}, { 1,  0}, { 1,  1}}, // Initial pos for the L
  {{ 0, -1}, { 1, -1}, { 0,  0}, { 0,  1}},
  {{-1,  0}, { 0,  0}, { 1,  0}, {-1, -1}},
  {{ 0, -1}, { 0,  0}, {-1,  1}, { 0,  1}},
};
static Tcoordinate_diff J_initial_mino_positions[4][4][2] = { // [rotation_state][mino_index][x or y]
  {{-1,  0}, { 0,  0}, { 1,  0}, {-1,  1}}, // Initial pos for the J
  {{ 0, -1}, { 0,  0}, { 0,  1}, { 1,  1}},
  {{-1,  0}, { 0,  0}, { 1,  0}, { 1, -1}},
  {{ 0, -1}, { 0,  0}, { 0,  1}, {-1, -1}},
};
static Tcoordinate_diff S_initial_mino_positions[4][4][2] = { // [rotation_state][mino_index][x or y]
  {{-1,  0}, { 0,  0}, { 0,  1}, { 1,  1}}, // Initial pos for the S
  {{ 0,  1}, { 0,  0}, { 1,  0}, { 1, -1}},
  {{ 1,  0}, { 0,  0}, { 0, -1}, {-1, -1}},
  {{ 0, -1}, { 0,  0}, {-1,  0}, {-1,  1}},
};
static Tcoordinate_diff Z_initial_mino_positions[4][4][2] = { // [rotation_state][mino_index][x or y]
  {{ 0,  0}, { 1,  0}, { 0,  1}, {-1,  1}}, // Initial pos for the Z
  {{ 0, -1}, { 0,  0}, { 1,  0}, { 1,  1}},
  {{ 0,  0}, {-1,  0}, { 0, -1}, { 1, -1}},
  {{ 0,  1}, { 0,  0}, {-1,  0}, {-1, -1}},
};
static Tcoordinate_diff O_initial_mino_positions[4][4][2] = { // [rotation_state][mino_index][x or y]
  {{ 0,  0}, { 1,  0}, { 0,  1}, { 1,  1}}, // Initial pos for the O
  {{ 0,  0}, { 1,  0}, { 0,  1}, { 1,  1}},
  {{ 0,  0}, { 1,  0}, { 0,  1}, { 1,  1}},
  {{ 0,  0}, { 1,  0}, { 0,  1}, { 1,  1}},
};
static Tcoordinate_diff I_initial_mino_positions[4][4][2] = { // [rotation_state][mino_index][x or y]
  {{-1,  0}, { 0,  0}, { 1,  0}, { 2,  0}}, // Initial pos for the I
  {{ 1, -1}, { 1,  0}, { 1,  1}, { 1,  2}},
  {{-1, -1}, { 0, -1}, { 1, -1}, { 2, -1}},
  {{ 0, -1}, { 0,  0}, { 0,  1}, { 0,  2}},
};
static Tcoordinate_diff T_initial_mino_positions[4][4][2] = { // [rotation_state][mino_index][x or y]
  {{-1,  0}, { 0,  0}, { 1,  0}, { 0,  1}}, // Initial pos for the T
  {{ 0, -1}, { 0,  0}, { 0,  1}, { 1,  0}},
  {{-1,  0}, { 0,  0}, { 1,  0}, { 0, -1}},
  {{ 0, -1}, { 0,  0}, { 0,  1}, {-1,  0}},
};

static void setTetriminoMinos (Ttetrimino *t, Tbyte rotation_state) {
  // Sets the given tetrimino into the given rotation state

  // Select which tetrimino shape to use
  switch (getTetriminoShape (t)) {
    case L: setMinos(t, (Tmino*) L_initial_mino_positions[rotation_state]); break;
    case J: setMinos(t, (Tmino*) J_initial_mino_positions[rotation_state]); break;
    case S: setMinos(t, (Tmino*) S_initial_mino_positions[rotation_state]); break;
    case Z: setMinos(t, (Tmino*) Z_initial_mino_positions[rotation_state]); break;
    case O: setMinos(t, (Tmino*) O_initial_mino_positions[rotation_state]); break;
    case I: setMinos(t, (Tmino*) I_initial_mino_positions[rotation_state]); break;
    case T: setMinos(t, (Tmino*) T_initial_mino_positions[rotation_state]); break;
    case EMPTY: break;
  }
}

// Accessors
Tshape getTetriminoShape (Ttetrimino *t) {
  return t->shape;
}
void setTetriminoShape (Ttetrimino *t, Tshape new_shape) {
  t->shape = new_shape;
}

Tmino *getIthMino (Ttetrimino *t, int i) {
  return &(t->minos[i]);
}
void setMinos (Ttetrimino *t, Tmino *new_minos) {
  t->minos = new_minos;
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

  setTetriminoMinos (&t, R0);
  setTetriminoRotationState (&t, R0);

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
  setTetriminoMinos (t, getTetriminoRotationState (t));
}
void moveTetriminoCCW (Ttetrimino *t) {
  // Rotates the tetrimino counter-clockwise
  setTetriminoRotationState (t, (getTetriminoRotationState (t)-1) % 4);
  setTetriminoMinos (t, getTetriminoRotationState (t));
}

// Copy
void copyTetrimino (Ttetrimino *dest, Ttetrimino *src) {
  // Copies the src tetrimino into dest

  setTetriminoShape (dest, getTetriminoShape (src));
  setTetriminoX (dest, getTetriminoX (src));
  setTetriminoY (dest, getTetriminoY (src));

  Trotation_state rotation = getTetriminoRotationState (src);
  setTetriminoRotationState (dest, rotation);
  setTetriminoMinos (dest, rotation);
}
